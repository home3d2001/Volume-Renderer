#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "clfw.hpp"
#include "GLUtilities/gl_utils.h"
#include "GLUtilities/Sketcher.h"
#include "Shaders/Shaders.hpp"
#include <cstdlib>
#include "Tests/HelperFunctions.hpp"

extern "C" {
#include "BinaryRadixTree/BuildBRT.h"
}

void fixResolution(int& width, int& height) {
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = min(width, mode->width);
	height = min(height, mode->height);
	if (width < height) {
		height = width;
	}
	else {
		width = height;
	}
}

void InitializeGLFW(int width = 1000, int height = 1000) {
	using namespace GLUtilities;
	GLUtilities::window_height = height;
	GLUtilities::window_width = width;
	restart_gl_log();
	start_gl();
	fixResolution(width, height);
	GLUtilities::window_height = height;
	GLUtilities::window_width = width;

	print_gl_error();
	glfwSetWindowTitle(window, "Parallel GVD");
	//InitializeGLFWEventCallbacks();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glfwSwapBuffers(window);
	glfwSwapInterval(0);

	glfwHideWindow(GLUtilities::window);
}

int processArgs(int argc, char** argv, std::vector<char*> &remainingArgs) {
	using namespace Options;

	int i = 1;
	bool stop = false;
	while (i < argc && !stop) {
		stop = true;
		if (processArg(i, argv)) {
			stop = false;
		}
	}

	remainingArgs.push_back(argv[0]);
	for (; i < argc; ++i) {
		remainingArgs.push_back(argv[i]);
	}

	return 0;
}

int main(int argc, char** argv)
{
	InitializeGLFW();
	Shaders::create();
	std::vector<char*> resultingArgv;
	processArgs(argc, argv, resultingArgv);

	/* global setup... */
	CLFW::Verbose = false;
	CLFW::ChosenDevice = Options::computeDevice;
	CLFW::BuildOptions = Options::cl_options;
	CLFW::NumQueues = 2;
	CLFW::GLInterop = true;
	CLFW::Initialize();

	int result = Catch::Session().run(resultingArgv.size(), &resultingArgv[0]);

	glfwTerminate();

	/* global clean-up... */
	cout << "Press enter to continue..." << endl;
	cin.get();
	return result;
}