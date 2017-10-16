#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace GLUtilities {
	class OrthoCamera {
		private:
			static glm::vec2 position;
			static float left;
			static float right;
			static float bottom;
			static float top;
			//static float near;
			//static float far;
		public: 
			static glm::mat4 V;
			static glm::mat4 IV;
			static glm::mat4 zoom(glm::vec2 origin, float scale);
			static glm::mat4 pan(glm::vec2 displacement);
			static glm::mat4 reset();
	};
}