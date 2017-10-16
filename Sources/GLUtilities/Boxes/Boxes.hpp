#pragma once

#include "Shaders/Shaders.hpp"
#include "GLUtilities/Entity.h"
#include "Boxes.h"

class Boxes3D : public Entity {
public:
	void render(glm::mat4);
	void add(float4 center, float4 scale, float4 color);
	void clear();
	Boxes3D();
protected:
	static int count;
	int totalBoxes;
	std::vector<Box> boxes;
	cl::Buffer boxesBuffer;

	int vboSize = 0;
	void updateVBO();
	void updateVAO();
};