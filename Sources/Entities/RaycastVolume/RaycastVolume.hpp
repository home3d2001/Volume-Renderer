#pragma once

#include "Shaders/Shaders.hpp"
#include "Entities/Entity.h"
#include "Entities/Cameras/OrbitCamera.h"
#include "Entities/ImagePlane/ImagePlane.hpp"
#include "Entities/Box/Box.hpp"
#include "RaycastVolume.h"
#include "Vector/vec.h"
using namespace GLUtilities;
using namespace std;

namespace Entities {
	class RaycastVolume : public Entity {
	public:
		RaycastVolume(string filename, int3 size, int bytesPerPixel, shared_ptr<OrbitCamera> camera, int samples);
		void update();
		void render(glm::mat4 parent_matrix, glm::mat4 projection);
		void handleKeys();
		std::string filename;
		void computeGradientMagnitudeVolume();
		void computeHistogram();
		void computeHistogram8();
		void computeHistogram16();
		void compute2DHistogram();

		void setTransferFunction(std::shared_ptr<Texture> transferFunction);
		std::shared_ptr<Texture> getHistogramTexture();

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
		int bytesPerPixel;

		bool interpolate = true;
		bool hide = false;

		std::shared_ptr<Box> bb;

		int samples;
		bool perturbation = false;
		int3 rawDimensions;
		shared_ptr<OrbitCamera> camera;
		
		glm::vec2 minTransferFunctionCoord = glm::vec2(0, 0);
		glm::vec2 maxTransferFunctionCoord = glm::vec2(0, 0);
		std::shared_ptr<Texture> transferFunction;
		std::shared_ptr<Texture> histogramTexture;

		vector<GLubyte> volume;
		vector<GLfloat> gradientMagnitudeVolume;

		GLuint volumeTextureID;
		GLuint gradientTextureID;
		void updateImage(string filename, int3 rawDimensions, int bytesPerPixel);
	};
}