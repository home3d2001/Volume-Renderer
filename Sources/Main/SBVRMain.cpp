#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <iostream>
#include "GLUtilities/gl_utils.h"
#include "clfw.hpp"
#include "Shaders/Shaders.hpp"
#include "Options/Options.h"
#include "Entities/Cameras/OrbitCamera.h"
#include "Entities/Boxes/Boxes.hpp"
#include "Entities/SlicedVolume/SlicedVolume.hpp"
#include "Entities/ImagePlane/ImagePlane.hpp"

#include <thread>
#include <atomic>

namespace GLFW {
	void InitializeGLFW(int width = 1920, int height = 1080) {
		using namespace GLUtilities;
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;
		restart_gl_log();
		start_gl();

		print_gl_error();
		glfwSetWindowTitle(window, "Slice Based Volume Renderer");

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		
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

int framerate = 120;
int updaterate = 60;

void updateEntities() {
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

void renderEntities() {
	using namespace GLUtilities;
	using namespace std;

	shared_ptr<OrbitCamera> camera = 
		dynamic_pointer_cast<OrbitCamera>(World.objects.at("Camera"));
	shared_ptr<OrbitCamera> altCamera =
		dynamic_pointer_cast<OrbitCamera>(World.objects.at("AltCamera"));
	
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
			Screen.render();
			glfwSwapBuffers(GLUtilities::window);
			lastTime = currentTime;
		}
	}
}



int main(int argc, char** argv) {
	using namespace std;
	using namespace GLUtilities;

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
	
	/* Setup the scene */
	shared_ptr<OrbitCamera> camera = make_shared<OrbitCamera>(glm::vec3(0.0, 5.0, 0.0));
	shared_ptr<OrbitCamera> altCamera = make_shared<OrbitCamera>(glm::vec3(0.0, 10.0, 0.0));
	camera->setWindowSize(GLUtilities::window_width, GLUtilities::window_height);
	altCamera->setWindowSize(GLUtilities::window_width, GLUtilities::window_height);

	std::shared_ptr<Boxes3D> boxes = make_shared<Boxes3D>();
	boxes->add({ 0.0, 0.0, 0.0, 1.0 }, { 0.5, 0.5, 0.5, 1.0 }, { 1.0, 0.0, 0.0, 1.0 });
	shared_ptr<ImagePlane> transferFunction = make_shared<ImagePlane>("BWTransferFunction.png");
	transferFunction->M = glm::translate(transferFunction->M, glm::vec3(-1.0, -1.0, 0.0));
	transferFunction->M = glm::scale(transferFunction->M, glm::vec3(1/4096.0 * 2.0, 1 / 256.0 * .1, 1.0));
	shared_ptr<SlicedVolume> volume = make_shared<SlicedVolume>("Sheep.raw", make_int3(352, 352, 256), 1, camera, transferFunction, 16);

	
	World.add("BoundingBox", boxes);
	World.add("Camera", camera);
	World.add("AltCamera", altCamera);
	World.add("Volume", volume);
	
	World.add("TransferFunction", transferFunction);

	/* Handle scene evens on seperate thread */
	thread updateThread = thread(updateEntities);

	/* Render on the current thread */
	renderEntities();
	
	/* Quit */
	quit = true;
	updateThread.join();
	glfwTerminate();
	return 0;
}
