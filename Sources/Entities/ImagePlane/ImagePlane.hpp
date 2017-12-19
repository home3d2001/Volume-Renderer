#pragma once

#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "ImagePlane.h"
#include "Vector/vec.h"
#include "GLUtilities/texture.hpp"

namespace Entities {
	class ImagePlane : public Entity {
	public:
		float2 minPos = make_float2(0.0, 0.0);
		float2 maxPos = make_float2(1.0, 1.0);

		void render(glm::mat4, glm::mat4);
		ImagePlane(std::shared_ptr<Texture> texture);
		std::string filename;
		std::shared_ptr<Texture> texture;
		void setInterpolation(bool useInterpolation);

	protected:
		static int count;
		void updateVBO();
		void updateVAO();

	private:
		GLuint VAO;
		cl_GLuint VBO;
		cl::BufferGL VBO_cl;
		size_t VBOSize = 0;

		bool useInterpolation = true;
		int width; 
		int height;
		float4 offset;

		void updateImage(string imagePath);
	
	};
}
