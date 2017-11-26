#pragma once
#include <string>
#include "GLUtilities/stb_image.h"
#include <assert.h>
#include "GLUtilities/gl_utils.h"

using namespace std;
class Texture {
public: 
	int width;
	int height;
	GLuint textureID;

	Texture(string imagePath) {
		/* Loads and uploads a texture to the GPU */
		int comp;
		int forceChannels = 4;
		stbi_set_flip_vertically_on_load(1);
		unsigned char* image = stbi_load(imagePath.c_str(), &width, &height, &comp, forceChannels);
		assert(image != 0);

		glGenTextures(1, &textureID);
		
		glBindTexture(GL_TEXTURE_2D, textureID);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}

	Texture(int width, int height) {
		this->width = width; 
		this->height = height;
		glGenTextures(1, &textureID);
		print_gl_error();

		glBindTexture(GL_TEXTURE_2D, textureID);
		print_gl_error();


		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		print_gl_error();


		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		print_gl_error();
	}

	Texture(int width, int height, std::vector<GLubyte> image, bool greyScale)
	{
		this->width = width;
		this->height = height;
		glGenTextures(1, &textureID);
		print_gl_error();

		glBindTexture(GL_TEXTURE_2D, textureID);
		print_gl_error();

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		print_gl_error();


		glTexImage2D(GL_TEXTURE_2D, 0, (greyScale) ? GL_LUMINANCE : GL_RGBA, width, height, 0, (greyScale) ? GL_LUMINANCE : GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		print_gl_error();
	}

	Texture(int width, int height, std::vector<GLushort> image, bool greyScale)
	{
		this->width = width;
		this->height = height;
		glGenTextures(1, &textureID);
		print_gl_error();

		glBindTexture(GL_TEXTURE_2D, textureID);
		print_gl_error();


		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		print_gl_error();


		glTexImage2D(GL_TEXTURE_2D, 0, (greyScale) ? GL_LUMINANCE : GL_RGBA, width, height, 0, (greyScale) ? GL_LUMINANCE : GL_RGBA, GL_UNSIGNED_SHORT, image.data());
		print_gl_error();
	}
};