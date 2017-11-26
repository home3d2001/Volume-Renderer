#pragma once
//#define STB_IMAGE_IMPLEMENTATION
#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "Entities/Cameras/OrbitCamera.h"
#include "Entities/ImagePlane/ImagePlane.hpp"
#include "SlicedVolume.h"
#include "Vector/vec.h"
using namespace GLUtilities;
using namespace std;

namespace Entities {


	class SlicedVolume : public Entity {
	public:
		void render(glm::mat4, glm::mat4);
		void handleKeys();
		void update();
		SlicedVolume(string filename, int3 rawDimesions, int bytesPerPixel, shared_ptr<OrbitCamera> camera, int samples);
		void setTransferFunction(std::shared_ptr<Texture> transferFunction);
		shared_ptr<Texture> getHistogramTexture();

		void computeHistogram();
		std::string filename;

	protected:
		static int count;
		void updateVBO();
		void updateVAO();

	private:
		GLuint VAO;
		cl_GLuint VBO;
		cl::BufferGL VBO_cl;
		size_t VBOSize = 0;

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
		bool interpolate = true;
		GLuint edgePointsVAO;
		cl_GLuint edgePointsVBO;
		cl::BufferGL edgePointsVBO_cl;
		size_t edgePointsVBOSize = 0;

		int samplesToTake;
		int3 rawDimensions;
		shared_ptr<OrbitCamera> camera;

		shared_ptr<Texture> transferFunction;
		shared_ptr<Texture> histogramTexture;

		GLuint textureID;
		float4 offset;
		vector<GLubyte> pVolume;

		void updateImage(string filename, int3 rawDimensions, int bytesPerPixel);

	};
}