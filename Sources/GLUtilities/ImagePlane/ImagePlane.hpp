#pragma once

#include "Shaders/Shaders.hpp"
#include "GLUtilities/Entity.h"
#include "ImagePlane.h"
#include "GLUtilities/stb_image.h"
#include "Vector/vec.h"

class ImagePlane : public Entity {
public:
	void render(glm::mat4);
	ImagePlane(string filename);
	std::string filename;
	GLuint getTextureID();

protected:
	static int count;
	void updateVBO();
	void updateVAO();

private:
	int width; 
	int height;
	GLuint textureID;
	float4 offset;

	void updateImage(string imagePath);
	
};