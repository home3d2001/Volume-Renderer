#include "ImagePlane.hpp"
#include "Vector/vec.h"

namespace Entities {
	int ImagePlane::count = 0;

	ImagePlane::ImagePlane(std::shared_ptr<Texture> texture) {
		this->texture = texture;
		count++;
		offset = make_float4(texture->width * .5, texture->height * .5, 0.0, 0.0);
		width = 1.0;
		height = 1.0;

		updateVBO();
		updateVAO();
	}

	void ImagePlane::setInterpolation(bool useInterpolation) {
		this->useInterpolation = useInterpolation;
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

	void ImagePlane::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		glm::mat4 finalMatrix = projection * parent_matrix * transform.LocalToParentMatrix();
		glUseProgram(Shaders::planeProgram->program);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glUniformMatrix4fv(Shaders::planeProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		glActiveTexture(GL_TEXTURE0);
		print_gl_error();

		glBindTexture(GL_TEXTURE_2D, texture->textureID);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (useInterpolation) ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (useInterpolation) ? GL_LINEAR : GL_NEAREST);

		glUniform1i(Shaders::planeProgram->texture0_id, 0);
		glUniform3fv(Shaders::planeProgram->offset_uniform_id,
			1, &offset.x);
		glUniform1f(Shaders::planeProgram->width_uniform_id, width);
		glUniform1f(Shaders::planeProgram->height_uniform_id, height);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		print_gl_error();
	}
}