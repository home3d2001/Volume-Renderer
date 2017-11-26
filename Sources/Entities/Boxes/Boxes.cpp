#include "Boxes.hpp"
namespace Entities {
	int Boxes3D::count = 0;

	Boxes3D::Boxes3D() {
		updateVAO();
		count++;
		totalBoxes = 0;
	}

	void Boxes3D::add(float4 center, float4 scale, float4 color) {
		cl_int error = 0;
		bool old;
		Box b;
		b.center = center;
		b.scale = scale;
		b.color = color;
		boxes.push_back(b);
		totalBoxes++;

		// Upload data to OpenCL buffer here
		error |= CLFW::getBuffer(boxesBuffer, "boxes" + count,
			CLFW::NextPow2(boxes.size()) * sizeof(Box), old, true);
		error |= CLFW::Upload<Box>(boxes, boxesBuffer);
		assert_cl_error(error);
		updateVBO();
	}

	void Boxes3D::clear() {
		boxes.clear();
		totalBoxes = 0;
	}

	void Boxes3D::updateVBO() {
		cl_int error = 0;

		/* Resize OpenGL VBO if required */
			// 12 lines per box, 2 points per line, 2 float4s per point
		if (VBOSize < totalBoxes * 12 * 2 * 2 * sizeof(float4)) {
			VBOSize = CLFW::NextPow2(totalBoxes * 12 * 2 * 2 * sizeof(float4));
			glDeleteBuffers(1, &VBO);
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, VBOSize, 0, GL_DYNAMIC_DRAW);
			glFinish();
			VBO_cl = cl::BufferGL(CLFW::DefaultContext, CL_MEM_READ_WRITE, VBO, &error);
			updateVAO();
		}

		cl::Kernel &kernel = CLFW::Kernels["CreateBoxes3DVBO"];
		cl::CommandQueue &queue = CLFW::DefaultQueue;

		/* Wait for OpenGL to finish */
		glFinish();
		error |= queue.finish();

		/* Aquire the VBO for modification. */
		std::vector<cl::Memory> mem_objs;
		mem_objs.push_back(VBO_cl);
		error |= queue.enqueueAcquireGLObjects(&mem_objs);

		/* Modify the VBO */
		error |= kernel.setArg(0, boxesBuffer);
		error |= kernel.setArg(1, VBO_cl);
		error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange,
			cl::NDRange(24 * totalBoxes), cl::NDRange());

		/* Release the VBO */
		error |= queue.enqueueReleaseGLObjects(&mem_objs);
		error |= queue.finish();
		assert_cl_error(error);
	}

	void Boxes3D::updateVAO() {
		glDeleteVertexArrays(1, &VAO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), 0);
		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), (void*)(sizeof(float4)));
		//print_gl_error();
	}

	void Boxes3D::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		glm::mat4 finalMatrix = projection * parent_matrix * transform.localToParentMatrix.load();
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		//print_gl_error();

		Shaders::lineProgram->use();
		glBindVertexArray(VAO);
		//print_gl_error();

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//print_gl_error();

		glUniformMatrix4fv(
			Shaders::lineProgram->matrix_id, 1, 0, &(finalMatrix[0].x));
		//print_gl_error();

		glUniform1f(Shaders::lineProgram->pointSize_id, 10.0);
		//print_gl_error();
		glLineWidth(4.);
		glDrawArrays(GL_LINES, 0, 24 * totalBoxes);
		//print_gl_error();
		glBindVertexArray(0);
	}
}