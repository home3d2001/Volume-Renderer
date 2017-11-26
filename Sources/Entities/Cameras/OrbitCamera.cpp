#include "OrbitCamera.h"
#include "ParallelAlgorithms/defs.h"
#include "Vector/vec.h"


namespace Entities {
	int OrbitCamera::count = 0;
	int OrbitCamera::selected = 0;

	OrbitCamera::OrbitCamera(glm::vec3 initialPos, glm::quat initialRot) : Entity() {
		id = count;
		count++;

		forward = glm::vec3(0, 1, 0);
		up = glm::vec3(0, 0, 1);
		left = glm::vec3(1, 0, 0);
		
		this->initialPos = initialPos;
		this->initialRot = initialRot;

		translation = initialPos;
		rotation = initialRot;

		fov = 45.f;
		aspectRatio = 1.0f;
		nearClippingPlane = .1f;
		farClippingPlane = 100.f;
		focalLength = 1.0 / tan(fov / 2.0);


		angle = glm::vec3(0.0);
		recalculate();
		reset();

		updateVBO();
		updateVAO();
	}
	void OrbitCamera::setWindowSize(int width, int height) {
		float aspectRatio = width / (float)height;

		assert(aspectRatio > 0);
		if (aspectRatio > 0) {
			this->windowSize = glm::vec2(width, height);
			this->aspectRatio = aspectRatio;
		}
	}
	glm::vec3 OrbitCamera::getForward() {
		glm::mat4 IM = atomIM;
		return glm::vec3(IM * glm::vec4(forward, 0.0));
	}
	glm::vec3 OrbitCamera::getLeft() {
		glm::mat4 IM = atomIM;
		return glm::vec3(IM * glm::vec4(left, 0.0));
	}
	glm::vec3 OrbitCamera::getUp() {
		glm::mat4 IM = atomIM;
		glm::mat4 RotationMatrix = glm::toMat4(rotation);
		return glm::vec3(IM * glm::vec4(up, 0.0));
	}
	glm::vec3 OrbitCamera::getPosition() {
		glm::mat4 IM = atomIM;
		glm::mat4 IV = atomIV;
		return glm::vec3(IM * IV * glm::vec4(0.0, 0.0, 0.0, 1.0));
	}
	void OrbitCamera::recalculate() {
		glm::mat4 RotationMatrix = glm::toMat4(rotation);

		V = glm::lookAt(glm::vec3(0,0,0), forward, up);
		IV = glm::inverse(V);
		
		transform.localToParentMatrix = glm::mat4(1.0);
		transform.localToParentMatrix = glm::translate(transform.LocalToParentMatrix(), translation) * RotationMatrix;
		IM = glm::inverse(transform.LocalToParentMatrix());

		VM = V * transform.LocalToParentMatrix();
		IVM = glm::inverse(VM);

		P = glm::perspective(fov, aspectRatio, nearClippingPlane, farClippingPlane);
		IP = glm::inverse(P);
		
		PVM = P * V * transform.LocalToParentMatrix();

		atomIM = IM;
		atomIV = IV;
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
	void OrbitCamera::handleMouse() {
		/* GLFW doesn't give hold info, so we have to handle it ourselves here. */
		/* GLFW also doesn't supply delta cursor position, so we compute it. */

		if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!mousePrevPressed) {
				glfwGetCursorPos(GLUtilities::window, &oldXPos, &oldYPos);
				mousePrevPressed = true;
			}
			else {
				glfwGetCursorPos(GLUtilities::window, &newXPos, &newYPos);
				yawVelocity += (oldXPos - newXPos) * rotationAcceleration;
				pitchVelocity += (oldYPos - newYPos) * rotationAcceleration;
				oldXPos = newXPos;
				oldYPos = newYPos;
			}
		}
		if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			mousePrevPressed = false;
		}
	}
	void OrbitCamera::handleZoom() {
		float arrowSpeed = 1;
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_MINUS)) {
			zoomVelocity += arrowSpeed * zoomAcceleration;
		}
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_EQUAL)) {
			zoomVelocity -= arrowSpeed * zoomAcceleration;
		}
	}
	void OrbitCamera::handleReset() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_R)) {
			reset();
		}
	}
	void OrbitCamera::handleCameraSwitch() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_0)) {
			selected = 0;
		} else if (glfwGetKey(GLUtilities::window, GLFW_KEY_1)) {
			selected = 1;
		}
	}
	void OrbitCamera::raycast(glm::vec4 point, glm::vec4 direction) {
		/* Don't update unselected cameras */
		if (id != selected) return;
		
		handleMouse();
	}
	void OrbitCamera::update() {
		/* Don't update unselected cameras */
		if (id != selected) return;

		handleCameraSwitch();
		handleArrowKeys();
	//	handleMouse();
		handleZoom();
		handleReset();



	//	transform.RotateAround(target.transform.position, transform.right, pitchAmount);
	//	transform.RotateAround(target.transform.position, transform.up, yawAmount);



		zoomVelocity -= zoomVelocity* zoomResistance;
		yawVelocity -= yawVelocity * rotateResistance;
		pitchVelocity -= pitchVelocity * rotateResistance;
		translation += forward * zoomVelocity;

		glm::vec3 currentLeft = left * rotation;
		glm::vec3 currentUp = up * rotation;

		rotation *= glm::angleAxis(glm::radians(-pitchVelocity), currentLeft);
		rotation *= glm::angleAxis(glm::radians(-yawVelocity), currentUp);

		
		recalculate();

	}
	void OrbitCamera::reset() {
		translation = initialPos;
		rotation = initialRot;
		pitchVelocity = yawVelocity = 0;
		zoomVelocity = 0;
	}

	void OrbitCamera::zoom(float amount) {
		//glm::vec3 direction = glm::normalize(position - at);
		//float magnitude = glm::distance(position, at);
		//position += (amount * magnitude) * direction;
	}
	void OrbitCamera::updateVBO() {
		float data[8] = {
			0.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f
		};

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STATIC_DRAW);
	}
	void OrbitCamera::updateVAO() {
		glDeleteVertexArrays(1, &VAO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(Shaders::pointProgram->position_id);
		glEnableVertexAttribArray(Shaders::pointProgram->color_id);
		glVertexAttribPointer(Shaders::pointProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		glVertexAttribPointer(Shaders::pointProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), (void*)(sizeof(float4)));
	}
	void OrbitCamera::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (id != selected && visible) {
			glm::mat4 finalMatrix = projection * parent_matrix * IM * IV;

			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			//print_gl_error();

			Shaders::pointProgram->use();
			glBindVertexArray(VAO);
			//print_gl_error();

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//print_gl_error();

			glUniformMatrix4fv(
				Shaders::pointProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
			//print_gl_error();

			glUniform1f(Shaders::pointProgram->pointSize_id, 10.0);
			//print_gl_error();
			glLineWidth(4.);
			glDrawArrays(GL_POINTS, 0, 1);
			//print_gl_error();
			glBindVertexArray(0);
		}
	}
}