#include "RaycastVolume.hpp"
#include "Vector/vec.h"
#include "Entities/Cameras/OrbitCamera.h"

namespace Entities {

	int RaycastVolume::count = 0;


	RaycastVolume::RaycastVolume(
		string filename,
		int3 rawDimensions,
		int bytesPerPixel,
		shared_ptr<OrbitCamera> camera,
		int samples)
	{
		this->camera = camera;
		this->filename = filename;
		count++;
		this->rawDimensions = rawDimensions;
		this->samples = samples;
		this->bytesPerPixel = bytesPerPixel;

		updateVBO();
		updateVAO();
		updateImage(filename, rawDimensions, bytesPerPixel);
		computeHistogram();
	}

	void RaycastVolume::setTransferFunction(std::shared_ptr<Texture> transferFunction) {
		this->transferFunction = transferFunction;
	}

	std::shared_ptr<Texture> RaycastVolume::getHistogramTexture() {
		return histogramTexture;
	}

	void RaycastVolume::updateVBO() {
		using namespace glm;
		using namespace std;

		/* Create cube to render volume in */

		// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
		const GLfloat cubePoints[] = {
			-1.0f,-1.0f,-1.0f,1.0f,  -1.0f,-1.0f, 1.0f,1.0f,  -1.0f, 1.0f, 1.0f,1.0f, //t1
			1.0f, 1.0f,-1.0f,1.0f,  -1.0f,-1.0f,-1.0f,1.0f,  -1.0f, 1.0f,-1.0f,1.0f, //t2
			1.0f,-1.0f, 1.0f,1.0f,  -1.0f,-1.0f,-1.0f,1.0f,  1.0f,-1.0f,-1.0f,1.0f, //t3
			1.0f, 1.0f,-1.0f,1.0f,  1.0f,-1.0f,-1.0f,1.0f,  -1.0f,-1.0f,-1.0f,1.0f, //t4
			-1.0f,-1.0f,-1.0f,1.0f,  -1.0f, 1.0f, 1.0f,1.0f,  -1.0f, 1.0f,-1.0f,1.0f, //t5
			1.0f,-1.0f, 1.0f,1.0f,  -1.0f,-1.0f, 1.0f,1.0f,  -1.0f,-1.0f,-1.0f,1.0f, //t6
			-1.0f, 1.0f, 1.0f,1.0f,  -1.0f,-1.0f, 1.0f,1.0f,  1.0f,-1.0f, 1.0f,1.0f, //t7
			1.0f, 1.0f, 1.0f,1.0f,  1.0f,-1.0f,-1.0f,1.0f,  1.0f, 1.0f,-1.0f,1.0f, //t8
			1.0f,-1.0f,-1.0f,1.0f,  1.0f, 1.0f, 1.0f,1.0f,  1.0f,-1.0f, 1.0f,1.0f, //t9
			1.0f, 1.0f, 1.0f,1.0f,  1.0f, 1.0f,-1.0f,1.0f,  -1.0f, 1.0f,-1.0f,1.0f, //t10
			1.0f, 1.0f, 1.0f,1.0f,  -1.0f, 1.0f,-1.0f,1.0f,  -1.0f, 1.0f, 1.0f,1.0f, //t11
			1.0f, 1.0f, 1.0f,1.0f,  -1.0f, 1.0f, 1.0f,1.0f,  1.0f,-1.0f, 1.0f,1.0f //t12
		};
		pointsVBOSize = sizeof(cubePoints);

		// One color for each vertex. They were generated randomly.
		static const GLfloat cubeColors[] = {
			1.083f,  0.771f,  0.014f,  1.0f,  0.609f,  0.115f,  0.436f,  1.0f,  0.327f,  0.483f,  0.844f,  1.0f,
			0.822f,  1.069f,  0.201f,  1.0f,  0.435f,  0.602f,  0.223f,  1.0f,  0.310f,  0.747f,  0.185f,  1.0f,
			1.097f,  0.770f,  0.761f,  1.0f,  1.059f,  0.436f,  0.730f,  1.0f,  0.359f,  1.083f,  0.152f,  1.0f,
			0.483f,  1.096f,  0.789f,  1.0f,  1.059f,  0.861f,  0.639f,  1.0f,  0.195f,  1.048f,  0.859f,  1.0f,
			0.014f,  0.184f,  1.076f,  1.0f,  0.771f,  0.328f,  0.970f,  1.0f,  0.406f,  0.615f,  0.116f,  1.0f,
			0.676f,  0.977f,  0.133f,  1.0f,  0.971f,  1.072f,  0.833f,  1.0f,  0.140f,  0.616f,  0.489f,  1.0f,
			0.997f,  1.013f,  0.064f,  1.0f,  0.945f,  0.719f,  1.092f,  1.0f,  1.043f,  0.021f,  0.978f,  1.0f,
			0.279f,  0.317f,  1.005f,  1.0f,  0.167f,  0.620f,  0.077f,  1.0f,  0.347f,  0.857f,  0.137f,  1.0f,
			0.055f,  0.953f,  0.042f,  1.0f,  0.714f,  1.005f,  0.345f,  1.0f,  0.783f,  0.290f,  0.734f,  1.0f,
			0.722f,  0.645f,  0.174f,  1.0f,  0.302f,  0.455f,  0.848f,  1.0f,  0.225f,  1.087f,  0.040f,  1.0f,
			1.017f,  0.713f,  0.338f,  1.0f,  0.053f,  0.959f,  0.120f,  1.0f,  0.393f,  0.621f,  0.362f,  1.0f,
			0.673f,  0.211f,  0.457f,  1.0f,  0.820f,  0.883f,  0.371f,  1.0f,  0.982f,  0.099f,  0.879f,  1.0f
		};
		colorsVBOSize = sizeof(cubeColors);

		/* Upload the points */
		glGenBuffers(1, &pointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glBufferData(GL_ARRAY_BUFFER, pointsVBOSize, cubePoints, GL_STATIC_DRAW);

		/* Upload the colors */
		glGenBuffers(1, &colorsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		glBufferData(GL_ARRAY_BUFFER, colorsVBOSize, cubeColors, GL_STATIC_DRAW);
	}

	void RaycastVolume::updateVAO() {

		glGenVertexArrays(1, &linesVAO);
		glBindVertexArray(linesVAO);

		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);

		glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glEnableVertexAttribArray(Shaders::raycastVolProgram->position_id);
		glVertexAttribPointer(Shaders::raycastVolProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		print_gl_error();

		/*glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		print_gl_error();
		glEnableVertexAttribArray(Shaders::raycastVolProgram->color_id);
		print_gl_error();
		glVertexAttribPointer(Shaders::raycastVolProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);*/
		print_gl_error();


	}

	void RaycastVolume::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		if (hide == true) return;
		
		glm::mat4 finalMatrix = parent_matrix * transform.LocalToParentMatrix();
		glm::mat4 inverseMatrix = transform.ParentToLocalMatrix();
		glm::vec4 position = glm::vec4(camera->getPosition(), 1.0);
		glm::vec4 rayOrigin = inverseMatrix * position;

		/* 1. Use program */
		Shaders::raycastVolProgram->use();

		/* 2. Use VAO */
		glBindVertexArray(VAO);

		/* 3. Update uniforms */
		glUniformMatrix4fv(Shaders::raycastVolProgram->model_view_id, 1, 0, &(finalMatrix[0].x));
		glUniformMatrix4fv(Shaders::raycastVolProgram->projection_id, 1, 0, &(projection[0].x));
		glUniform1fv(Shaders::raycastVolProgram->focal_length_id, 1, &camera->focalLength);
		glUniform2fv(Shaders::raycastVolProgram->window_size_id, 1, &camera->windowSize.x);
		glUniform3fv(Shaders::raycastVolProgram->ray_origin_id, 1, &rayOrigin.x);
		glUniform1i(Shaders::raycastVolProgram->samples_id, samples);
		glUniform1i(Shaders::raycastVolProgram->perturbation_id, perturbation);
		print_gl_error();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);

		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, (interpolate) ? GL_LINEAR : GL_NEAREST);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, (interpolate) ? GL_LINEAR : GL_NEAREST);

		glUniform1i(Shaders::raycastVolProgram->texture0_id, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, transferFunction->textureID);
		glUniform1i(Shaders::raycastVolProgram->texture1_id, 1);
		print_gl_error();

		/* 4. Draw */
		glDrawArrays(GL_TRIANGLES, 0, pointsVBOSize / sizeof(float4));
		print_gl_error();
	}

	void RaycastVolume::handleKeys()
	{
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_N)) {
			interpolate = false;
		}
		else if (glfwGetKey(GLUtilities::window, GLFW_KEY_L)) {
			interpolate = true;
		}
		
		if (glfwGetKey(GLUtilities::window, GLFW_KEY_Y)) {
			samples = 32;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_U)) {
			samples = 64;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_I)) {
			samples = 128;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_O)) {
			samples = 256;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_P)) {
			samples = 1024;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_COMMA)) {
			perturbation = true;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_PERIOD)) {
			perturbation = false;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_W)) {
			hide = true;
		}

		if (glfwGetKey(GLUtilities::window, GLFW_KEY_S)) {
			hide = false;
		}
	}

	void RaycastVolume::update() {
		//this->transform.AddRotation(glm::angleAxis(0.01f, glm::vec3(0.0, .0, 1.0)));

		handleKeys();
	}

	void RaycastVolume::updateImage(string filename, int3 rawDimensions, int bytesPerPixel) {
		/* Loads and uploads a 3D texture to the GPU */
		const int sizeInBytes = rawDimensions.x * rawDimensions.y * rawDimensions.z * bytesPerPixel;
		FILE *pFile = fopen(filename.c_str(), "rb");
		if (NULL == pFile) {
			/*return false;*/
			cout << "ERROR LOADING VOLUME " << filename << endl;
			return;
		}

		pVolume.resize(rawDimensions.x * rawDimensions.y * rawDimensions.z * bytesPerPixel);
		fread(pVolume.data(), sizeof(GLubyte), sizeInBytes, pFile);
		fclose(pFile);

		glGenTextures(1, &textureID);
		print_gl_error();

		glBindTexture(GL_TEXTURE_3D, textureID);
		print_gl_error();

		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, rawDimensions.x, rawDimensions.y, rawDimensions.z, 0, GL_LUMINANCE,
			(bytesPerPixel == 1) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, pVolume.data());
		
		//glGenerateMipmap(GL_TEXTURE_3D);

		print_gl_error();
	}

	void RaycastVolume::computeHistogram() {
		if (bytesPerPixel == 1)
			computeHistogram8();
		else
			computeHistogram16();
	}

	void RaycastVolume::computeHistogram8() {
		/* ONLY WORKS FOR 1 BYTE ATM...*/
		int width = 256;
		vector<int> histogram(width);

		int height = 0;
		for (int i = 0; i < pVolume.size(); ++i) {
			int address = pVolume.at(i);
			histogram.at(address) += 1;
			height = std::max(histogram.at(pVolume.at(i)), height);
		}

		vector<GLubyte> image(256 * width);
		for (int y = 0; y < 256; ++y) {
			for (int x = 0; x < width; ++x) {
				float normalized = histogram.at(pVolume.at(x)) / (float)height; // potential bug here
				image.at(x + (255 - y) * width) = ((255 * normalized) < y) ? 255 : 0;
			}
		}
		histogramTexture = make_shared<Texture>(width, 256, image, true);
	}

	void RaycastVolume::computeHistogram16() {
		/* ONLY WORKS FOR 2 BYTE ATM...*/
		int width = 65536;
		vector<int> histogram(256); // 16 bit lut is too big. use 8 bits for now.

		int height = 0;
		for (int i = 0; i < pVolume.size(); i+=2) {
			int address = pVolume.at(i);
			address |= pVolume.at(i + 1) << 8;

			float normalized = address / 65536.0;
			histogram.at(normalized * 255) += 1;
			height = std::max(histogram.at(normalized * 255), height);
		}

		vector<GLubyte> image(256 * 256);
		for (int y = 0; y < 256; ++y) {
			for (int x = 0; x < 256; ++x) {
				float normalized = histogram.at(x) / (float)height; // potential bug here
				image.at(x + (255 - y) * 256) = ((255 * normalized) < y) ? 255 : 0;
			}
		}
		histogramTexture = make_shared<Texture>(256, 256, image, true);
	}
}