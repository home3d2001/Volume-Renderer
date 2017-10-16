#pragma once

#include "Entity.h"

namespace GLUtilities {
	class OrbitCamera : public Entity {
		private:
			float zoomVelocity = 0.0f;
			float zoomAcceleration = .01f;
			float zoomResistance = .1f;

			float yawVelocity = 0.0f;
			float pitchVelocity = 0.0f;
			float rotationAcceleration = 0.03f;
			float rotateResistance = .1f;

			double oldXPos, oldYPos, newXPos, newYPos;
			bool mousePrevPressed;

			void handleArrowKeys();
			void handleMouse();
			void handleZoom();
			void handleReset();
			void handleCameraSwitch();
			
			glm::vec3 up;
			glm::vec3 forward;
			glm::vec3 left;
		public: 
			float fov;
			float aspectRatio;
			float nearClippingPlane;
			float farClippingPlane;

			static int count;
			static int selected;
			int id;
			bool visible = true;
			OrbitCamera(glm::vec3 initialPos = glm::vec3(), glm::quat initialRot = glm::quat());
			void setAspectRatio(float aspectRatio);
			// View
			void render(glm::mat4 parent_matrix);
			void update();
			void recalculate();
			glm::vec3 getForward();
			glm::vec3 getLeft();
			glm::vec3 getUp();
			glm::vec3 getPosition();
			glm::vec3 initialPos;
			glm::vec3 position;
			glm::quat initialRot;
			glm::quat rotation;
			glm::mat4 VM;
			glm::mat4 IM;
			glm::mat4 V;
			glm::mat4 IV;
			glm::mat4 P;
			glm::mat4 IP;
			glm::mat4 PVM;
			glm::vec3 angle;
			glm::vec3 translation;
			
			void reset();
			void zoom(float amount);
			void updateVBO();
			void updateVAO();
	};
}