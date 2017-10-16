/*******************************************************
 ** Generalized Voronoi Diagram Project               **
 ** Copyright (c) 2015 John Martin Edwards            **
 ** Scientific Computing and Imaging Institute        **
 ** 72 S Central Campus Drive, Room 3750              **
 ** Salt Lake City, UT 84112                          **
 **                                                   **
 ** For information about this project contact        **
 ** John Edwards at                                   **
 **    edwardsjohnmartin@gmail.com                    **
 ** or visit                                          **
 **    sci.utah.edu/~jedwards/research/gvd/index.html **
 *******************************************************/

#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

 //------------------------------------------------------------------------------
 // Options
 //------------------------------------------------------------------------------
namespace Options {
  extern int computeDevice;
  extern std::vector<std::string> filenames;
  extern unsigned char max_level;
  extern bool series;

  extern float bbxmin, bbymin, bbxmax, bbymax;
  
  // Render settings
  extern bool showObjectVertices;
	extern bool showObjects;
	extern bool showQuantizedObjects;
  extern bool showOctree;
  extern bool showObjectIntersections;
	extern bool showResolutionPoints;
	extern bool showInstructions;
	extern bool pruneOctree;
	extern bool resolveAmbiguousCells;

  extern float conflict_color[3];

  extern int maxConflictIterations;

  extern bool zoomMode;
  extern bool debug;
  extern bool benchmarking;
  extern std::string cl_options;
	extern bool fullScreen;

  bool processArg(int& i, char** argv);
};