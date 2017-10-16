/* OpenCL/C++ */
#pragma once
#ifndef OpenCL
#include "clfw.hpp"
#endif
#include "Vector/vec.h"

struct Box 
{
	float4 center;
	float4 color;
	float4 scale;
};

typedef struct Box Box;