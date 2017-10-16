/*

*/

#include "SBVREvents.h"
#include "mouse.h"
#include "GLUtilities/Scene.h"

using namespace GLUtilities;
using namespace Events;

#define DOWN true
#define UP false

static MouseData md;
static GLFWcursor* crossHairCursor;

std::shared_ptr<OrbitCamera> camera;

void Left(int action) {
	bool down = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
	/*if (down)
		PerspectiveCamera::position += glm::vec3(0.0, 0.1, 0.0);*/
}

void Events::HandleKeys() {

	Left(glfwGetKey(GLUtilities::window, GLFW_KEY_LEFT));
}

void Events::Initialize() {
	/* Look up */
	RedrawScene();
	//PerspectiveCamera::reset();
}

void Events::RedrawScene() {
	//Data_OLD::boundingBox->add({ 0.0, 0.0, 0.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 0.0, 0.0, 1.0 });
}

float angle = 0.0f;
void Events::Refresh() {
	using namespace GLUtilities;

	/* Clear stuff */

	//PerspectiveCamera::refresh();

	HandleKeys();

	//float distance = 3.0;
	//PerspectiveCamera::setPosition(glm::vec3(distance * sin(angle), 0.0, distance * cos(angle)));
	//angle += .01;
	//Scene_OLD::render(VP);
	/* Draw objects */
	//Sketcher::instance()->drawBoxes(VP);
	//Sketcher::instance()->drawLines(VP);
	//Sketcher::instance()->drawPoints(VP);

	//if (Options::showInstructions)
	//	Sketcher::instance()->drawPlane("instructions", 0, glm::mat4(1.0));
	

}

