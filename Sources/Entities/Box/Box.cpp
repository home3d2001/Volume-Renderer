#include "Box.hpp"
#include "Vector/vec.h"
#include "Entities/Cameras/OrbitCamera.h"

namespace Entities {

	int Box::count = 0;


	Box::Box()
	{
		count++;
	
		updateVBO();
		updateVAO();
	}

	void Box::updateVBO() {
		using namespace glm;
		using namespace std;

		/* Create cube to render volume in */

		// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
		GLfloat cubePoints[] = {
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

		/* Upload the points */
		glGenBuffers(1, &pointsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glBufferData(GL_ARRAY_BUFFER, pointsVBOSize, cubePoints, GL_STATIC_DRAW);

	}

	void Box::updateVAO() {
		glGenVertexArrays(1, &linesVAO);
		glBindVertexArray(linesVAO);

		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glEnableVertexAttribArray(Shaders::uniformColorProgram->position_id);
		glVertexAttribPointer(Shaders::uniformColorProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);

		/*glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, sizeof(float4), 0);*/

		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO);

		//glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		//glEnableVertexAttribArray(Shaders::raycastVolProgram->position_id);
		//glVertexAttribPointer(Shaders::raycastVolProgram->position_id, 4,
		//	GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		//print_gl_error();

		//glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
		//print_gl_error();
		//glEnableVertexAttribArray(Shaders::raycastVolProgram->color_id);
		//print_gl_error();
		//glVertexAttribPointer(Shaders::raycastVolProgram->color_id, 4,
		//GL_FLOAT, GL_FALSE, sizeof(float4), 0);
		print_gl_error();
	}

	void Box::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		glm::mat4 finalMatrix = projection * parent_matrix * transform.localToParentMatrix.load();

		Shaders::uniformColorProgram->use();


		/* Edge points*/
		glBindVertexArray(linesVAO);
		glUniformMatrix4fv(Shaders::uniformColorProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		//glUniform1f(Shaders::lineProgram->pointSize_id, 10.0);
		glUniform4fv(Shaders::uniformColorProgram->color_uniform_id, 1, &color.x);
		glDrawArrays(GL_TRIANGLES, 0, pointsVBOSize / sizeof(float4));
		print_gl_error();

		Entity::render(parent_matrix * transform.localToParentMatrix.load(), projection);
	}

	void Box::update() {
		// For testing
		//this->rotate(glm::angleAxis(.1f, glm::vec3(1.0, 0.1, 1.0)));

		Entity::update();
	}
}
