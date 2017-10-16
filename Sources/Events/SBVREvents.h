#pragma once
#include "GLUtilities/gl_utils.h"
#include "GLUtilities/OrbitCamera.h"
#include "Options/Options.h"
#include "Shaders/Shaders.hpp"

namespace Events {
	void HandleKeys();
	//void mouse_cb(GLFWwindow* window, int button, int action, int mods);
	//void mouse_move_cb(GLFWwindow* window, double xpos, double ypos);
	//void scroll_cb(GLFWwindow* window, double xoffset, double yoffset);
	//void resize_cb(GLFWwindow* window, int width, int height);
	//void focus_cb(GLFWwindow* window, int focused);
	void Initialize();
	void RedrawScene();
	void Refresh();
}