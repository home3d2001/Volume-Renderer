#pragma once

#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "Vector/vec.h"
using namespace GLUtilities;
using namespace std;

namespace Entities {
	class Box : public Entity {
	public:
		void render(glm::mat4, glm::mat4);
		void update();
		Box();
		glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);

	protected:
		static int count;
		void updateVBO();
		void updateVAO();

	private:
		GLuint linesVAO, VAO;
		cl_GLuint pointsVBO;
		cl_GLuint colorsVBO;
		size_t pointsVBOSize = 0;
		size_t colorsVBOSize = 0;
	};
}