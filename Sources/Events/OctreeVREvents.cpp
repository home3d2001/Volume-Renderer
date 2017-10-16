/*
* Left click - Draw a line
* Right click - Pan the camera
* Scroll - Zoom in or out
* R - Reset the camera
* P - Toggle quadtree pruning
* X - Show intersections
* C - Clear all lines.
* V - Show line vertices
* L - Show lines
* O - Show octree
* U - Undo a line
* Q - Show quantized lines
* S - Save lines to new dataset
* H - Hide/Show instructions
*/
#include "OctreeVREvents.h"
#include "mouse.h"
#include "glm/ext.hpp"

using namespace GLUtilities;
using namespace Events;

#define DOWN true
#define UP false

static MouseData md;
static VRData vrd;

static void X(bool down) {
	if (down) {
		cout << "Toggling Intersections" << endl;
		Options::showObjectIntersections = !Options::showObjectIntersections;
		//Data::octree->build(Data::lines2D);
		RedrawScene();
		Refresh();
	}
}

static void V(bool down) {
	if (down) {
		cout << "Toggling Vertices" << endl;
		Options::showObjectVertices = !Options::showObjectVertices;
		Options::showResolutionPoints = !Options::showResolutionPoints;
		//Data::octree->build(Data::lines2D);
		RedrawScene();
		Refresh();
	}
}

static void L(bool down) {
	if (down) {
		cout << "Toggling lines" << endl;
		Options::showObjects = !Options::showObjects;
		RedrawScene();
		Refresh();
	}
}

static void C(bool down) {
	if (down) {
		cout << "Clearing!" << endl;
		Data::lines->clear();

		RedrawScene();
		Refresh();
	}
}

static void P(bool down) {
	if (down) {
		cout << "Toggling pruning" << endl;
		Options::pruneOctree = !Options::pruneOctree;
		//Data::octree->build(Data::lines2D);
		RedrawScene();
		Refresh();
	}
}

static void K(bool down) {
	if (down) {
		cout << "Toggling Karras-only octree" << endl;
		Options::resolveAmbiguousCells = !Options::resolveAmbiguousCells;
		//Data::octree->build(Data::lines2D);
		RedrawScene();
		Refresh();
	}
}

static void O(bool down) {
	if (down) {
		cout << "Toggling Octree!" << endl;
		Options::showOctree = !Options::showOctree;
		RedrawScene();
		Refresh();
	}
}

static void Z(bool down) {
}

static void R(bool down) {
	if (down) {
		Data::octree->build(Data::lines.get());
	}
}

static void Q(bool down) {
	if (down) {
		cout << "Showing Quantized Lines" << endl;
		Options::showQuantizedObjects = !Options::showQuantizedObjects;
	}
}

static void I(bool down) {
	if (down) {
		cout << "Toggling Instructions" << endl;
		Options::showInstructions = !Options::showInstructions;
	}
}

static void U(bool down) {
	if (down) {
		cout << "Undoing line" << endl;
		Data::lines->undoLine();
		//Data::octree->build(Data::lines2D);
	}
	RedrawScene();
	Refresh();
}

static void S(bool down) {
	if (down) {
		cout << "Saving lines to folder" << endl;
		Data::lines->write3DToFile("./TestData/TestData/temp");
	}
}

static void Up(bool down) {
	if (down) {
		Options::maxConflictIterations++;
		Data::octree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void Down(bool down) {
	if (down) {
		if (Options::maxConflictIterations > 0)
			Options::maxConflictIterations--;
		Data::octree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void RTrigger(bool initial) {
	glm::vec3 point = glm::vec3(vrd.rightMatrix[3]);
	if (!initial) {
		Data::lines->addPoint(make_float3(point.x, point.y, point.z));
	}
	else {
		Data::lines->newLine(make_float3(point.x, point.y, point.z));
	}
	Data::octree->build(Data::lines.get());
	Events::RedrawScene();
}

static void LMenu() {
	glm::vec3 point = glm::vec3(vrd.rightMatrix[3]);
	if (((vrd.leftControllerPreviousState.ulButtonPressed >> 1ull) & vr::EVRButtonId::k_EButton_ApplicationMenu) != 0) {
		//Data::lines3D->addPoint(make_float3(point.x, point.y, point.z));
	}
	else {
		Data::lines->write3DToFile("./TestData/TestData/temp");
	}
}

static void RGrip(bool initial) {
	glm::vec3 point = glm::vec3(vrd.rightMatrix[3]);
	if (initial) {
		Data::lines->addPoint(make_float3(point.x, point.y, point.z));
	}
	else {
		Data::lines->replaceLast(make_float3(point.x, point.y, point.z));
	}
	Data::octree->build(Data::lines.get());
}

static void LGrip() {
	glm::vec3 point = glm::vec3(vrd.rightMatrix[3]);
	if (!((vrd.rightControllerPreviousState.ulButtonPressed >> 1ull) == vr::EVRButtonId::k_EButton_Grip)) {
		Data::lines->newLine(make_float3(point.x, point.y, point.z));
	}
	else {
		Data::lines->replaceLast(make_float3(point.x, point.y, point.z));
	}
	Data::octree->build(Data::lines.get());
}

static void RTouchClick(bool initial) {
	if (initial) {
		Data::lines->clear();
		Data::octree->build(Data::lines.get());
	}
}

static void RMenuClick(bool initial) {
	if (initial) {
		Data::lines->undoLine();
	}
}

void Events::key_cb(GLFWwindow* window, int key, int scancode, int action, int mods) {
	bool down = action == GLFW_PRESS || action == GLFW_REPEAT;
	switch (key) {
	case GLFW_KEY_S: S(down);
		break;
	case GLFW_KEY_C: C(down);
		break;
	case GLFW_KEY_I: I(down);
		break;
	case GLFW_KEY_P: P(down);
		break;
	case GLFW_KEY_O: O(down);
		break;
	case GLFW_KEY_Z: Z(down);
		break;
	case GLFW_KEY_R: R(down);
		break;
	case GLFW_KEY_Q: Q(down);
		break;
	case GLFW_KEY_U: U(down);
		break;
	case GLFW_KEY_X: X(down);
		break;
	case GLFW_KEY_V: V(down);
		break;
	case GLFW_KEY_L: L(down);
		break;
	case GLFW_KEY_K: K(down);
		break;
	case GLFW_KEY_UP: Up(down);
		break;
	case GLFW_KEY_DOWN: Down(down);
		break;
	}
}

#define checkVREvent(SIDE, BUTTONID) (vrd. ## SIDE ## ControllerState.ulButtonPressed == vr::ButtonMaskFromId(vr::EVRButtonId:: ## BUTTONID ##))
#define checkPreviousVREvent(SIDE, BUTTONID) (vrd. ## SIDE ## ControllerPreviousState.ulButtonPressed == vr::ButtonMaskFromId(vr::EVRButtonId:: ## BUTTONID ##))

void Events::HandleVRInput() {
	if (checkVREvent(right, k_EButton_SteamVR_Trigger)) {
		bool initial = !checkPreviousVREvent(right, k_EButton_SteamVR_Trigger);
		RTrigger(initial);
	}

	if (checkVREvent(right, k_EButton_Grip)) {
		bool initial = !checkPreviousVREvent(right, k_EButton_Grip);
		RGrip(initial);
	}

	if (checkVREvent(right, k_EButton_SteamVR_Touchpad)) {
		bool initial = !checkPreviousVREvent(right, k_EButton_SteamVR_Touchpad);
		RTouchClick(initial);
	}

	if (checkVREvent(right, k_EButton_ApplicationMenu)) {
		bool initial = !checkPreviousVREvent(right, k_EButton_ApplicationMenu);
		RMenuClick(initial);
	}
	//switch () {
	//		break;
	//	}
	//	case vr::EVRButtonId::k_EButton_Grip: {
	//		RGrip();
	//		break;
	//	}
	//	case vr::EVRButtonId::k_EButton_SteamVR_Touchpad: {
	//		RTouchClick();
	//		break;
	//	}
	//}

	////Left controller
	//switch (vrd.leftControllerState.ulButtonPressed >> 1ull) {
	//	case vr::EVRButtonId::k_EButton_ApplicationMenu: {
	//		LMenu();
	//		break;
	//	}
	//	case vr::EVRButtonId::k_EButton_Grip: {
	//		LGrip();
	//		break;
	//	}
	//}
}

void Events::Initialize() {
	using namespace GLUtilities;
	using namespace Data;

	/* Setup VR Headset */
	vr::HmdError peError;
	vrd.m_pHMD = VR_Init(&peError, vr::EVRApplicationType::VRApplication_Scene);
	if (peError != vr::HmdError::VRInitError_None) cout << peError << endl;
	if (peError == vr::HmdError::VRInitError_Init_HmdNotFound) {
		cout << "HTC vive seems to be unplugged. Please double check that all vive cables \
		are connected properly, and then rerun the program.";
		glfwDestroyWindow(GLUtilities::window);
		return;
	}
	SetupStereoRenderTargets();
	SetupCameras();
	
	/* Render instructions to scene */
	string file = ".\\Sources\\2d_quadtree_instructions.png";
	Sketcher::instance()->uploadImage(file, "instructions");
	Plane p = {};
	p.width = .5;
	p.height = .5;
	p.offset = glm::vec3(0.0);
	p.texName = "instructions";
	Sketcher::instance()->add(p);
	
	/* Build quadtree and add to scene */
	Data::octree->build(Data::lines.get());

	/* Create hands*/
	Data::LHand->add({ 0.0f,0.0f,0.025f,1.0f }, { .001f, .001f, .025f, 1.0f }, { 1.0f,0.0f,0.0f,1.0f });
	Data::LHand->add({ 0.0f,0.0f,0.1f,1.0f }, { .02f, .02f, .05f, 1.0f }, { 1.0f,0.0f,0.0f,1.0f });

	Data::RHand->add({ 0.0f,0.0f,0.025f,1.0f }, { .001f, .001f, .025f, 1.0f }, { 0.0f,1.0f,0.0f,1.0f });
	Data::RHand->add({ 0.0f,0.0f,0.1f,1.0f }, { .02f, .02f, .05f, 1.0f }, { 0.0f,1.0f,0.0f,1.0f });
}

void Events::RedrawScene() {
	using namespace Data;
	//Sketcher::instance()->clear();
	//Sketcher::instance()->add(*octree);
	//Sketcher::instance()->add(*lines3D);
}

void Events::Update() {
	updateVRData();
	receiveVRInput();
	HandleVRInput();

	/* Move hands */
	Data::LHand->matrix = vrd.leftMatrix;
	Data::RHand->matrix = vrd.rightMatrix;
}

void Events::Refresh() {
	using namespace GLUtilities;

	/* Update Scene */
	Update();

	/* Draw objects on left eye render buffer */
	glBindFramebuffer(GL_FRAMEBUFFER, vrd.leftEyeDesc.m_nRenderFramebufferId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight);
	Render(vr::Eye_Left);

	/* Copy from render to resolve buffer */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, vrd.leftEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, vrd.leftEyeDesc.m_nResolveFramebufferId);
	glBlitFramebuffer(0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight, 0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);
	
	/* Draw objects on right eye */
	glBindFramebuffer(GL_FRAMEBUFFER, vrd.rightEyeDesc.m_nRenderFramebufferId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight);
	Render(vr::Eye_Right);

	/* Copy from render to resolve buffer */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, vrd.rightEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, vrd.rightEyeDesc.m_nResolveFramebufferId);
	glBlitFramebuffer(0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight, 0, 0, vrd.m_nRenderWidth, vrd.m_nRenderHeight,
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);
	
	/* Submit textures to OpenVR Compositor */
	vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)vrd.leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)vrd.rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
}

void Events::Render(vr::Hmd_Eye eye) {
	glm::mat4 VP = GetCurrentViewProjectionMatrix(eye);
	Scene::render(VP);
	//Sketcher::instance()->drawBoxes(VP);
	//Sketcher::instance()->drawLines(VP);
	//Sketcher::instance()->drawPoints(VP);
	//if (Options::showInstructions)
	//	Sketcher::instance()->drawPlane("instructions", 0, VP);
}

