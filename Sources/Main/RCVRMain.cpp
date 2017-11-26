#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <iostream>
#include "GLUtilities/gl_utils.h"
#include "clfw.hpp"
#include "Shaders/Shaders.hpp"
#include "Options/Options.h"
#include "Entities/Entity.h"
#include "Entities/Cameras/OrbitCamera.h"
#include "Entities/Boxes/Boxes.hpp"
#include "Entities/GUI/2D_Desktop_TransferFunction/TransferFunctionGUI.hpp"
#include "Entities/RaycastVolume/RaycastVolume.hpp"
#include "Entities/SlicedVolume/SlicedVolume.hpp"
#include "Entities/ImagePlane/ImagePlane.hpp"
#include "GLUtilities/texture.hpp"

#include <thread>
#include <atomic>

using namespace Entities;

namespace GLFW {
	void InitializeGLFW(int width = 1920, int height = 1080) {
		using namespace GLUtilities;
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;
		restart_gl_log();
		start_gl();

		print_gl_error();
		glfwSetWindowTitle(window, "Slice Based Volume Renderer");

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glDisable(GL_CULL_FACE);
		
		glfwSwapBuffers(window);
		glfwSwapInterval(0);
	}
	
	void Initialize() {
		InitializeGLFW();

		/* Hide the GLFW window until CL device is chosen */
		glfwHideWindow(GLUtilities::window);
	}
}

int processArgs(int argc, char** argv) {
	using namespace Options;

	int i = 1;
	bool stop = false;
	while (i < argc && !stop) {
		stop = true;
		if (processArg(i, argv)) {
			stop = false;
		}
	}

	for (; i < argc; ++i) {
		string filename(argv[i]);
		filenames.push_back(filename);
	}
	//TODO: fix this.
	return 0;
}

Entity World = Entity();
Entity Screen = Entity();

std::atomic_bool quit = false;

int framerate = 60;
int updaterate = 60;

void update() {
	auto lastTime = glfwGetTime();
	while (quit == false) {
		auto currentTime = glfwGetTime();
		if (currentTime - lastTime > 1.0 / updaterate) {
			World.update();
			Screen.update();
			lastTime = currentTime;
		}
	}
}

void castRay() {
	glm::mat4 orthoproj = glm::ortho(-1, 1, -1, 1, -1, 1);

	glm::mat4 inverseOrtho = inverse(orthoproj);

	auto lastTime = glfwGetTime();
	while (quit == false) {
		auto currentTime = glfwGetTime();
		if (currentTime - lastTime > 1.0 / updaterate) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			//xpos = 0, ypos = 0;
			if (xpos >= 0 && ypos >= 0 && xpos < window_width && ypos < window_height) {
				lastTime = currentTime;

				glm::vec4 sp = glm::vec4(1.0 - (xpos / window_width), ypos / window_height, 1.0, 0.0);
				sp *= 2.0; sp -= 1.0;
				glm::vec4 sd = glm::vec4(0.0, 0.0, -1.0, 0.0);

				sp = inverseOrtho * sp;
				Screen.raycast(sp, sd);

				/* Todo: cast ray into world */
			}
		}
	}
}

void render() {
	using namespace std;
	using namespace GLUtilities;
	using namespace Entities;

	shared_ptr<OrbitCamera> camera = 
		dynamic_pointer_cast<OrbitCamera>(World.objects.at("Camera"));
	shared_ptr<OrbitCamera> altCamera =
		dynamic_pointer_cast<OrbitCamera>(World.objects.at("AltCamera"));
	
	glm::mat4 orthoproj = glm::ortho(-1, 1, -1, 1, -1, 1);

	auto lastTime = glfwGetTime();
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		auto currentTime = glfwGetTime();
		if (currentTime - lastTime > 1.0 / framerate) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glfwPollEvents();
			if (camera->id == camera->selected)
				World.render(camera->VM, camera->P);
			else if (altCamera->id == camera->selected) {
				World.render(altCamera->VM, altCamera->P);
			}

			Screen.render(orthoproj);
			glfwSwapBuffers(GLUtilities::window);
			lastTime = currentTime;
		}
	}
}

int main(int argc, char** argv) {
	using namespace std;
	using namespace GLUtilities;
	using namespace Entities;

	processArgs(argc, argv);

	/* Initialize OpenGL */
	GLFW::Initialize();
	Shaders::create();

	/* Initialize OpenCL */
	CLFW::BuildOptions = Options::cl_options;
	CLFW::NumQueues = 2;
	CLFW::GLInterop = true;
	CLFW::ChosenDevice = Options::computeDevice;
	if (CLFW::Initialize() != CL_SUCCESS) {
		glfwTerminate();
		getchar();
		return -1;
	}
	else
		glfwShowWindow(window);
	
	/* Create Cameras*/
	shared_ptr<OrbitCamera> camera = make_shared<OrbitCamera>(glm::vec3(0.0, 5.0, 0.0));
	shared_ptr<OrbitCamera> altCamera = make_shared<OrbitCamera>(glm::vec3(0.0, 10.0, 0.0));
	camera->setWindowSize(GLUtilities::window_width, GLUtilities::window_height);
	altCamera->setWindowSize(GLUtilities::window_width, GLUtilities::window_height);
	
	/* Create Transfer Function Editor */
	shared_ptr<TransferFunctionGUI> TFGUI = 
		make_shared<TransferFunctionGUI>(GLUtilities::window_width, GLUtilities::window_height, Options::bytesPerPixel);

	/* Create Volume */
	int3 dimensions = make_int3(Options::volumeWidth, Options::volumeHeight, Options::volumeDepth);
	shared_ptr<RaycastVolume> raycastVolume = make_shared<RaycastVolume>(Options::volumeLocation, dimensions, 
		Options::bytesPerPixel, camera, Options::totalSamples);
	raycastVolume->transform.SetScale(glm::vec3(.5, .5, .5));

	/* Connect the volume with the GUI */
	raycastVolume->setTransferFunction(TFGUI->getTransferFunction());
	TFGUI->setHistogramTexture(raycastVolume->getHistogramTexture());
	
	/* Add entities to World and Screen */
	Screen.add("TransferFunctionGUI", TFGUI);
	World.add("Volume", raycastVolume);
	World.add("Camera", camera);
	World.add("AltCamera", altCamera);
	
	/* Handle scene evens on seperate thread */
	thread updateThread = thread(update);
	thread castRayThread = thread(castRay);

	/* Render on the current thread */
	render();
	
	/* Quit */
	quit = true;
	updateThread.join();
	castRayThread.join();
	glfwTerminate();
	return 0;
}
