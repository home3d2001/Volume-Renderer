#include <cstdlib>
#include <fstream>
#include "./Options.h"

//------------------------------------------------------------------------------
// Options
//------------------------------------------------------------------------------
namespace Options {
	int computeDevice = -1;
	std::string volumeLocation = "Sheep.raw";
	int volumeWidth = 352, volumeHeight = 352, volumeDepth = 256;
	int bytesPerPixel = 1;
	int totalSamples = 256;
	std::vector<std::string> filenames;
	bool series = false; //Not implemented

// Render settings
	bool fullScreen = false;
	bool debug = true;
	bool benchmarking = false;
	bool showObjectVertices = true;
	bool showTransferFunctionGUI = true;
	std::string cl_options = "-cl-std=CL1.2 -w -I ./Sources -D OpenCL";//"-x spir -cl-std=CL2.0";

#define $(flag) (strcmp(argv[i], flag) == 0)
	bool processArg(int& i, char** argv) {
	int orig_i = i;
	if $("-d") {
		printf("Setting device\n");
		++i;
		computeDevice = atoi(argv[i]);
		++i;
	}
	else if $("-v") {
		++i;
		debug = true;
	}
	else if $("--benchmarking") {
		++i;
		benchmarking = true;
	}
	else if $("--samples") {
		++i;
		totalSamples = atoi(argv[i]);
		++i;
	}
	else if $("-o") {
		++i;
		cl_options += "-cl-opt-disable ";
	}
	else if $("--volume") {
		++i;
		volumeLocation = argv[i];
		++i;
	}
	else if $("--dimensions") {
		++i;
		volumeWidth = atoi(argv[i]);
		++i;
		volumeHeight = atoi(argv[i]);
		++i;
		volumeDepth = atoi(argv[i]);
		++i;
	}
	else if $("--bytesPerPixel") {
		++i;
		bytesPerPixel = atoi(argv[i]);
		++i;
	}
	else if $("-f") {
		++i;
		fullScreen = true;
	}
	return i != orig_i;
	}
#undef $
}
