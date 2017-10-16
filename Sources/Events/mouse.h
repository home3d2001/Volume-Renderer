#pragma once
#include "glfw/glfw3.h"

struct MouseData {
  GLFWcursor* cursor;
	bool leftDown;
	bool rightDown;
	bool middleDown;
	float x;
	float y;
	int mods;
};