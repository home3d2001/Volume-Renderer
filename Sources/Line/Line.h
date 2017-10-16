#pragma once

#ifndef OpenCL
#ifdef __APPLE__
#include <OpenCL/cl_platform.h>
#else
#include "CL/cl_platform.h"
#endif
#endif

#include "ParallelAlgorithms/defs.h"

/* 16 bytes */
typedef struct Line {
  cl_int first;
  cl_int second;
  cl_short color;
  cl_short level;
} Line;
