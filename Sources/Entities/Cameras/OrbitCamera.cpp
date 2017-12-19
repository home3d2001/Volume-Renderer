#include "OrbitCamera.h"
#include "ParallelAlgorithms/defs.h"
#include "Vector/vec.h"


namespace Entities {
	int OrbitCamera::count = 0;
	int OrbitCamera::selected = 0;

	OrbitCamera::OrbitCamera(glm::vec3 initialPos, glm::vec3 rotatePoint, glm::quat initialRot) : Entity() {
		id = count;
		count++;

		transform.SetPosition(initialPos);
		transform.SetRotation(initialRot);

		this->initialPos = initialPos;
		this->initialRot = initialRot;
		this->rotatePoint = rotatePoint;

		fov = 45.f;
		aspectRatio = 1.0f;
		nearClippingPlane = .1f;
		farClippingPlane = 100.f;
		focalLength = 1.0 / tan(fov / 2.0);

		angle = glm::vec3(0.0);
		recalculate();
		
		updateVBO();
		updateVAO();
	}

	void OrbitCamera::setWindowSize(int width, int height) {
		float aspectRatio = width / (float)height;

		assert(aspectRatio > 0);
		if (aspectRatio > 0) {
			this->windowSize = glm::vec2(width, height);
			this->aspectRatio = aspectRatio;
			recalculate();
		}
	}
	
	void OrbitCamera::recalculate() {
		/* Construct object space lookat */
		V = glm::lookAt(transform.position, transform.position + transform.forward, transform.up);
		
		/* Create perspective transformation */
		P = glm::perspective(fov, aspectRatio, nearClippingPlane, farClippingPlane);
	}

	void OrbitCamera::handleArrowKeys() {
		float arrowSpeed = 10;
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_UP)) {
			pitchVelocity += arrowSpeed * rotationAcceleration;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_DOWN)) {
			pitchVelocity -= arrowSpeed * rotationAcceleration;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_LEFT)) {
			yawVelocity += arrowSpeed * rotationAcceleration;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_RIGHT)) {
			yawVelocity -= arrowSpeed * rotationAcceleration;
		}
	}
	//void OrbitCamera::handleMouse() {
	//	/* GLFW doesn't give hold info, so we have to handle it ourselves here. */
	//	/* GLFW also doesn't supply delta cursor position, so we compute it. */

	//	if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
	//		if (!mousePrevPressed) {
	//			glfwGetCursorPos(GLUtilities::window, &oldXPos, &oldYPos);
	//			mousePrevPressed = true;
	//		}
	//		else {
	//			glfwGetCursorPos(GLUtilities::window, &newXPos, &newYPos);
	//			yawVelocity += (oldXPos - newXPos) * rotationAcceleration;
	//			pitchVelocity += (oldYPos - newYPos) * rotationAcceleration;
	//			oldXPos = newXPos;
	//			oldYPos = newYPos;
	//		}
	//	}
	//	if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
	//		mousePrevPressed = false;
	//	}
	//}
	void OrbitCamera::handleZoom() {
		float arrowSpeed = 1;
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_MINUS)) {
			zoomVelocity -= arrowSpeed * zoomAcceleration;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_EQUAL)) {
			zoomVelocity += arrowSpeed * zoomAcceleration;
		}
	}
	void OrbitCamera::handleReset() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_R)) {
			reset();
		}
	}
	/*void OrbitCamera::handleCameraSwitch() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_0)) {
			selected = 0;
		} else if (glfwGetKey(GLUtilities::window, GLFW_KEY_1)) {
			selected = 1;
		}
	}*/
	void OrbitCamera::raycast(glm::vec4 point, glm::vec4 direction) {
		///* Don't update unselected cameras */
		//if (id != selected) return;
		//
		//handleMouse();
	}
	void OrbitCamera::update() {
		/* Don't update unselected cameras */
		if (id != selected) return;


	//	handleCameraSwitch();
		handleArrowKeys();
	////	handleMouse();
		handleZoom();
		handleReset();




		zoomVelocity -= zoomVelocity* zoomResistance;
		yawVelocity -= yawVelocity * rotateResistance;
		pitchVelocity -= pitchVelocity * rotateResistance;
		transform.AddPosition(glm::normalize(rotatePoint - transform.position) * zoomVelocity);

		glm::vec3 currentRight = transform.right;
		glm::vec3 currentUp = transform.up;
		
		transform.RotateAround(rotatePoint, currentRight, pitchVelocity);
		transform.RotateAround(rotatePoint, -currentUp, yawVelocity);
		
		recalculate();
	}
	void OrbitCamera::reset() {
		transform.SetPosition(initialPos);
		transform.SetRotation(initialRot);
		pitchVelocity = yawVelocity = 0;
		zoomVelocity = 0;
	}

	void OrbitCamera::zoom(float amount) {
		//glm::vec3 direction = glm::normalize(position - at);
		//float magnitude = glm::distance(position, at);
		//position += (amount * magnitude) * direction;
	}
	void OrbitCamera::updateVBO() {
		/*float data[8] = {
			0.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f
		};

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STATIC_DRAW);
		print_gl_error();*/
	}
	void OrbitCamera::updateVAO() {
		/*glDeleteVertexArrays(1, &VAO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(Shaders::pointProgram->position_id);
		glEnableVertexAttribArray(Shaders::pointProgram->color_id);
		glVertexAttribPointer(Shaders::pointProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		glVertexAttribPointer(Shaders::pointProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), (void*)(sizeof(float4)));
		print_gl_error();*/

	}
	void OrbitCamera::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		//if (id != selected && visible) {
		//	glm::mat4 finalMatrix = projection * parent_matrix * IM * IV;

		//	glEnable(GL_LINE_SMOOTH);
		//	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		//	//print_gl_error();

		//	Shaders::pointProgram->use();
		//	glBindVertexArray(VAO);
		//	//print_gl_error();

		//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//	//print_gl_error();

		//	glUniformMatrix4fv(
		//		Shaders::pointProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		//	//print_gl_error();

		//	glUniform1f(Shaders::pointProgram->pointSize_id, 10.0);
		//	//print_gl_error();
		//	glLineWidth(4.);
		//	glDrawArrays(GL_POINTS, 0, 1);
		//	//print_gl_error();
		//	glBindVertexArray(0);
		//}
	}
}