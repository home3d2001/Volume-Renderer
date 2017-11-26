#include "OrbitCamera.h"

namespace GLUtilities {
	OrbitCamera::OrbitCamera() : Entity() {
		forward = glm::vec3(0, 1, 0);
		up = glm::vec3(0, 0, 1);
		left = glm::vec3(1, 0, 0);
		
		// View
		position = glm::vec3(0,0,0);
		//at = glm::vec3(0,0,0);
		//up = glm::vec3(0,1,0);
		//left = glm::vec3();

		rotation= glm::quat();

		// Projection
		fov = 45.f;
		aspectRatio = 1.0f;
		nearClippingPlane = .1f;
		farClippingPlane = 100.f;

		angle = glm::vec3(0.0);
		translation = glm::vec3(0.0);
		recalculate();
		reset();
	}

	void OrbitCamera::recalculate() {
		glm::mat4 RotationMatrix = glm::toMat4(rotation);

		V = glm::lookAt(position, forward, up);
		IV = glm::inverse(V);
		matrix = glm::mat4(1.0);
		matrix = glm::translate(matrix, translation);
		matrix = matrix * RotationMatrix;
		IVM = glm::inverse(matrix);
		P = glm::perspective(fov, aspectRatio, nearClippingPlane, farClippingPlane);
		IP = glm::inverse(P);
		VP = P * V * matrix;
	}

	void OrbitCamera::update() {
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_UP)) {
			//position += glm::vec3(.1, 0.0, 0.0);
			zoomAmount += zoomSpeed;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_DOWN)) {
			//position += glm::vec3(-.1, 0.0, 0.0);
			zoomAmount -= zoomSpeed;
		}
		
		if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (!mousePrevPressed) {
				glfwGetCursorPos(GLUtilities::window, &oldXPos, &oldYPos);
				mousePrevPressed = true;
			}
			else {
				glfwGetCursorPos(GLUtilities::window, &newXPos, &newYPos);
				yaw += (newXPos - oldXPos) * rotateSpeed;
				pitch += -(newYPos - oldYPos) * rotateSpeed;
				oldXPos = newXPos;
				oldYPos = newYPos;
			}
		}
		if (glfwGetMouseButton(GLUtilities::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			mousePrevPressed = false;
		}



	//	transform.RotateAround(target.transform.position, transform.right, pitchAmount);
	//	transform.RotateAround(target.transform.position, transform.up, yawAmount);



		zoomAmount -= zoomAmount* zoomResistance;
		yaw -= yaw * rotateResistance;
		pitch -= pitch * rotateResistance;
		translation += forward * zoomAmount;

		glm::vec3 currentLeft = left * rotation;
		glm::vec3 currentUp = up * rotation;

		rotation *= glm::angleAxis(glm::radians(-pitch), currentLeft);
		rotation *= glm::angleAxis(glm::radians(yaw), currentUp);

		
		recalculate();

	}



	void OrbitCamera::reset() {
/*		position = glm::vec3(0, 0, 2);
		at = glm::vec3(0, 0, 0);
		up = glm::vec3(0, 1, 0);
		angle = glm::vec3(0.0);
		translation = glm::vec3(0.0)*/;
	}

	void OrbitCamera::zoom(float amount) {
		//glm::vec3 direction = glm::normalize(position - at);
		//float magnitude = glm::distance(position, at);
		//position += (amount * magnitude) * direction;
	}
}