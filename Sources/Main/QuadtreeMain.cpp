#include <iostream>
#include "GLUtilities/gl_utils.h"
#include "clfw.hpp"
#include "Events/QuadtreeEvents.h"
#include "Shaders/Shaders.hpp"
#include "GLUtilities/Polylines/Polylines.h"
#include "GLUtilities/Polygons.h"
#include "GLUtilities/Scene.h"
#include "GlobalData/data.h"
#include "Options/Options.h"

namespace GLFW {
	void InitializeGLFWEventCallbacks() {
		using namespace GLUtilities;
		glfwSetKeyCallback(window, Events::key_cb);
		glfwSetMouseButtonCallback(window, Events::mouse_cb);
		glfwSetCursorPosCallback(window, Events::mouse_move_cb);
		glfwSetWindowSizeCallback(window, Events::resize_cb);
		glfwSetWindowFocusCallback(window, Events::focus_cb);
		glfwSetScrollCallback(window, Events::scroll_cb);
	}

	void InitializeGLFW(int width = 1400, int height = 1400) {
		using namespace GLUtilities;
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;
		restart_gl_log();
		start_gl();
		GLUtilities::window_height = height;
		GLUtilities::window_width = width;

		print_gl_error();
		glfwSetWindowTitle(window, "Quadtree Demo");
		InitializeGLFWEventCallbacks();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
		glDisable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glfwSwapBuffers(window);
		glfwSwapInterval(0);
	}
	
	void Initialize() {
		InitializeGLFW();
		InitializeGLFWEventCallbacks();
		/* Hide the window until CLFW is initialized*/
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
	processArgs(argc, argv);

	GLFW::Initialize();
	Shaders::create();
	
	CLFW::Verbose = false;
	CLFW::ChosenDevice = Options::computeDevice;
	CLFW::BuildOptions = Options::cl_options;
	CLFW::NumQueues = 2;
	CLFW::GLInterop = true;
	if (CLFW::Initialize() != CL_SUCCESS) {
		getchar();
		return -1;
	}
	else {
		glfwShowWindow(GLUtilities::window);
	}
	
	Data::lines = make_shared<PolyLines>(Options::filenames, false);
	Data::quadtree = make_shared<Quadtree>();
	Data::instructions = make_shared<ImagePlane>(".\\Sources\\2d_quadtree_instructions.png");
	Scene::add("polylines", Data::lines);
	Scene::add("quadtree", Data::quadtree);
	Scene::add("instructions", Data::instructions);
	Events::Initialize();

	/* Event loop */
	int framerate = 120;
	auto lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(GLUtilities::window)) {
		auto currentTime = glfwGetTime();
		if (currentTime - lastTime > 1.0 / framerate) {
			glfwPollEvents();
			Events::Refresh();
			lastTime = currentTime;
		}
	}
	glfwTerminate();
	return 0;
}
