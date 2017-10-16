#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <chrono>

#include "GLUtilities/gl_utils.h"
#include "./Quadtree.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/RadixSort/RadixSort.hpp"
#include "CellResolution/ConflictCellResolution.hpp"

using namespace std;

#define benchmark(text)
#define check(error) {assert_cl_error(error);}

Quadtree::Quadtree() {
	resln = make_resln(1 << Options::max_level);
}

cl_int Quadtree::placePointsOnCurve(
	cl::Buffer points_i,
	int numPts,
	Resln resln,
	BoundingBox2D bb,
	string str,
	cl::Buffer &qpoints_o,
	cl::Buffer &zpoints_o) {
	cl_int error = 0;

	/* Quantize the points. */
	error |= Quantize2DPoints_p(points_i, numPts, bb, resln.width, str, qpoints_o);
	check(error);

	/* Convert the points to Z-Order */
	error |= QPoints2DToZPoints_p(qpoints_o, numPts, resln.bits, str, zpoints_o);
	check(error);

	return error;
}

cl_int Quadtree::buildVertexQuadtree(
	cl::Buffer zpoints_i,
	int numPts,
	Resln resln,
	BoundingBox2D bb,
	string uniqueString,
	cl::Buffer &quadtree_o,
	cl_int &numQuadnodes_o,
	cl::Buffer &leaves_o,
	cl_int &totalLeaves_o)
{
	cl_int error = 0;
	cl_int uNumPts = numPts;
	cl::Buffer zpoints_copy, brt, nullBuffer;

	/* Make a copy of the zpoints. */
	CLFW::getBuffer(zpoints_copy, uniqueString + "zptscpy",
		CLFW::NextPow2(sizeof(big) * numPts));
	error |= CLFW::DefaultQueue.enqueueCopyBuffer(zpoints_i, zpoints_copy,
		0, 0, numPts * sizeof(big));
	check(error);

	/* Radix sort the zpoints */
	error |= RadixSortBig_p(zpoints_copy, numPts,
		resln.mbits, uniqueString);
	check(error);

	/* Unique the zpoints */
	error |= UniqueSorted(zpoints_copy, numPts, uniqueString,
		uNumPts);
	check(error);

	/* Build a binary radix tree*/
	error |= BuildBinaryRadixTree_p(zpoints_copy, uNumPts,
		resln.mbits, uniqueString, brt);
	check(error);

	/* Convert the binary radix tree to a quadtree */
	error |= BinaryRadixToQuadtree_p(brt, false, nullBuffer, uNumPts,
		uniqueString, quadtree_o, numQuadnodes_o);
	check(error);

	/* Use the internal quadtree nodes to calculate leaves */
	error |= GetQuadLeaves_p(quadtree_o, numQuadnodes_o, leaves_o, totalLeaves_o);
	check(error);

	return error;
}

cl_int Quadtree::buildPrunedQuadtree(
	cl::Buffer zpoints_i,
	cl::Buffer pntColors_i,
	int numPts,
	Resln resln,
	BoundingBox2D bb,
	string str,
	cl::Buffer &quadtree_o,
	cl_int &numQuadnodes_o,
	cl::Buffer &leaves_o,
	cl_int &totalLeaves_o)
{
	cl_int error = 0;
	cl_int uniqueTotalPoints = numPts;
	cl::Buffer zpoints_copy, pntColors_copy, brt, brtColors;

	/* Make a copy of the zpoints and colors. */
	CLFW::getBuffer(zpoints_copy, str + "zptscpy", sizeof(big) * CLFW::NextPow2(numPts));
	error |= CLFW::DefaultQueue.enqueueCopyBuffer(zpoints_i, zpoints_copy,
		0, 0, numPts * sizeof(big));

	CLFW::getBuffer(pntColors_copy, str + "colscpy", sizeof(cl_int) * CLFW::NextPow2(numPts));
	error |= CLFW::DefaultQueue.enqueueCopyBuffer(pntColors_i, pntColors_copy,
		0, 0, sizeof(cl_int) * numPts);
	
	/* Radix sort the zpoints */
	error |= RadixSortBigToInt_p(zpoints_copy, pntColors_copy, numPts, resln.mbits, "sortZpoints");
	check(error);
	
	/* Unique the zpoints */
	error |= UniqueSortedBUIntPair(zpoints_copy, pntColors_copy, numPts, str,
		uniqueTotalPoints);
	check(error);

	/* Build a colored binary radix tree*/
	error |= BuildColoredBinaryRadixTree_p(zpoints_copy, pntColors_copy, uniqueTotalPoints,
		resln.mbits, str, brt, brtColors);
	check(error);

	/* Identify required cells */
	error |= PropagateBRTColors_p(brt, brtColors, uniqueTotalPoints - 1, str);
	check(error);
	
	/* Convert the binary radix tree to a quadtree*/
	error |= BinaryRadixToQuadtree_p(brt, true, brtColors, uniqueTotalPoints, str,
		quadtree_o, numQuadnodes_o); //occasionally currentSize is 0...
	check(error);

	/* Use the internal quadtree nodes to calculate leaves */
	error |= GetQuadLeaves_p(quadtree_o, numQuadnodes_o, leaves_o, totalLeaves_o);
	check(error);

	return error;
}

cl_int Quadtree::initializeConflictCellDetection(
	cl::Buffer &zpoints_i,
	cl::Buffer &lines_i,
	cl_int numLines,
	Resln &resln,
	cl::Buffer &quadtree_i,
	cl_int numQuadNodes,
	cl::Buffer &lineIndices_o,
	cl::Buffer &LCPBounds_o
) {
	cl_int error = 0;
	/* Compute line bounding cells and generate the unordered line indices. */
	cl::Buffer LineLCPs;
	error |= GetLineLCPs_p(lines_i, numLines, zpoints_i, resln.mbits, LineLCPs);
	error |= InitializeFacetIndices_p(numLines, lineIndices_o);
	check(error);

	/* For each bounding cell, look up it's surrounding quadnode in the tree. */
	cl::Buffer LCPToQuadNode;
	error |= LookUpQuadnodeFromLCP_p(LineLCPs, numLines, quadtree_i, LCPToQuadNode);
	check(error);

	/* Sort the node to line pairs by key. This gives us a node to facet mapping
	for conflict cell detection. */
	error |= RadixSortIntToInt_p(LCPToQuadNode, lineIndices_o, numLines, 8 * sizeof(cl_int), "facetMapping");
	check(error);

	/* For each octnode, determine the first and last bounding cell index to be
	used for conflict cell detection. */
	error |= GetLCPBounds_p(LCPToQuadNode, numLines, numQuadNodes, LCPBounds_o);
	check(error);
	return error;
}

cl_int Quadtree::findConflictCells(
	cl::Buffer &quadtree_i,
	cl_int numQuadNodes,
	cl::Buffer &leaves_i,
	cl_int numLeaves,
	cl::Buffer &qpoints_i,
	cl::Buffer &zpoints_i,
	cl::Buffer &lines_i,
	cl_int numLines,
	bool keepCollisions,
	Resln &resln,
	cl::Buffer &conflicts_o,
	cl_int &numConflicts
) {
	cl_int error = 0;

	/* Initialize the node to facet mapping*/
	cl::Buffer LineLCPs, lineIndices, LCPBounds;
	error |= initializeConflictCellDetection(zpoints_i, lines_i, numLines, resln,
		quadtree_i, numQuadNodes, lineIndices, LCPBounds);

	/* Use that mapping to find conflict cells*/
	cl::Buffer sparseConflicts;
	error |= FindConflictCells_p(quadtree_i, leaves_i, numLeaves, lineIndices,
		LCPBounds, lines_i, numLines, qpoints_i, resln.width, keepCollisions, sparseConflicts);
	check(error);
	
	/* Compact the non-conflict cells to the right */
	cl::Buffer cPred, cAddr;
	error |= CLFW::getBuffer(cPred, "cPred", sizeof(cl_int) * CLFW::NextPow2(numLeaves));
	error |= CLFW::getBuffer(cAddr, "cAddr", sizeof(cl_int) * CLFW::NextPow2(numLeaves));
	error |= PredicateConflicts_p(sparseConflicts, numLeaves, "", cPred);
	error |= StreamScan_p(cPred, numLeaves, "cnflctaddr", cAddr);
	error |= CLFW::Download<cl_int>(cAddr, numLeaves - 1, numConflicts);
	error |= CLFW::getBuffer(conflicts_o, "conflicts", sizeof(Conflict) * CLFW::NextPow2(numLeaves));
	error |= CompactConflicts_p(sparseConflicts, cPred, cAddr, numLeaves, conflicts_o);
	check(error);
	return error;
}

cl_int Quadtree::generateResolutionPoints(
	cl::Buffer &conflicts_i,
	cl_int numConflicts,
	Resln &resln,
	cl::Buffer &qpoints_i,
	cl::Buffer &resPts,
	cl::Buffer &resZPts,
	cl_int &numResPts
) {
	cl_int error = 0;

	/* Use the conflicts to initialize data required to calculate resolution points */
	cl::Buffer conflictInfo, numPtsPerConflict, scannedNumPtsPerConflict, predPntToConflict,
		pntToConflict;
	error |= GetResolutionPointsInfo_p(conflicts_i, numConflicts, qpoints_i, conflictInfo,
		numPtsPerConflict);
	/* Scan the pts per conflict to determine beginning and ending addresses for res pts*/
	error |= CLFW::getBuffer(scannedNumPtsPerConflict, "snptspercnflct",
		sizeof(cl_int) * CLFW::NextPow2(numConflicts));
	error |= StreamScan_p(numPtsPerConflict, numConflicts, "conflictInfo",
		scannedNumPtsPerConflict);
	error |= CLFW::Download<cl_int>(scannedNumPtsPerConflict, numConflicts - 1, numResPts);
	/* Create a res pnt to conflict info mapping so we can determine resolution points
	in parallel. */
	error |= PredicatePointToConflict_p(scannedNumPtsPerConflict, numConflicts, numResPts,
		predPntToConflict);
	error |= CLFW::getBuffer(pntToConflict, "pnt2Conflict", CLFW::NextPow2(sizeof(cl_int) * numResPts));
	error |= StreamScan_p(predPntToConflict, numResPts, "pnt2Conf", pntToConflict);
	check(error);

	/* Get the resolution points */
	error |= GetResolutionPoints_p(conflicts_i, conflictInfo, scannedNumPtsPerConflict,
		numResPts, pntToConflict, qpoints_i, resPts);

	if (Options::showResolutionPoints) {
		int offset = resolutionPoints.size();
		resolutionPoints.resize(resolutionPoints.size() + numResPts);
		vector<intn> temp;
		CLFW::Download<intn>(resPts, numResPts, temp);
		for (int i = 0; i < temp.size(); ++i) {
			resolutionPoints[offset + i] = UnquantizePoint(&temp[i], &bb.minimum, resln.width, bb.maxwidth);
		}
	}

	/* Convert to z-order */
	error |= QPoints2DToZPoints_p(resPts, numResPts, resln.bits, "res", resZPts);
	check(error);
	return error;
}

cl_int Quadtree::combinePoints(
	cl::Buffer &qpoints_i,
	cl::Buffer &zpoints_i,
	cl::Buffer &pntCols_i,
	cl_int numPts,
	cl::Buffer &resPts_i,
	cl::Buffer &resZPts_i,
	cl_int numResPts,
	cl_int iteration,
	cl::Buffer &combinedQPts_o,
	cl::Buffer &combinedZPts_o,
	cl::Buffer &combinedCols_o
) {
	cl_int error = 0;

	cl::CommandQueue &queue = CLFW::DefaultQueue;
	error |= CLFW::getBuffer(combinedQPts_o, "qpoints" + std::to_string(iteration),
		CLFW::NextPow2(numPts + numResPts) * sizeof(intn));
	error |= queue.enqueueCopyBuffer(qpoints_i, combinedQPts_o, 0, 0, numPts * sizeof(intn));
	error |= queue.enqueueCopyBuffer(resPts_i, combinedQPts_o, 0,
		numPts * sizeof(intn), numResPts * sizeof(intn));
	check(error);

	error |= CLFW::getBuffer(combinedZPts_o, "zpoints" + std::to_string(iteration),
		CLFW::NextPow2(numPts + numResPts) * sizeof(big));
	error |= queue.enqueueCopyBuffer(zpoints_i, combinedZPts_o, 0, 0, numPts * sizeof(big));
	error |= queue.enqueueCopyBuffer(resZPts_i, combinedZPts_o, 0, numPts * sizeof(big),
		numResPts * sizeof(big));
	check(error);

	error |= CLFW::getBuffer(combinedCols_o, "ptcol" + std::to_string(iteration),
		CLFW::NextPow2(numPts + numResPts) * sizeof(cl_int));
	error |= queue.enqueueCopyBuffer(pntCols_i, combinedCols_o, 0, 0, numPts * sizeof(cl_int));
	vector<cl_int> resCols(numResPts);
	for (int i = 0; i < numResPts; ++i) resCols[i] = -3 - i;
	error |= queue.enqueueWriteBuffer(combinedCols_o, CL_TRUE, numPts * sizeof(cl_int),
		numResPts * sizeof(cl_int), resCols.data());
	check(error);
	return error;
}

cl_int Quadtree::resolveAmbiguousCells(
	cl::Buffer &quadtree_i,
	cl_int &numQuadNodes,
	cl::Buffer &leaves_i,
	cl_int &numLeaves,
	cl::Buffer lines_i,
	cl_int numLines,
	cl::Buffer qpoints_i,
	cl::Buffer zpoints_i,
	cl::Buffer pntCols_i,
	cl_int numPts,
	cl_int iteration
) {
	if (numLines <= 1) return CL_SUCCESS;
	cl_int error = 0;
	if (iteration >= Options::maxConflictIterations) {
		if (Options::showObjectIntersections) {
			error |= findConflictCells(quadtree_i, numQuadNodes, leaves_i, numLeaves,
				qpoints_i, zpoints_i, lines_i, numLines, true, resln, conflictsBuffer, numConflicts);
			CLFW::Download<Conflict>(conflictsBuffer, numConflicts, conflicts);
		}
		return error;
	};

	/* Determine conflicts to resolve */
	error |= findConflictCells(quadtree_i, numQuadNodes, leaves_i, numLeaves,
		qpoints_i, zpoints_i, lines_i, numLines, false, resln, conflictsBuffer, numConflicts);
	check(error);

	if (numConflicts == 0) return error;

	/* Use the conflicts to generate resolution points */
	cl::Buffer resPts, resZPoints; cl_int numResPts;
	error |= generateResolutionPoints(conflictsBuffer, numConflicts, resln, qpoints_i, resPts, resZPoints, numResPts);
	check(error);

	/* Combine the original and generated resolution points */
	cl::Buffer combinedQPts, combinedZPts, combinedCols;
	combinePoints(qpoints_i, zpoints_i, pntCols_i, numPts,
		resPts, resZPoints, numResPts, iteration, combinedQPts, combinedZPts, combinedCols);
	check(error);


	/* Build a quadtree from the combined points */
	cl::Buffer combinedQuadtree, combinedLeaves;
	cl_int combinedQuadSize, combinedLeafSize;
	if (Options::pruneOctree)
		error |= buildPrunedQuadtree(combinedZPts, combinedCols, numPts + numResPts, resln, bb,
			"res" + iteration, combinedQuadtree, combinedQuadSize, combinedLeaves, combinedLeafSize);
	else
		error |= buildVertexQuadtree(combinedZPts, numPts + numResPts, resln, bb,
			"res" + iteration, combinedQuadtree, combinedQuadSize, combinedLeaves, combinedLeafSize);
	check(error);
	int originalQuadSize = numQuadNodes;
	quadtree_i = combinedQuadtree;
	numQuadNodes = combinedQuadSize;
	leaves_i = combinedLeaves;
	numLeaves = combinedLeafSize;

	/* If the resolution points don't effect the quadtree, quit resolving. */
	if (combinedQuadSize == originalQuadSize) {
		if (Options::showObjectIntersections) {
			error |= findConflictCells(quadtree_i, numQuadNodes, leaves_i, numLeaves,
				qpoints_i, zpoints_i, lines_i, numLines, true, resln, conflictsBuffer, numConflicts);
			CLFW::Download<Conflict>(conflictsBuffer, numConflicts, conflicts);
		}
		return error;
	}

	/* resolve further conflicts */
	if (iteration < Options::maxConflictIterations)
		resolveAmbiguousCells(quadtree_i, numQuadNodes, leaves_i, numLeaves,
			lines_i, numLines, combinedQPts,
			combinedZPts, combinedCols, numPts + numResPts, iteration + 1);
	else {
		if (Options::showObjectIntersections) {
			error |= findConflictCells(quadtree_i, numQuadNodes, leaves_i, numLeaves,
				qpoints_i, zpoints_i, lines_i, numLines, true, resln, conflictsBuffer, numConflicts);
			CLFW::Download<Conflict>(conflictsBuffer, numConflicts, conflicts);
		}
	}

	return error;
}

void Quadtree::clear() {
	quadtreeSize = 0;
	numResPoints = 0;
	nodes.clear();
	conflicts.clear();
	resolutionPoints.clear();
}

void Quadtree::generate_boxes() {
	cl_int error = 0;
	cl::CommandQueue &queue = CLFW::DefaultQueue;
	cl::Kernel &kernel = CLFW::Kernels["CreateQuadtreeBoxesKernel"];
	error |= CLFW::getBuffer(boxesBuffer, "boxes" + count, sizeof(Box) * totalLeaves);

	error |= kernel.setArg(0, quadtreeBuffer);
	error |= kernel.setArg(1, leavesBuffer);
	error |= kernel.setArg(2, bb);
	error |= kernel.setArg(3, boxesBuffer);
	
	//vector<QuadNode> quadtree_vec;
	//vector<Leaf> leaf_vec;
	//CLFW::Download<QuadNode>(quadtreeBuffer, quadtreeSize, quadtree_vec);
	//CLFW::Download<Leaf>(leavesBuffer, totalLeaves, leaf_vec);

	error |= queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(totalLeaves), cl::NullRange);
	//error |= queue.finish();
	//vector<Box> boxes_vec;
	//CLFW::Download<Box>(boxesBuffer, totalLeaves, boxes_vec);
	totalBoxes = totalLeaves;
	updateVBO();
}

void Quadtree::build_internal (cl_int numPts, cl_int numLines) {
	CLFW::DefaultQueue = CLFW::Queues[0];
	cl_int error = 0;
	using namespace std::chrono;

	/* Place the points on a Z-Order curve */
	error |= placePointsOnCurve(pointsBuffer, numPts, resln, bb, "initial", qpoints, zpoints);
	check(error);

//	Options::pruneOctree = false;
	/* Build the initial quadtree */
	CLFW::DefaultQueue = CLFW::Queues[0];
	if (Options::pruneOctree)
		error |= buildPrunedQuadtree(zpoints, pntColorsBuffer, numPts, resln, bb, "initial",
			quadtreeBuffer, quadtreeSize, leavesBuffer, totalLeaves);
	else
		error |= buildVertexQuadtree(zpoints, numPts, resln, bb, "initial", quadtreeBuffer,
			quadtreeSize, leavesBuffer, totalLeaves);
	check(error);

	//resolutionRequired = false;
	//Options::resolveAmbiguousCells = false;
	/* Finally, resolve the ambiguous cells. */
	if (resolutionRequired && Options::resolveAmbiguousCells) {
		error |= resolveAmbiguousCells(quadtreeBuffer, quadtreeSize, leavesBuffer, totalLeaves,
			linesBuffer, numLines, qpoints, zpoints, pntColorsBuffer, numPts, 0);
		check(error);
	}

	/* Download the quadtree for CPU usage */
	//error |= CLFW::Download<QuadNode>(quadtreeBuffer, quadtreeSize, nodes);
	check(error);
}

void Quadtree::build(vector<float2> &points, vector<cl_int> &pointColors, vector<Line> &lines,
	BoundingBox2D bb) {
	CLFW::DefaultQueue = CLFW::Queues[0];
	cl_int error = 0;

	/* Clear the old quadtree */
	clear();

	if (points.size() == 0) return;

	this->bb = bb;

	/* Upload the data to OpenCL buffers */
	error |= CLFW::getBuffer(pointsBuffer, "pts", CLFW::NextPow2(points.size()) * sizeof(float2));
	error |= CLFW::getBuffer(pntColorsBuffer, "ptcolr", CLFW::NextPow2(points.size()) * sizeof(cl_int));
	error |= CLFW::getBuffer(linesBuffer, "lines", CLFW::NextPow2(lines.size()) * sizeof(Line));
	check(error);

	error |= CLFW::Upload<float2>(points, pointsBuffer);
	error |= CLFW::Upload<cl_int>(pointColors, pntColorsBuffer);
	error |= CLFW::Upload<Line>(lines, linesBuffer);
	check(error);

	//glm::mat4 matrix(1.0);
	////matrix = glm::translate(matrix, glm::vec3(.2, .2, 0.0));
	//error |= multiplyM4V4_p(untranslatedPointsBuffer, points.size(), matrix, "translated", pointsBuffer);
	//CLFW::DefaultQueue.finish();
	//check(error);

	build_internal(points.size(), lines.size());
}

void Quadtree::build(const PolyLines *polyLines) {
	CLFW::DefaultQueue = CLFW::Queues[0];
	cl_int error = 0;

	/* Clear the old quadtree */
	clear();

	/* Don't build a quadtree if there are no points. */
	if (polyLines->lines.size() == 0) return;

	//TODO: parallelize this calculation...
	getBoundingBox(polyLines->float2s, polyLines->float2s.size(), bb);
	resolutionRequired = polyLines->lasts.size() > 1;
	/*bb.minimum = make_float2(-1.0, -1.0);
	bb.maximum = make_float2(1.0, 1.0);
	bb.maxwidth = 2.0;*/
	if (polyLines->animated) {
		this->pointsBuffer = polyLines->float2World;
		bb.minimum = make_float2(-1.0, -1.0);
		bb.maximum = make_float2(1.0, 1.0);
		bb.maxwidth = 2.0;
	}
	else
		this->pointsBuffer = polyLines->float2Local;
	this->pntColorsBuffer = polyLines->pointColorsBuffer;
	this->linesBuffer = polyLines->linesBuffer;
	
	CLFW::DefaultQueue.finish();
	auto start_time = chrono::high_resolution_clock::now();
	build_internal(polyLines->float2s.size(), polyLines->lines.size());
	CLFW::DefaultQueue.finish();
	auto end_time = chrono::high_resolution_clock::now();

	cout << chrono::duration_cast<chrono::microseconds>(end_time - start_time).count() << endl;
	//cout << "quadtree size " << quadtreeSize << endl;
	//cout << "leaves size " << totalLeaves << endl;
	Boxes3D::clear();
	generate_boxes();
}

void Quadtree::build(const Polygons* polygons) {
	CLFW::DefaultQueue = CLFW::Queues[0];
	cl_int error = 0;

	/* Clear the old quadtree */
	clear();

	/* Don't build a quadtree if there are no points. */
	if (polygons->points.size() == 0) return;

	//TODO: parallelize this calculation...
	getBoundingBox(polygons->points, polygons->points.size(), bb);
	resolutionRequired = polygons->lasts.size() > 1;
	//bb.maxwidth = 4.0;
	//bb.minimum = make_float2(-2.4, -1.6);
	//bb.maximum = make_float2(2.4, 1.6);

	this->pointsBuffer = polygons->pointsBuffer;
	this->pntColorsBuffer = polygons->pointColorsBuffer;
	this->linesBuffer = polygons->linesBuffer;

	build_internal(polygons->points.size(), polygons->lines.size());
}

inline float2 getMinFloat(const float2 a, const float2 b) {
	float2 result;
	for (int i = 0; i < DIM2D; ++i) {
		result.s[i] = (a.s[i] < b.s[i]) ? a.s[i] : b.s[i];
	}
	return result;
}

inline float2 getMaxFloat(const float2 a, const float2 b) {
	floatn result;
	for (int i = 0; i < DIM2D; ++i) {
		result.s[i] = (a.s[i] > b.s[i]) ? a.s[i] : b.s[i];
	}
	return result;
}

void Quadtree::getBoundingBox(const vector<float2> &points, const int totalPoints,
	BoundingBox2D &bb) {
	benchmark("getBoundingBox");

	if (Options::bbxmin == -1 && Options::bbxmax == -1) {
		//Probably should be parallelized...
		float2 minimum = points[0];
		float2 maximum = points[0];
		for (int i = 1; i < totalPoints; ++i) {
			minimum = getMinFloat(points[i], minimum);
			maximum = getMaxFloat(points[i], maximum);
		}
		bb = BB2D_initialize(&minimum, &maximum);
		bb = BB2D_make_centered_square(&bb);
	}
	else {
		bb.initialized = true;
		bb.minimum = make_floatn(Options::bbxmin, Options::bbymin);
		bb.maximum = make_floatn(Options::bbxmax, Options::bbymax);
		bb.maxwidth = BB2D_max_size(&bb);
	}
}

void Quadtree::render(glm::mat4 parent_matrix) {
	Boxes3D::render(parent_matrix);
}
#undef benchmark
#undef check