#include "ImagePlane.hpp"
#include "Vector/vec.h"

int ImagePlane::count = 0;

ImagePlane::ImagePlane(std::string filename) {
	this->filename = filename;
	count++;
	offset = make_float4(0, 0, 0.0, 0.0);
	width = 1.0;
	height = 1.0;

	updateVBO();
	updateVAO();
	updateImage(filename);
}

void ImagePlane::updateVBO() {
	vector<float> planePoints = {
		 -1., -1.,  0.0,  1.0 ,
		  1., -1.,  0.0,  1.0 ,
		 -1.,  1.,  0.0,  1.0 ,
		  1., -1.,  0.0,  1.0 ,
		  1.,  1.,  0.0,  1.0 ,
		 -1.,  1.,  0.0,  1.0 
	};
	VBOSize = planePoints.size() * sizeof(float);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VBOSize, planePoints.data(), GL_STATIC_DRAW);
	//print_gl_error();
}

void ImagePlane::updateVAO() {
	glDeleteVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(Shaders::planeProgram->position_id);
	glVertexAttribPointer(Shaders::planeProgram->position_id, 4,
		GL_FLOAT, GL_FALSE, sizeof(float4), 0);

	//print_gl_error();
}

void ImagePlane::render(glm::mat4 parent_matrix) {
	glm::mat4 finalMatrix = parent_matrix * M;
	
	glUseProgram(Shaders::planeProgram->program);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUniformMatrix4fv(Shaders::planeProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
	glActiveTexture(GL_TEXTURE0);
	//print_gl_error();

	glBindTexture(GL_TEXTURE_2D, textureID);
	glUniform1i(Shaders::planeProgram->texture0_id, 0);
	glUniform3fv(Shaders::planeProgram->offset_uniform_id,
		1, &offset.x);
	glUniform1f(Shaders::planeProgram->width_uniform_id, width);
	glUniform1f(Shaders::planeProgram->height_uniform_id, height);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	print_gl_error();
}

void ImagePlane::updateImage(string imagePath) {
	/* Loads and uploads a texture to the GPU */
	int comp;
	int forceChannels = 4;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image = stbi_load(imagePath.c_str(), &width, &height, &comp, forceChannels);
	assert(image != 0);

	offset = make_float4(width * .5, height * .5, 0.0, 0.0);

	glGenTextures(1, &textureID);
	//print_gl_error();

	glBindTexture(GL_TEXTURE_2D, textureID);
	//print_gl_error();

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//print_gl_error();

	stbi_image_free(image);
}

GLuint ImagePlane::getTextureID() {
	return this->textureID;
}
