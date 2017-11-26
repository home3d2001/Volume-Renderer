#include "Polylines.hpp"

namespace Entities {

	int PolyLines::count = 0;

	PolyLines::PolyLines(std::vector<std::string> filenames, bool is3D, bool animated) {
		uid = count;
		count++;
		this->animated = animated;

		/* Read files */
		for (std::string s : filenames) {
			if (is3D)
				read3DLines(s);
			else
				read2DLines(s);
		}

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glGenVertexArrays(1, &VAO);
		//print_gl_error();

		glBindVertexArray(VAO);
		//print_gl_error();

		glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		//print_gl_error();

		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		//print_gl_error();

		glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), 0);
		//print_gl_error();

		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), (void*)(sizeof(float4)));
		//print_gl_error();
	}

	PolyLines::PolyLines(std::vector<std::vector<float2>> objects, bool is3D, bool animated) {
		uid = count;
		count++;
		this->animated = animated;

		/* Read objects */
		for (std::vector<float2> o : objects) {
			//if (is3D)
			addLine(o);
			/*else
				read2DLines(o);*/
		}

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glGenVertexArrays(1, &VAO);
		//print_gl_error();

		glBindVertexArray(VAO);
		//print_gl_error();

		glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		//print_gl_error();

		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		//print_gl_error();

		glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), 0);
		//print_gl_error();

		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), (void*)(sizeof(float4)));
		//print_gl_error();
	}

	PolyLines::PolyLines() {
		uid = count;
		count++;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(Shaders::lineProgram->position_id);
		glEnableVertexAttribArray(Shaders::lineProgram->color_id);
		glVertexAttribPointer(Shaders::lineProgram->position_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), 0);
		glVertexAttribPointer(Shaders::lineProgram->color_id, 4,
			GL_FLOAT, GL_FALSE, 2 * sizeof(float4), (void*)(sizeof(float4)));
		//print_gl_error();
	}

	PolyLines::~PolyLines() {
		count--;
	}

	void PolyLines::clear() {
		float3s.clear();
		float2s.clear();
		lasts.clear();
		pointColors.clear();
		lines.clear();
		currentColor = 0;
		VBOSize = 0;
	}

	void PolyLines::newLine(const float2 &p) {
		newLine(make_float3(p.x, p.y, 0.0));
	}

	void PolyLines::newLine(const float3& p) {
		bool old;
		cl_int error = 0;
		cl_int oldNumPts = float3s.size();

		/* Add new "lasts" entry for the new polyline. */
		lasts.push_back(1);
		currentColor++;

		/* Update point data. */
		float3s.push_back(p);
		float2s.push_back(make_float2(p.x, p.y));
		pointColors.push_back(currentColor);

		//Update point buffer size if required, then buffer up only the new points.
		error |= CLFW::getBuffer(float3Local, std::to_string(uid) + "3DPlylnPts",
			sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float3World, std::to_string(uid) + "3DPlylnPtsW",
				sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(float2Local, std::to_string(uid) + "2DPlylnPts",
			sizeof(float2) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(pointColorsBuffer, std::to_string(uid) + "PlylnPtClrs",
			sizeof(cl_int) * CLFW::NextPow2(float3s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float2World, std::to_string(uid) + "2DPlylnPtsW",
				sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);

		error |= CLFW::Upload<float3>((float3)p, oldNumPts, float3Local);
		error |= CLFW::Upload<float2>(make_float2(p.x, p.y), oldNumPts, float2Local);
		error |= CLFW::Upload<cl_int>(currentColor, oldNumPts, pointColorsBuffer);
		assert_cl_error(error);

		updateVBO();
	}

	void PolyLines::addPoint(const float2& p) {
		addPoint(make_float3(p.x, p.y, 0.0));
	}

	void PolyLines::addPoint(const float3& p) {
		/* Don't add a point if a new line hasn't been created first. */
		if (lasts.size() == 0) return;
		bool old;
		cl_int error = 0;
		cl_int oldNumPts = float3s.size();
		cl_int oldNumLines = lines.size();

		/* Update point/line data */
		float3s.push_back(p);
		float2s.push_back(make_float2(p.x, p.y));
		pointColors.push_back(currentColor);
		Line l;
		l.color = currentColor;
		l.first = float3s.size() - 2;
		l.second = float3s.size() - 1;
		lines.push_back(l);

		lasts[lasts.size() - 1]++;

		//Update point/line buffer size if required, then buffer up only the new points.
		error |= CLFW::getBuffer(float3Local, std::to_string(uid) + "3DPlylnPts",
			sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float3World, std::to_string(uid) + "3DPlylnPtsW",
				sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(float2Local, std::to_string(uid) + "2DPlylnPts",
			sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float2World, std::to_string(uid) + "2DPlylnPtsW",
				sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		error |= CLFW::getBuffer(pointColorsBuffer, std::to_string(uid) + "PlylnPtClrs",
			sizeof(cl_int) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(linesBuffer, std::to_string(uid) + "PlylnLn",
			sizeof(Line) * CLFW::NextPow2(lines.size()), old, true);

		error |= CLFW::Upload<float3>((float3)p, oldNumPts, float3Local);
		error |= CLFW::Upload<float2>(make_float2(p.x, p.y), oldNumPts, float2Local);
		error |= CLFW::Upload<cl_int>(currentColor, oldNumPts, pointColorsBuffer);
		error |= CLFW::Upload<Line>(l, oldNumLines, linesBuffer);
		assert_cl_error(error);

		updateVBO();
	}

	void PolyLines::replaceLast(const float2 &p) {
		replaceLast(make_float3(p.x, p.y, 0.0));
	}

	void PolyLines::replaceLast(const float3 &p) {
		/* Don't replace a point if there's nothing to replace. */
		if (float3s.size() == 0) return;
		bool old;
		cl_int error = 0;
		cl_int oldNumPts = float3s.size();
		cl_int oldNumLines = lines.size();

		/* Update point/line data */
		float2s[float2s.size() - 1] = make_float2(p.x, p.y);
		float3s[float3s.size() - 1] = p;
		pointColors[pointColors.size() - 1] = currentColor;

		//Update point/line buffer size if required, then buffer up only the new points.
		error |= CLFW::getBuffer(float3Local, std::to_string(uid) + "3DPlylnPts",
			sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float3World, std::to_string(uid) + "3DPlylnPtsW",
				sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(float2Local, std::to_string(uid) + "2DPlylnPts",
			sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float2World, std::to_string(uid) + "2DPlylnPtsW",
				sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		error |= CLFW::getBuffer(pointColorsBuffer, std::to_string(uid) + "PlylnPtClrs",
			sizeof(cl_int) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(linesBuffer, std::to_string(uid) + "PlylnLn",
			sizeof(Line) * CLFW::NextPow2(lines.size()), old, true);

		error |= CLFW::Upload<float3>((float3)p, oldNumPts - 1, float3Local);
		error |= CLFW::Upload<float2>(make_float2(p.x, p.y), oldNumPts - 1, float2Local);
		error |= CLFW::Upload<cl_int>(currentColor, oldNumPts - 1, pointColorsBuffer);
		assert_cl_error(error);

		updateVBO();
	}

	void PolyLines::addLine(const std::vector<float2> newPoints) {
		vector<float3> temp(newPoints.size());
		for (int i = 0; i < newPoints.size(); ++i) {
			temp[i] = make_float3(newPoints[i].x, newPoints[i].y, 0.0);
		}
		addLine(temp);
	}

	void PolyLines::addLine(const std::vector<float3> new3DPoints) {
		if (new3DPoints.size() <= 1) return;
		bool old;
		cl_int oldNumPts = float3s.size();
		cl_int oldNumLines = lines.size();
		cl_int error = 0;

		/* Spoof 2D points for quadtree */
		std::vector<float2> new2DPoints(new3DPoints.size());
		for (int i = 0; i < new3DPoints.size(); i++) {
			new2DPoints[i] = make_float2(new3DPoints[i].x, new3DPoints[i].y);
		}

		/* Add new "lasts" entry for the new polyline. */
		lasts.push_back(new3DPoints.size());
		currentColor++;

		/* Update point data */
		float3s.insert(std::end(float3s), std::begin(new3DPoints), std::end(new3DPoints));
		float2s.insert(std::end(float2s), std::begin(new2DPoints), std::end(new2DPoints));
		std::vector<cl_int> newColors(new3DPoints.size(), currentColor);
		pointColors.insert(std::end(pointColors), std::begin(newColors), std::end(newColors));

		/* Update point buffer size if required, then buffer only the new points. */
		error |= CLFW::getBuffer(float3Local, std::to_string(uid) + "3DPlylnPts",
			sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float3World, std::to_string(uid) + "3DPlylnPtsW",
				sizeof(float3) * CLFW::NextPow2(float3s.size()), old, true);
		error |= CLFW::getBuffer(float2Local, std::to_string(uid) + "2DPlylnPts",
			sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		if (animated)
			error |= CLFW::getBuffer(float2World, std::to_string(uid) + "2DPlylnPtsW",
				sizeof(float2) * CLFW::NextPow2(float2s.size()), old, true);
		error |= CLFW::getBuffer(pointColorsBuffer, std::to_string(uid) + "PlylnPtClrs",
			sizeof(cl_int) * CLFW::NextPow2(float3s.size()), old, true);

		error |= CLFW::Upload<float3>(new3DPoints, oldNumPts, float3Local);
		error |= CLFW::Upload<float2>(new2DPoints, oldNumPts, float2Local);
		error |= CLFW::Upload<cl_int>(newColors, oldNumPts, pointColorsBuffer);
		assert_cl_error(error);

		/* Update line data */
		std::vector<Line> newLines(new3DPoints.size() - 1);
		for (int i = 0; i < new3DPoints.size() - 1; ++i) {
			Line l;
			l.first = i + oldNumPts;
			l.second = i + oldNumPts + 1;
			l.color = currentColor;
			newLines[i] = l;
		}
		lines.insert(std::end(lines), std::begin(newLines), std::end(newLines));

		/* Update line buffer size if required, then buffer only the new lines. */
		error |= CLFW::getBuffer(linesBuffer, std::to_string(uid) + "PlylnLn",
			CLFW::NextPow2(sizeof(Line) * lines.size()), old, true);
		error |= CLFW::Upload<Line>(newLines, oldNumLines, linesBuffer);
		assert_cl_error(error);

		updateVBO();
	}

	void PolyLines::undoLine() {
		if (lasts.size() == 0) return;
		int numToRemove = lasts[lasts.size() - 1];
		float3s.resize(float3s.size() - numToRemove);
		float2s.resize(float2s.size() - numToRemove);
		pointColors.resize(pointColors.size() - numToRemove);
		lines.resize(lines.size() - (numToRemove - 1));
		lasts.pop_back();
	}

	void PolyLines::read2DLines(const string& filename) {
		ifstream in(filename.c_str());
		if (!in) {
			cerr << "Failed to read " << filename << endl;
			return;
		}

		std::vector<float2> linePts;

		float2 pnt;

		while (in >> pnt.x && in >> pnt.y) {
			linePts.push_back(pnt);
		}
		in.close();

		addLine(linePts);
	}

	void PolyLines::read3DLines(const string& filename) {
		ifstream in(filename.c_str());
		if (!in) {
			cerr << "Failed to read " << filename << endl;
			return;
		}

		std::vector<float3> linePts;

		float3 pnt;

		while (in >> pnt.x && in >> pnt.y && in >> pnt.z) {
			linePts.push_back(pnt);
		}
		in.close();

		addLine(linePts);
	}

	void PolyLines::write2DToFile(const string &foldername) {
		ofstream out;
		ofstream subout;
		out.open(foldername + "/" + "files" + ".txt");
		out.clear();
		int first = 0;
		for (int i = 0; i < lasts.size(); ++i) {
			//cout << "writing to " << foldername << "/" << i << ".bin" << endl;
			out << foldername << "/" << i << ".bin" << endl;
			subout.open(foldername + "/" + std::to_string(i) + ".bin");
			subout.clear();
			int last = lasts[i];
			for (int j = first; j < first + lasts[i]; j++) {
				subout << float2s[j].x << " " << float2s[j].y << endl;
			}
			subout.close();
			first += last;
		}
		out.close();
	}

	void PolyLines::write3DToFile(const string &foldername) {
		ofstream out;
		ofstream subout;
		out.open(foldername + "/" + "files" + ".txt");
		out.clear();
		int first = 0;
		for (int i = 0; i < lasts.size(); ++i) {
			cout << "writing to " << foldername << "/" << i << ".bin" << endl;
			out << foldername << "/" << i << ".bin" << endl;
			subout.open(foldername + "/" + std::to_string(i) + ".bin");
			subout.clear();
			int last = lasts[i];
			for (int j = first; j < first + lasts[i]; j++) {
				subout << float3s[j].x << " " << float3s[j].y << " " << float3s[j].z << endl;
			}
			subout.close();
			first += last;
		}
		out.close();
	}

	void PolyLines::updateVAO() {
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

	void PolyLines::updateVBO() {
		cl_int error = 0;
		if (lines.size() == 0) return;
		cl::CommandQueue &queue = CLFW::DefaultQueue;
		cl::Kernel &kernel = CLFW::Kernels["GeneratePointsVBO"];

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		int vboSize;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);

		/* Resize OpenGL VBO if required */
		// Lines contain 2 points. each point contains 2 float4s.
		if (vboSize < lines.size() * 2 * (2 * sizeof(cl_float4))) {
			vboSize = CLFW::NextPow2(lines.size() * 2) * 2 * sizeof(cl_float4);
			glDeleteBuffers(1, &VBO);
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vboSize, 0, GL_DYNAMIC_DRAW);
			glFinish();
			VBO_cl = cl::BufferGL(CLFW::DefaultContext, CL_MEM_READ_WRITE, VBO, &error);
			updateVAO();
		}

		/* Wait for OpenGL to finish */
		glFinish();
		error = queue.finish();

		/* Aquire the VBO for modification. */
		std::vector<cl::Memory> mem_objs;
		mem_objs.push_back(VBO_cl);
		error = queue.enqueueAcquireGLObjects(&mem_objs);

		/* Modify the VBO */
		if (animated)
			error = kernel.setArg(0, float3World);
		else
			error = kernel.setArg(0, float3Local);

		error = kernel.setArg(1, linesBuffer);
		error = kernel.setArg(2, VBO_cl);
		error = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(lines.size()), cl::NullRange);

		/* Release the VBOs*/
		error = queue.enqueueReleaseGLObjects(&mem_objs);
		error = queue.finish();
		assert_cl_error(error);
	}

	void PolyLines::render(glm::mat4 parent_matrix, glm::mat4 projection) {
		glm::mat4 finalMatrix = projection * parent_matrix;

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

		int nBufferSize;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &nBufferSize);
		//print_gl_error();
		int originalVertexArraySize = (nBufferSize / sizeof(float));
		updateVBO();
		glLineWidth(4.);
		glPointSize(20.);
		if (Options::showObjectVertices)
			glDrawArrays(GL_POINTS, 0, 2 * lines.size());
		else
			glDrawArrays(GL_LINES, 0, 2 * lines.size());
		//print_gl_error();
		glBindVertexArray(0);
	}

	void PolyLines::moveLine(int index, glm::mat4 matrix) {
		if (!animated) return;
		cl_int error = 0;
		int offset = (index == 0) ? 0 : lasts[index - 1];
		int numPtsToMove = lasts[index];

		//CLFW::DefaultQueue.enqueueCopyBuffer(float3Local, float3World, 0, 0,
			//float3s.size() * sizeof(float3));
		//CLFW::DefaultQueue.enqueueCopyBuffer(float2Local, float2BufferWorld, 0, 0,
			//float2s.size() * sizeof(float2));
		error |= multiplyM4V4_p(float3Local, float3World, numPtsToMove, offset, matrix);
		error |= multiplyM4V2_p(float2Local, float2World, numPtsToMove, offset, matrix);
		updateVBO();
		//error |= CLFW::Download<float3>(float3Buffer_animated, float3s.size(), float3s);
		assert_cl_error(error);
	}
}