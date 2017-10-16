#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "./glm/gtc/matrix_transform.hpp"
#include "Shaders/Shaders.hpp"
#include "Quantize/Quantize.h"
#include "ParallelAlgorithms/Matrix/Matrix.hpp"
extern "C" {
#include "Line/Line.h"
}
#include "clfw.hpp"
#include "Vector/vec.h"
#include "Options/Options.h"
#include "Color/Color.h"
#include <cstring>
#include <iostream>

#include "GLUtilities/Entity.h"

class PolyLines : public Entity {
private:
	static int count;
	int uid;
	int currentColor = 0;
	void read2DLines(const string& filename);
	void read3DLines(const string& filename);
public:
	bool animated = false;
	void updateVBO();
	void updateVAO();
	std::vector<cl_int> lasts;
	std::vector<float3> float3s;
	std::vector<float2> float2s;
	std::vector<cl_int> pointColors;
	std::vector<Line> lines;

	cl::Buffer float3Local;
	cl::Buffer float3World;
	cl::Buffer float2Local;
	cl::Buffer float2World;
	cl::Buffer linesBuffer;
	cl::Buffer pointColorsBuffer;

	void newLine(const float2& p);
	void newLine(const float3& p);
	void addPoint(const float2& p);
	void addPoint(const float3& p);
	void replaceLast(const float2 &p);
	void replaceLast(const float3 &p);
	void addLine(const std::vector<float2> newPoints);
	void addLine(const std::vector<float3> newPoints);
	void undoLine();
	void clear();
	void write2DToFile(const string &foldername);
	void write3DToFile(const string &foldername);
	void render(glm::mat4 parent_matrix);
	void moveLine(int index, glm::mat4 matrix);

	PolyLines(std::vector<std::string> filenames, bool is3D, bool animated = false);
	PolyLines(std::vector<std::vector<float2>> objects, bool is3D, bool animated = false);
	PolyLines();
	~PolyLines();
};