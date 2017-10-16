#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"

Scenario("Build Binary Radix Tree", "[brt]") {
	Given("A set of unique ordered zpoints") {
		cl_int numPts = readFromFile<cl_int>("TestData//simple//uNumPts.bin");
		vector<big> zpnts = readFromFile<big>("TestData//simple//usZPts.bin", numPts);
		Resln resln = readFromFile<Resln>("TestData//simple//resln.bin");

		When("we build a binary radix tree using these points in series") {
			vector<BrtNode> s_brt;
			BuildBinaryRadixTree_s(zpnts, resln.mbits, s_brt);

			Then("the results should be valid") {
				vector<BrtNode> f_brt = readFromFile<BrtNode>("TestData//simple//brt.bin", numPts - 1);
				cl_int success = 1;
				for (int i = 0; i < numPts - 1; ++i) {
					success &= (true == compareBrtNode(&s_brt[i], &f_brt[i]));
				}
				Require(success == true);
			}

			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<BrtNode> p_brt;
				cl::Buffer b_zpoints, b_brt;
				error |= CLFW::getBuffer(b_zpoints, "b_zpoints", numPts * sizeof(big));
				error |= CLFW::Upload<big>(zpnts, b_zpoints);
				error |= BuildBinaryRadixTree_p(b_zpoints, numPts, resln.mbits, "a", b_brt);
				error |= CLFW::Download<BrtNode>(b_brt, numPts - 1, p_brt);
				Require(error == CL_SUCCESS);
				Then("the resulting binary radix tree should be valid") {
					/* Precomputed */
					int success = true;
					for (int i = 0; i < numPts - 1; ++i) {
						success &= (true == compareBrtNode(&s_brt[i], &p_brt[i]));
					}
					Require(success == true);
				}
			}
		}
	}
}

Scenario("Build Colored Binary Radix Tree", "[brt]") {
	Given("A set of colored unique ordered zpoints") {
		cl_int numPts = readFromFile<cl_int>("TestData//simple//uNumPts.bin");
		vector<big> zpnts = readFromFile<big>("TestData//simple//usZPts.bin", numPts);
		Resln resln = readFromFile<Resln>("TestData//simple//resln.bin");
		vector<cl_int> leafColors = readFromFile<cl_int>("TestData//simple//uniqueColors.bin", numPts);
		
		When("we build a colored binary radix tree using these points in series") {
			vector<BrtNode> brt_s;
			vector<cl_int> brtColors_s;
			BuildColoredBinaryRadixTree_s(zpnts, leafColors, resln.mbits, brt_s, brtColors_s);
			Then("the resulting binary radix tree and cooresponding colors should be valid") {
				vector<BrtNode> brt_f = readFromFile<BrtNode>("TestData//simple//brt.bin", numPts - 1);
				vector<cl_int> brtColors_f = readFromFile<cl_int>("TestData//simple//unpropagatedBrtColors.bin", numPts - 1);
				cl_int success = true;
				for (int i = 0; i < brt_s.size(); ++i) {
					success &= (brtColors_s[i] == brtColors_f[i]);
					success &= (compareBrtNode(&brt_s[i], &brt_f[i]));

					if (brt_s[i].parent != -1) {
						BrtNode p = brt_s[brt_s[i].parent];
						assert(p.left == i || p.left + 1 == i);
					}
				}
				Require(success == true);
			}
			Then("the series results should match the parallel results") {
				cl_int error = 0;
				vector<BrtNode> brt_p;
				vector<cl_int> brtColors_p;
				cl::Buffer b_zpoints, b_leafColors, b_brt, b_brtColors;
				error |= CLFW::getBuffer(b_zpoints, "b_zpoints", numPts * sizeof(big));
				error |= CLFW::getBuffer(b_leafColors, "b_leafColors", numPts * sizeof(cl_int));
				error |= CLFW::Upload<big>(zpnts, b_zpoints);
				error |= CLFW::Upload<cl_int>(leafColors, b_leafColors);
				error |= BuildColoredBinaryRadixTree_p(b_zpoints, b_leafColors, numPts, resln.mbits, "", b_brt, b_brtColors);
				error |= CLFW::Download<BrtNode>(b_brt, numPts - 1, brt_p);
				error |= CLFW::Download<cl_int>(b_brtColors, numPts - 1, brtColors_p);
				Require(error == CL_SUCCESS);
				cl_int success = true;
				for (int i = 0; i < brt_s.size(); ++i) {
					success &= (brtColors_s[i] == brtColors_p[i]);
					success &= (compareBrtNode(&brt_s[i], &brt_p[i]));
				}
				Require(success == true);
			}
		}
	}
}

Scenario("Propagate Brt Colors", "[brt]") {
	Given("a colored binary radix tree") {
		cl_int totalPoints = readFromFile<cl_int>("TestData//simple//numPoints.bin");
		vector<BrtNode> brt = readFromFile<BrtNode>("TestData//simple//brt.bin", totalPoints - 1);
		vector<cl_int> brtColors_s = readFromFile<cl_int>("TestData//simple//unpropagatedBrtColors.bin", totalPoints - 1);
		vector<cl_int> brtColors_f = readFromFile<cl_int>("TestData//simple//unpropagatedBrtColors.bin", totalPoints - 1);

		When("we propagate the BRT colors up the tree in series") {
			PropagateBRTColors_s(brt, brtColors_s);
			Then("the results should be valid") {
				vector<cl_int> brtColors_f = readFromFile<cl_int>("TestData//simple//brtColors.bin", totalPoints - 1);

				cl_int success = true;
				for (cl_int i = 0; i < totalPoints - 1; i++) {
					success &= brtColors_s[i] == brtColors_f[i];
				}
				Require(success == true);
			}
			Then("the series results should match the parallel results") {
				cl_int error = 0;
				cl::Buffer b_brt, b_brtColors;
				vector<cl_int> brtColors_p;
				error |= CLFW::getBuffer(b_brt, "brt", (totalPoints - 1) * sizeof(BrtNode));
				error |= CLFW::getBuffer(b_brtColors, "b_brtColors", (totalPoints - 1) * sizeof(cl_int));
				error |= CLFW::Upload<BrtNode>(brt, b_brt);
				error |= CLFW::Upload<cl_int>(brtColors_f, b_brtColors);
				error |= PropagateBRTColors_p(b_brt, b_brtColors, totalPoints - 1, "");
				error |= CLFW::Download<cl_int>(b_brtColors, totalPoints - 1, brtColors_p);

				Require(error == CL_SUCCESS);

				cl_int success = true;
				for (cl_int i = 0; i < totalPoints - 1; i++) {
					success &= brtColors_p[i] == brtColors_s[i];
				}
				Require(success == true);
			}
		}
	}
}