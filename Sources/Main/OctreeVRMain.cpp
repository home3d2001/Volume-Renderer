#include <iostream>
#include "GLUtilities/gl_utils.h"
#include "clfw.hpp"
#include "Events/OctreeEvents.h"
#include "Shaders/Shaders.hpp"
#include "GLUtilities/Polylines/Polylines.h"
#include "GLUtilities/Polygons.h"
#include "GlobalData/data.h"
#include "Options/Options.h"
#include "openvr.h"

#include "GLUtilities/Scene.h"

namespace GLFW {
	void InitializeGLFWEventCallbacks() {
		using namespace GLUtilities;
		glfwSetKeyCallback(window, Events::key_cb);
	}

	void InitializeGLFW(int width = 260, int height = 1) {
		using namespace GLUtilities;
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;
		restart_gl_log();
		start_gl();
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;

		print_gl_error();
		glfwSetWindowTitle(window, "Octree VR Demo");
		InitializeGLFWEventCallbacks();

		glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}
	
	void Initialize() {
		InitializeGLFW();
		InitializeGLFWEventCallbacks();
		/* Hide the GLFW window until CL device is chosen.*/
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
	return 0;
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
	if (CLFW::Initialize() != CL_SUCCESS) {
		glfwTerminate();
		getchar();
		return -1;
	}
	else 
		glfwShowWindow(GLUtilities::window);
	
	/* Setup the scene */
	Data::lines = make_shared<PolyLines>(Options::filenames, true);
	Data::boxes3D = make_shared<Boxes3D>();
	Data::LHand = make_shared<Boxes3D>();
	Data::RHand = make_shared<Boxes3D>();
	Data::octree = make_shared<Octree>();
	Scene::add("Handy Boxes", Data::boxes3D);
	Scene::add("Polylines", Data::lines);
	Scene::add("LHand", Data::LHand);
	Scene::add("RHand", Data::RHand);
	Scene::add("Octree", Data::octree);

	/* Handle events */
	int framerate = 120;
	Events::Initialize();
	auto lastTime = glfwGetTime();
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		auto currentTime = glfwGetTime();
		if (currentTime - lastTime > 1.0 / framerate) {
			Events::Refresh();
			glfwPollEvents();
			lastTime = currentTime;
		}
	}

	/* Quit */
	glfwTerminate();
	vr::VR_Shutdown();
	return 0;
}
