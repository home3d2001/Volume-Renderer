#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "CellResolution/ConflictCellDetection.hpp"

Scenario("Look Up Quadnode From LCP", "[conflict]") {
	Given("a quadtree and a LCP") {
		cl_int numOctNodes = readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
		vector<QuadNode> quadtree = readFromFile<QuadNode>("TestData//simple//quadtree.bin", numOctNodes);
		LCP testLCP;
		testLCP.bu.blk[0] = 15;
		testLCP.len = 4;
		Then("we can find the bounding quadnode for that LCP") {
			int index = getQuadNode(testLCP.bu, testLCP.len, quadtree.data(), &quadtree[0]);
			Require(index == 17);
		}
	}
	Given("A quadtree, and the LCPs of the facets generating that quadtree") {
		cl_int numQuadNodes = readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
		cl_int numLines = readFromFile<cl_int>("TestData//simple//numLines.bin");
		vector<QuadNode> quadtree = readFromFile<QuadNode>("TestData//simple//quadtree.bin", numQuadNodes);
		vector<LCP> lineLCPs = readFromFile<LCP>("TestData//simple//line_lcps.bin", numLines);
		When("we look up the containing quadnodes in series") {
			// Precomputed
			vector<cl_int> f_LCPToQuadnode = readFromFile<cl_int>("TestData//simple//LCPToQuadNode.bin", numLines);
			vector<cl_int> s_LCPToQuadnode;
			LookUpQuadnodeFromLCP_s(lineLCPs, quadtree, s_LCPToQuadnode);
			Then("the series results should be valid") {
				cl_int success = 1;
				for (int i = 0; i < numLines; ++i)
					success &= (f_LCPToQuadnode[i] == s_LCPToQuadnode[i]);
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<cl_int> p_LCPToQuadnode(numLines);
				cl::Buffer b_quadtree, b_lineLCPs, b_LCPToQuadnode;
				error |= CLFW::getBuffer(b_quadtree, "b_quadtree", numQuadNodes * sizeof(QuadNode));
				error |= CLFW::getBuffer(b_lineLCPs, "b_lineLCPs", CLFW::NextPow2(numQuadNodes * sizeof(LCP)));
				error |= CLFW::Upload(quadtree, b_quadtree);
				error |= CLFW::Upload(lineLCPs, b_lineLCPs);
				error |= LookUpQuadnodeFromLCP_p(b_lineLCPs, numLines, b_quadtree, b_LCPToQuadnode);
				error |= CLFW::Download(b_LCPToQuadnode, numLines, p_LCPToQuadnode);
				Require(error == CL_SUCCESS);
				cl_int success = 1;
				for (int i = 0; i < numLines; ++i)
					success &= (p_LCPToQuadnode[i] == s_LCPToQuadnode[i]);
				Require(success == true);
			}
		}
	}
}

Scenario("Get Quadnode LCP Bounds", "[conflict]") {
	Given("a LCP to Quadnode mapping, ordered by quadnode ascending") {
		//Note: the LCP to quadnode mapping ordered by octnode ascending can be aquired using the RadixSortPairsByKey,
		cl_int numLines = readFromFile<cl_int>("TestData//simple//numLines.bin");
		cl_int numQuadNodes = readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
		vector<cl_int> LCPToQuadNode = readFromFile<cl_int>("TestData//simple//Sorted_LCPToQuadNode.bin", numLines);
		When("we get the quadnode facet bounds in series") {
			vector<Pair> s_LCPBounds;
			GetLCPBounds_s(LCPToQuadNode, numLines, numQuadNodes, s_LCPBounds);
			Then("the series results are valid") {
				vector<Pair> f_LCPBounds = readFromFile<Pair>("TestData//simple//LCPBounds.bin", numQuadNodes);
				cl_int success = true;
				for (int i = 0; i < numQuadNodes; i++) {
					success &= (s_LCPBounds[i].first == f_LCPBounds[i].first);
					success &= (s_LCPBounds[i].last == f_LCPBounds[i].last);
				}
				Require(success == true);
				And_then("the series results match the parallel results") {
					cl_int error = 0;
					vector<Pair> p_LCPBounds(numQuadNodes);
					cl::Buffer b_LCPToQuadNode, b_LCPBounds;
					error |= CLFW::getBuffer(b_LCPToQuadNode, "b_LCPToQuadNode", sizeof(cl_int) * numLines);
					error |= CLFW::Upload(LCPToQuadNode, b_LCPToQuadNode);
					error |= GetLCPBounds_p(b_LCPToQuadNode, numLines, numQuadNodes, b_LCPBounds);
					error |= CLFW::Download(b_LCPBounds, numQuadNodes, p_LCPBounds);
					cl_int success = true;
					for (int i = 0; i < numQuadNodes; i++) {
						success &= (s_LCPBounds[i].first == p_LCPBounds[i].first);
						success &= (s_LCPBounds[i].last == p_LCPBounds[i].last);
					}
					Require(success == true);
				}
			}
		}
	}
}

Scenario("Find Conflict Cells", "[conflict]") {
	Given("A quadtree, that quadtree's leaves, a bcell to \n"
		+ "line mapping (see paper), bcell index bounds for \n"
		+ "each internal quadnode, the lines and cooresponding \n"
		+ "points used to generate the quadtree, and the quadtree width") 
	{
		cl_int f_numQuadNodes							=		readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
		cl_int f_numLeaves								=		readFromFile<cl_int>("TestData//simple//numLeaves.bin");
		cl_int f_numLines									=		readFromFile<cl_int>("TestData//simple//numLines.bin");
		cl_int f_numPoints								=		readFromFile<cl_int>("TestData//simple//numPoints.bin");
		Resln f_resln											=		readFromFile<Resln>("TestData//simple//resln.bin");
		vector<QuadNode> f_quadtree				=		readFromFile<QuadNode>("TestData//simple//quadtree.bin",						f_numQuadNodes);
		vector<Leaf> f_leaves							=		readFromFile<Leaf>("TestData//simple//leaves.bin",							f_numLeaves);
		vector<cl_int> f_LCPToLine				=		readFromFile<cl_int>("TestData//simple//LCPToLine.bin",					f_numLines);
		vector<Pair> f_LCPBounds					=		readFromFile<Pair>("TestData//simple//LCPBounds.bin",						f_numQuadNodes);
		vector<Line> f_lines							=		readFromFile<Line>("TestData//simple//lines.bin",								f_numLines);
		vector<intn> f_qpoints						=		readFromFile<intn>("TestData//simple//qpoints.bin",							f_numPoints);
		vector<Conflict> f_conflicts			=		readFromFile<Conflict>("TestData//simple//sparseConflicts.bin",	f_numLeaves);

		When("we use this data to find conflict cells in series") {
			vector<Conflict> s_conflicts;
			FindConflictCells_s(f_quadtree, f_leaves, f_LCPToLine, f_LCPBounds, f_lines, f_qpoints, f_resln.width, false, s_conflicts);
			Then("the series results match the parallel results") {
					cl_int error = 0;
					vector<Conflict> p_conflicts(f_numLeaves);
					cl::Buffer b_quadtree, b_leaves, b_LCPToLine, b_LCPBounds, b_lines, b_qpoints, b_conflicts;
					error |= CLFW::getBuffer(b_quadtree, "b_quadtree", sizeof(QuadNode) * f_numQuadNodes);
					error |= CLFW::getBuffer(b_leaves, "b_leaves", sizeof(Leaf) * f_numLeaves);
					error |= CLFW::getBuffer(b_LCPToLine, "b_LCPToLine", sizeof(cl_int) * f_numLines);
					error |= CLFW::getBuffer(b_LCPBounds, "b_LCPBounds", sizeof(Pair) * f_numQuadNodes);
					error |= CLFW::getBuffer(b_lines, "b_lines", sizeof(Line) * f_numLines);
					error |= CLFW::getBuffer(b_qpoints, "b_qpoints", sizeof(intn) * f_numPoints);
					error |= CLFW::Upload(f_quadtree, b_quadtree);
					error |= CLFW::Upload(f_leaves, b_leaves);
					error |= CLFW::Upload(f_LCPToLine, b_LCPToLine);
					error |= CLFW::Upload(f_LCPBounds, b_LCPBounds);
					error |= CLFW::Upload(f_lines, b_lines);
					error |= CLFW::Upload(f_qpoints, b_qpoints);
					error |= FindConflictCells_p(b_quadtree, b_leaves, f_numLeaves, b_LCPToLine, b_LCPBounds, 
						b_lines, f_numLines, b_qpoints, f_resln.width, false, b_conflicts);
					error |= CLFW::Download(b_conflicts, f_numLeaves, p_conflicts);
					Require(error == 0);
					cl_int success = true;
					for (int i = 0; i < f_numLeaves; i++) {
						success &= compareConflict(&p_conflicts[i], &s_conflicts[i]);
					}
					Require(success == true);
				}
			Then("the results are valid") {
				cl_int success = true;
				for (int i = 0; i < f_numLeaves; ++i) {
					success &= compareConflict(&s_conflicts[i], &f_conflicts[i]);
					if (!success) {
						success &= compareConflict(&s_conflicts[i], &f_conflicts[i]);
						if (!success)
							success &= compareConflict(&s_conflicts[i], &f_conflicts[i]);
					}
				}
				Require(success == true);
			}
		}
	}
}
