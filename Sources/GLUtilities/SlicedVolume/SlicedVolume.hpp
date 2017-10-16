#pragma once
//#define STB_IMAGE_IMPLEMENTATION
#include "Shaders/Shaders.hpp"
#include "GLUtilities/Entity.h"
#include "GLUtilities/OrbitCamera.h"
#include "GLUtilities/ImagePlane/ImagePlane.hpp"
#include "SlicedVolume.h"
//#include "GLUtilities/stb_image.h"
#include "Vector/vec.h"
using namespace GLUtilities;
using namespace std;

class SlicedVolume : public Entity {
public:
	void render(glm::mat4);
	void handleKeys();
	void update();
	SlicedVolume(string filename, int3 size, int bytesPerPixel, shared_ptr<OrbitCamera> camera, shared_ptr<ImagePlane> transferFunction, int samples);
	std::string filename;

protected:
	static int count;
	void updateVBO();
	void updateVAO();

private:
	/* Sample points*/
	bool renderSamplePoints = false;
	GLuint samplePointsVAO;
	cl_GLuint samplePointsVBO;
	cl::BufferGL samplePointsVBO_cl;
	size_t samplePointsVBOSize = 0;

	/* Edge points*/
	bool renderEdgePoints = false;
	bool renderFaces = false;
	bool renderSlices = true;
	GLuint edgePointsVAO;
	cl_GLuint edgePointsVBO;
	cl::BufferGL edgePointsVBO_cl;
	size_t edgePointsVBOSize = 0;

	int samplesToTake;
	int3 rawDimensions;
	shared_ptr<OrbitCamera> camera;
	shared_ptr<ImagePlane> transferFunction;

	GLuint textureID;
	float4 offset;

	void updateImage(string filename, int3 rawDimensions, int bytesPerPixel);
	
};