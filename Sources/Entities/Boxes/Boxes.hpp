#pragma once

#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "Boxes.h"

namespace Entities {
	class Boxes3D : public Entity {
	public:
		void render(glm::mat4, glm::mat4);
		void add(float4 center, float4 scale, float4 color);
		void clear();
		Boxes3D();
	protected:
		GLuint VAO;
		cl_GLuint VBO;
		cl::BufferGL VBO_cl;
		size_t VBOSize = 0;

		static int count;
		int totalBoxes;
		std::vector<Box> boxes;
		cl::Buffer boxesBuffer;

		int vboSize = 0;
		void updateVBO();
		void updateVAO();
	};
}
