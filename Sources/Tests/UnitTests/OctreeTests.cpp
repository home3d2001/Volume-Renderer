#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "Octree/BuildOctree.hpp"

Scenario("Build Quadtree", "[1][quadtree]") {
	Given("a binary radix tree") {
		cl_int numPts = readFromFile<cl_int>("TestData//simple//uNumPts.bin");
		auto brt = readFromFile<BrtNode>("TestData//simple//brt.bin", numPts - 1);
		When("we use that binary radix tree to build a quadtree in series") {
			std::vector<QuadNode> quadtree_s;
			BinaryRadixToQuadtree_s(brt, false, {}, quadtree_s);

			Then("our results should be valid") {
				cl_int numQuadNodes = readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
				std::vector<QuadNode> quadtree_f = readFromFile<QuadNode>("TestData//simple//quadtree.bin", numQuadNodes);
				Require(quadtree_s.size() == quadtree_f.size());
				int success = true;
				for (int i = 0; i < quadtree_s.size(); ++i) {
					success &= (compareQuadNode(&quadtree_s[i], &quadtree_f[i]));
				}

				And_then("our series results should match the parallel results") {
					cl_int error = 0, quadtree_size;
					cl::Buffer b_brt, b_quadtree, nullBuffer;
					error |= CLFW::getBuffer(b_brt, "brt", (numPts - 1) * sizeof(BrtNode));
					error |= CLFW::Upload<BrtNode>(brt, b_brt);
					error |= BinaryRadixToQuadtree_p(b_brt, false, nullBuffer, numPts, 
						"", b_quadtree, quadtree_size);
					vector<QuadNode> quadtree_p(quadtree_size);
					error |= CLFW::Download<QuadNode>(b_quadtree, quadtree_size, quadtree_p);

					Require(quadtree_size == quadtree_s.size());
					int success = true;
					for (int i = 0; i < quadtree_s.size(); ++i) {
						success &= (compareQuadNode(&quadtree_s[i], &quadtree_p[i]));
					}
				}
			}
		}
	}
}

Scenario("Generate Quadtree Leaves", "[2][quadtree]") {
	Given("a QuadNode") {
		QuadNode n;
		n.children[0] = -1;
		n.children[1] = 1;
		n.children[2] = -1;
		n.children[3] = 0;
		When("we generate the leaves for that quadnode") {
			vector<Leaf> leaves(4);
			vector<cl_int> predicates(4);
			for (int i = 0; i < 4; ++i)
				ComputeQuadLeaves(&n, leaves.data(), predicates.data(), 1, i);
			Then("the results should be valid") {
				Require(leaves[0].parent == leaves[2].parent);
				Require(leaves[2].parent == 0);
				Require(leaves[0].quadrant == 0);
				Require(leaves[2].quadrant == 2);
				Require(predicates[0] == 1);
				Require(predicates[1] == 0);
				Require(predicates[2] == 1);
				Require(predicates[3] == 0);
			}
		}
	}
	Given("a quadtree") {
		cl_int numQuadNodes = readFromFile<cl_int>("TestData//simple//numQuadNodes.bin");
		auto quadtree = readFromFile<QuadNode>("TestData//simple//quadtree.bin", numQuadNodes);

		When("we generate the leaves of this octree in series") {
			vector<cl_int> pred_s(4 * numQuadNodes);
			vector<Leaf> leaves_s(4 * numQuadNodes);
			GenerateQuadLeaves_s(quadtree, numQuadNodes, leaves_s, pred_s);
			Then("the parallel results match the serial ones") {
				cl_int error = 0;
				vector<cl_int> pred_p(4* numQuadNodes);
				vector<Leaf> leaves_p(4* numQuadNodes);
				cl::Buffer b_quadtree, b_pred, b_leaves;
				error |= CLFW::getBuffer(b_quadtree, "b_small_octree", 4 * numQuadNodes * sizeof(QuadNode));
				error |= CLFW::Upload<QuadNode>(quadtree, b_quadtree);
				error |= GenerateQuadLeaves_p(b_quadtree, numQuadNodes, b_leaves, b_pred);
				error |= CLFW::Download(b_pred, 4 * numQuadNodes, pred_p);
				error |= CLFW::Download(b_leaves, 4 * numQuadNodes, leaves_p);
				Require(error == CL_SUCCESS);
				cl_int success = true;
				for (int i = 0; i < 4 * 11; ++i) {
					success &= (compareLeaf(&leaves_s[i], &leaves_p[i]));
					success &= (pred_s[i] == pred_p[i]);
				}
				Require(success == true);
			}
		}
	}
}

Scenario("Build Octree", "[1][octree][disabled]") {
	Given("a binary radix tree") {
		cl_int numPts = readFromFile<cl_int>("TestData//simple3D//uNumPts.bin");
		auto brt = readFromFile<BrtNode>("TestData//simple3D//brt.bin", numPts - 1);
		When("we use that binary radix tree to build an octree in series") {
			std::vector<OctNode> octree_s;
			BinaryRadixToOctree_s(brt, false, {}, octree_s);

			Then("our results should be valid") {
				cl_int numOctNodes = readFromFile<cl_int>("TestData//simple3D//numOctNodes.bin");
				std::vector<OctNode> octree_f = readFromFile<OctNode>("TestData//simple3D//octree.bin", numOctNodes);
				Require(octree_s.size() == octree_f.size());
				int success = true;
				for (int i = 0; i < octree_s.size(); ++i) {
					success &= (compareOctNode(&octree_s[i], &octree_f[i]));
				}
				Require(success == true);

				And_then("our series results should match the parallel results") {
					cl_int error = 0, octree_size;
					cl::Buffer b_brt, b_octree, nullBuffer;
					error |= CLFW::getBuffer(b_brt, "brt", (numPts - 1) * sizeof(BrtNode));
					error |= CLFW::Upload<BrtNode>(brt, b_brt);
					error |= BinaryRadixToOctree_p(b_brt, false, nullBuffer, numPts,
						"", b_octree, octree_size);
					vector<OctNode> octree_p(octree_size);
					error |= CLFW::Download<OctNode>(b_octree, octree_size, octree_p);

					Require(octree_size == octree_s.size());
					int success = true;
					for (int i = 0; i < octree_s.size(); ++i) {
						success &= (compareOctNode(&octree_s[i], &octree_p[i]));
					}
				}
			}
		}
	}
}

Scenario("Generate Octree Leaves", "[2][octree]") {
	Given("an Octnode") {
		OctNode n;
		n.children[0] = -1;
		n.children[1] = 1;
		n.children[2] = -1;
		n.children[3] = 0;
		n.children[4] = -1;
		n.children[5] = 2;
		n.children[6] = -1;
		n.children[7] = 0;
		When("we generate the leaves for that octnode") {
			vector<Leaf> leaves(8);
			vector<cl_int> predicates(8);
			for (int i = 0; i < 8; ++i)
				ComputeOctLeaves(&n, leaves.data(), predicates.data(), 1, i);
			Then("the results should be valid") {
				Require(leaves[0].parent == leaves[2].parent);
				Require(leaves[2].parent == 0);
				Require(leaves[0].quadrant == 0);
				Require(leaves[2].quadrant == 2);
				Require(predicates[0] == 1);
				Require(predicates[1] == 0);
				Require(predicates[2] == 1);
				Require(predicates[3] == 0);
				Require(predicates[4] == 1);
				Require(predicates[5] == 0);
				Require(predicates[6] == 1);
				Require(predicates[7] == 0);
			}
		}
	}
	Given("a octree") {
		cl_int numOctNodes = readFromFile<cl_int>("TestData//simple3D//numOctNodes.bin");
		auto octree = readFromFile<OctNode>("TestData//simple3D//octree.bin", numOctNodes);

		When("we generate the leaves of this octree in series") {
			vector<cl_int> pred_s(8 * numOctNodes);
			vector<Leaf> leaves_s(8 * numOctNodes);
			GenerateOctLeaves_s(octree, numOctNodes, leaves_s, pred_s);
			Then("the parallel results match the serial ones") {
				cl_int error = 0;
				vector<cl_int> pred_p(8 * numOctNodes);
				vector<Leaf> leaves_p(8 * numOctNodes);
				cl::Buffer b_octree, b_pred, b_leaves;
				error |= CLFW::getBuffer(b_octree, "b_octree", 8 * numOctNodes * sizeof(OctNode));
				error |= CLFW::Upload<OctNode>(octree, b_octree);
				error |= GenerateOctLeaves_p(b_octree, numOctNodes, b_leaves, b_pred);
				error |= CLFW::Download(b_pred, 8 * numOctNodes, pred_p);
				error |= CLFW::Download(b_leaves, 8 * numOctNodes, leaves_p);
				Require(error == CL_SUCCESS);
				cl_int success = true;
				for (int i = 0; i < 8 * numOctNodes; ++i) {
					success &= (compareLeaf(&leaves_s[i], &leaves_p[i]));
					success &= (pred_s[i] == pred_p[i]);
				}
				Require(success == true);
			}
		}
	}
}