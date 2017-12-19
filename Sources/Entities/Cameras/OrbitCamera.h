#pragma once

#include "Entities/Entity.h"
#include <atomic>

namespace Entities {
	class OrbitCamera : public Entity {
		private:
			GLuint VAO;
			cl_GLuint VBO;
			cl::BufferGL VBO_cl;
			size_t VBOSize = 0;

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
			void handleZoom();
			void handleReset();
			/*void handleMouse();
			void handleCameraSwitch();*/
			
		public: 
			float fov;
			float focalLength;
			float aspectRatio;
			float nearClippingPlane;
			float farClippingPlane;
			glm::vec2 windowSize;

			static int count;
			static int selected;
			int id;
			bool visible = true;
			OrbitCamera(glm::vec3 initialPos = glm::vec3(), glm::vec3 rotatePoint = glm::vec3(), glm::quat initialRot = glm::quat());
			void setWindowSize(int width, int height);
			// View
			void render(glm::mat4 parent_matrix, glm::mat4 projection);
			void update();
			void raycast(glm::vec4 point, glm::vec4 direction);
			void recalculate();
			glm::vec3 initialPos;
			glm::vec3 position;
			glm::vec3 rotatePoint;
			glm::quat initialRot;
			//glm::quat rotation;
			//glm::mat4 VM, IVM;
			//glm::mat4 IM;
			atomic<glm::mat4> atomIM = glm::mat4(), atomIV = glm::mat4();
			glm::mat4 V, IV;
			glm::mat4 P, IP;
			//glm::mat4 PVM;
			glm::vec3 angle;
			//glm::vec3 translation;
			
			void reset();
			void zoom(float amount);
			void updateVBO();
			void updateVAO();
	};
}