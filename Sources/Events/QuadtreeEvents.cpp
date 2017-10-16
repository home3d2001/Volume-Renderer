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
#include "QuadtreeEvents.h"
#include "mouse.h"

using namespace GLUtilities;
using namespace Events;

#define DOWN true
#define UP false

static MouseData md;
static GLFWcursor* crossHairCursor;
static floatn point1 = make_floatn(-1.0, -1.0);
static floatn point2 = make_floatn(1.0, 1.0);

static void X(bool down) {
	if (down) {
		cout << "Toggling Intersections" << endl;
		Options::showObjectIntersections = !Options::showObjectIntersections;
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void V(bool down) {
	if (down) {
		cout << "Toggling Vertices" << endl;
		Options::showObjectVertices = !Options::showObjectVertices;
		Options::showResolutionPoints = !Options::showResolutionPoints;
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void L(bool down) {
	if (down) {
		cout << "Toggling lines" << endl;
		Scene::toggle("polylines");
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
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void K(bool down) {
	if (down) {
		cout << "Toggling Karras-only octree" << endl;
		Options::resolveAmbiguousCells = !Options::resolveAmbiguousCells;
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void O(bool down) {
	if (down) {
		cout << "Toggling Octree!" << endl;
		Scene::toggle("quadtree");
		Options::showOctree = !Options::showOctree;
		RedrawScene();
		Refresh();
	}
}

static void Z(bool down) {
}

static void R(bool down) {
	if (down) {
		//OrthoCamera::reset();
		Options::resolveAmbiguousCells = !Options::resolveAmbiguousCells;
		Data::quadtree->build(Data::lines.get());
		//RedrawScene();
		//Refresh();
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
		Scene::toggle("instructions");
	}
}

static void U(bool down) {
	if (down) {
		cout << "Undoing line" << endl;
		Data::lines->undoLine();
		Data::quadtree->build(Data::lines.get());
	}
	RedrawScene();
	Refresh();
}

static void D(bool down) {
	OrthoCamera::zoom(glm::vec2(0.0687363, 0.0242909), 1.02);
}

static void S(bool down) {
	if (down) {
		cout << "Saving lines to folder" << endl;
		Data::lines->write2DToFile("./TestData/TestData/temp");
	}
}

static void Up(bool down) {
	if (down) {
		Options::maxConflictIterations++;
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

static void Down(bool down) {
	if (down) {
		if (Options::maxConflictIterations > 0)
			Options::maxConflictIterations--;
		Data::quadtree->build(Data::lines.get());
		RedrawScene();
		Refresh();
	}
}

void LeftMouse(bool down, int mods) {
	md.leftDown = down;
	if (md.leftDown) {
		glm::vec4 temp = glm::vec4(md.x, -md.y, 0.0, 1.0);
		temp = OrthoCamera::IV * temp;

		/* If we're holding down shift but not control */
		if ((mods & GLFW_MOD_SHIFT) != 0 && (mods & GLFW_MOD_CONTROL) == 0) {
			Data::lines->addPoint(make_float2( temp.x, temp.y ));
			Data::quadtree->build(Data::lines.get());
		} /* Else if we're pressing shift but not control */
		else if ((mods & GLFW_MOD_CONTROL) != 0 ) {
			Data::lines->replaceLast(make_float2( temp.x, temp.y ));
			Data::quadtree->build(Data::lines.get());
		}
		else {
			Data::lines->newLine(make_float2( temp.x, temp.y ));
		}

		RedrawScene();
		Refresh();
	}
	md.mods = mods;
}

void RightMouse(bool down, int mods) {
	//OrthoCamera::pan({ md.x, md.y });
		//lines->setPoint(curMouse, true);
		//octree->build(*lines);
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
	case GLFW_KEY_D: D(down);
		break;
	}
}

void Events::mouse_cb(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		button == GLFW_MOUSE_BUTTON_LEFT ? LeftMouse(DOWN, mods) : RightMouse(DOWN, mods);
		button == GLFW_MOUSE_BUTTON_LEFT ? md.leftDown = DOWN : md.rightDown = DOWN;
	}
	else if (action == GLFW_RELEASE) {
		button == GLFW_MOUSE_BUTTON_LEFT ? LeftMouse(UP, mods) : RightMouse(UP, mods);
		button == GLFW_MOUSE_BUTTON_LEFT ? md.leftDown = UP : md.rightDown = UP;
	}
}

void Events::mouse_move_cb(GLFWwindow* window, double xpos, double ypos) {
	float oldx = md.x; float oldy = md.y;
	md.x = (xpos / window_width) * 2 - 1;
	md.y = (ypos / window_height) * 2 - 1;

	if (md.leftDown) {
		Sketcher::instance()->clear();
		glm::vec4 temp = glm::vec4(md.x, -md.y, 0.0, 1.0);
		temp = OrthoCamera::IV * temp;

		if ((md.mods & GLFW_MOD_CONTROL) != 0)
			Data::lines->replaceLast(make_float2( temp.x, temp.y ));
		else 
			Data::lines->addPoint(make_float2( temp.x, temp.y ));

		Data::quadtree->build(Data::lines.get());

		RedrawScene();
		Refresh();
	}

	if (md.rightDown) {
		glm::vec4 newVec = { md.x, md.y, 0.0, 1.0 };
		glm::vec4 oldVec = { oldx, oldy, 0.0, 1.0 };
		newVec = OrthoCamera::IV  * newVec;
		oldVec = OrthoCamera::IV  * oldVec;
		OrthoCamera::pan({ newVec.x - oldVec.x, -(newVec.y - oldVec.y) });
	}
}

void Events::scroll_cb(GLFWwindow* window, double xoffset, double yoffset) {
	int sign = signbit(yoffset);
	glm::vec4 temp = glm::vec4(md.x, -md.y, 0.0, 1.0);
	temp = OrthoCamera::IV * temp;
 	if (sign == 1)
		OrthoCamera::zoom(glm::vec2(temp.x, temp.y), .90);
	else
		OrthoCamera::zoom(glm::vec2(temp.x, temp.y), 1.0 / .90);
}

void Events::resize_cb(GLFWwindow* window, int width, int height) {
	GLUtilities::window_width = width;
	GLUtilities::window_height = height;
	glViewport(0, 0, width, height);
}

void Events::focus_cb(GLFWwindow* window, int focused) {
}

//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
void Events::Initialize() {
	//using namespace GLUtilities;
	//string file = ".\\Sources\\titleslide.png";
	//Sketcher::instance()->uploadImage(file, "instructions");
	//Plane p = {};
	//p.width = .5;
	//p.height = .5;
	//p.offset = glm::vec3(0.0);
	//p.texName = "test";
	//Sketcher::instance()->add(p);

	//using namespace cv;
	//using namespace std;

	//Mat src, src_gray, canny;
	//vector<Vec4i> hierarchy;
	//int thresh = 100;
	///* Load the source image */
	//src = imread(file, 1);
	///* convert it to gray scale and blur. */
	//cvtColor(src, src_gray, CV_BGR2GRAY);
	//blur(src_gray, src_gray, Size(3, 3));
	///* Detect edges using canny */
	//Canny(src_gray, canny, thresh, thresh * 2, 3);
	///* Find contours */
	//vector<vector<cv::Point> > contours;
	//findContours(canny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS, cv::Point(3, 3));
	///* Add the contours to polylines */
	////for (int i = 0; i < contours.size(); ++i) {
	////	for (int j = 0; j < contours[i].size(); ++j) {
	////		floatn p = { (float)(contours[i][j].x-1) / src.rows, (float)(contours[i][j].y-1) / src.cols };
	////		p *= 2.0;
	////		p = p - 1.005;
	////		if (j == 0)
	////			Data::lines->newLine(p);
	////		else
	////			Data::lines->addPoint(p);
	////	}
	////}
	Data::quadtree->build(Data::lines.get());
	RedrawScene();
}

void Events::RedrawScene() {
	using namespace Data;
}

void Events::Refresh() {
	using namespace GLUtilities;

	/* Clear stuff */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Draw objects */
	Scene::render(OrthoCamera::V);
	
	//if (Options::showInstructions)
		//Sketcher::instance()->drawPlane("instructions", 0, glm::mat4(1.0));
	glfwSwapBuffers(GLUtilities::window);
}

