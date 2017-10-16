#pragma once

#include "clfw.hpp"
#include "Shaders/Shaders.hpp"
#include "GLUtilities/Polylines/Polylines.hpp"
#include "GLUtilities/Polygons.h"
#include "GLUtilities/gl_utils.h"
#include "GLUtilities/Boxes/Boxes.hpp"
#include "Options/Options.h"
#include "BinaryRadixTree/BuildBRT.hpp"
#include "BoundingBox/BoundingBox.h"
#include <glm/glm.hpp>

extern "C" {
#include "OctreeResolution/Resln.h"
#include "Line/Line.h"
}
#include "Octree/OctNode.h"
#include "Quantize/Quantize.hpp"
#include "ZOrder/ZOrder.hpp"
#include "CellResolution/ConflictCellDetection.h"

class Octree : public Boxes3D {
public:
	std::vector<OctNode> nodes;
	std::vector<Conflict> conflicts;
	std::vector<floatn> resolutionPoints;
	BoundingBox3D bb;
	Resln resln;
private:
	int octreeSize;
	int numPts;
	int numLeaves;
	int numResPoints;
	cl_int numConflicts = 0;
	bool resolutionRequired = true;

	cl::Buffer pointsBuffer;
	cl::Buffer pntColorsBuffer;
	cl::Buffer qpoints;
	cl::Buffer zpoints;
	cl::Buffer zpointsCopy;
	cl::Buffer linesBuffer;
	cl::Buffer resQPoints;
	cl::Buffer leavesBuffer;
	cl::Buffer octreeBuffer;
	cl::Buffer conflictsBuffer; 

	//void getPoints(const PolyLines2D *polyLines, vector<floatn> &points, vector <cl_int> &pointColors, std::vector<Line> &lines);
	void getBoundingBox(const vector<float3> &points, const int totalPoints, BoundingBox3D &bb);

public:
	Octree();
	void render(glm::mat4 parent_matrix);
	void build(const PolyLines* lines);
	void build(vector<float3> &points, vector<cl_int> &pointColors, vector<Line> &lines, BoundingBox3D bb);

private:
	void generate_boxes();
	void build_internal(cl_int numPts, cl_int numLines);

	void clear();
	cl_int placePointsOnCurve(cl::Buffer points_i, int totalPoints, Resln resln, BoundingBox3D bb, string uniqueString, cl::Buffer &qpoints_o, cl::Buffer &zpoints_o);
	cl_int buildVertexOctree(cl::Buffer points_i, int totalPoints, Resln resln, BoundingBox3D bb, string uniqueString, cl::Buffer &octree_o, cl_int &totalOctnodes_o, cl::Buffer &leaves_o, cl_int &totalLeaves_o);
	cl_int buildPrunedOctree(cl::Buffer points_i, cl::Buffer pntColors_i, int totalPoints, Resln resln, BoundingBox3D bb, string uniqueString, cl::Buffer &octree_o, cl_int &totalOctnodes_o, cl::Buffer &leaves_o, cl_int &totalLeaves_o);
	/*cl_int resolveAmbiguousCells(cl::Buffer &octree_i, cl_int &totalOctNodes, cl::Buffer leaves_i, cl_int totalLeaves, cl::Buffer lines_i, cl_int totalLines, cl::Buffer qpoints_i, cl::Buffer zpoints_i, cl::Buffer pntCols_i, cl_int totalPoints, cl_int iteration);
	cl_int initializeConflictCellDetection( cl::Buffer &zpoints_i, cl::Buffer &lines_i, cl_int numLines, Resln &resln, cl::Buffer &octree_i, cl_int numOctNodes, cl::Buffer &lineIndices_o, cl::Buffer &LCPBounds_o);
	cl_int findConflictCells( cl::Buffer &octree_i, cl_int numOctNodes, cl::Buffer &leaves_i, cl_int numLeaves, cl::Buffer &qpoints_i, cl::Buffer &zpoints_i, cl::Buffer &lines_i, cl_int numLines, bool keepCollisions, Resln &resln, cl::Buffer &conflicts_o, cl_int &numConflicts );
	cl_int generateResolutionPoints( cl::Buffer &conflicts_i, cl_int numConflicts, Resln &resln, cl::Buffer &qpoints_i, cl::Buffer &resPts, cl::Buffer &resZPts, cl_int &numResPts );
	cl_int combinePoints( cl::Buffer &qpoints_i, cl::Buffer &zpoints_i, cl::Buffer &pntCols_i, cl_int numPts, cl::Buffer &resPts_i, cl::Buffer &resZPts_i, cl_int numResPts, cl_int iteration, cl::Buffer &combinedQPts_o, cl::Buffer &combinedZPts_o, cl::Buffer &combinedCols_o );*/
};
