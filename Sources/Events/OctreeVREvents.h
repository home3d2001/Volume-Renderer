#pragma once
#include "GLUtilities/gl_utils.h"
#include "GLUtilities/PerspectiveCamera.h"
#include "Options/Options.h"
#include "GlobalData/data.h"
#include "Shaders/Shaders.hpp"
#include "openvr.h"
#include "vr.h"
#include "GLUtilities/Scene.h"

namespace Events {
	void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
	void HandleVRInput();
	void Initialize();
	void RedrawScene();
	void Update();
	void Refresh();
	void Render(vr::Hmd_Eye eye);
}