#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/RadixSort/RadixSort.hpp"

using namespace std;

Scenario("Check Order", "[disabled][4way][sort]") {
	Given("a list containing 2^n ordered big") {
		TODO("run this check on n' elements, and then on 1 + n - n' elements");
		vector<big> small_input(CLFW::NextPow2(a_few));
		vector<big> large_input(CLFW::NextPow2(a_lot));
		for (int i = 0; i < small_input.size(); ++i)
			small_input[i] = makeBig(i);
		for (int i = 0; i < large_input.size(); ++i)
			large_input[i] = makeBig(i);
		When("we check to see if these numbers are in order in parallel") {
			cl_int error = 0;
			cl_int smallResult, largeResult;
			cl::Buffer b_small_input, b_large_input;
			error |= CLFW::getBuffer(b_small_input, "small_input", CLFW::NextPow2(a_few) * sizeof(big));
			error |= CLFW::getBuffer(b_large_input, "large_input", CLFW::NextPow2(a_lot) * sizeof(big));
			error |= CLFW::Upload<big>(small_input, b_small_input);
			error |= CLFW::Upload<big>(large_input, b_large_input);
			error |= CheckBigOrder_p(b_small_input, CLFW::NextPow2(a_few), smallResult);
			error |= CheckBigOrder_p(b_large_input, CLFW::NextPow2(a_lot), largeResult);
			Require(error == CL_SUCCESS);
			Then("we should get back true") {
				Require(smallResult == 0);
				Require(largeResult == 0);
			}
		}
		And_when("we modify the list so it isn't in order") {
			small_input[10] = makeBig(999);
			large_input[600] = makeBig(13);
			When("we check to see if these numbers are in order in parallel") {
				cl_int error = 0;
				cl_int smallResult, largeResult;
				cl::Buffer b_small_input, b_large_input;
				error |= CLFW::getBuffer(b_small_input, "small_input", CLFW::NextPow2(a_few) * sizeof(big));
				error |= CLFW::getBuffer(b_large_input, "large_input", CLFW::NextPow2(a_lot) * sizeof(big));
				error |= CLFW::Upload<big>(small_input, b_small_input);
				error |= CLFW::Upload<big>(large_input, b_large_input);
				TODO("make checkorder work for non-power of two elements");
				error |= CheckBigOrder_p(b_small_input, CLFW::NextPow2(a_few), smallResult);
				error |= CheckBigOrder_p(b_large_input, CLFW::NextPow2(a_lot), largeResult);
				Require(error == CL_SUCCESS);
				Then("we should get back the total number of out of order numbers") {
					Require(smallResult == 1);
					Require(largeResult == 1);
				}
			}
		}
	}
}

Scenario("Four Way Frequency Count (bigs)", "[sort][4way]") {
	Given("a block size and a set of big numbers evenly divisable by that block size ") {
		cl_int blockSize = 4;
		vector<big> in(20);
		in[0].blk[0] = 1; in[1].blk[0] = 2; in[2].blk[0] = 0; in[3].blk[0] = 3;
		in[4].blk[0] = 0; in[5].blk[0] = 1; in[6].blk[0] = 1; in[7].blk[0] = 0;
		in[8].blk[0] = 3; in[9].blk[0] = 3; in[10].blk[0] = 3; in[11].blk[0] = 2;
		in[12].blk[0] = 1; in[13].blk[0] = 2; in[14].blk[0] = 2; in[15].blk[0] = 0;
		in[16].blk[0] = 2; in[17].blk[0] = 0; in[18].blk[0] = 0; in[19].blk[0] = 2;

		writeToFile<big>(in, "TestData//4waysort//original.bin");
		When("we compute the 4 way frequency count in series") {
			vector<big> s_shuffle, result;
			vector<cl_int> localPrefix, s_blockSum, localShuffleAddr;
			BigFourWayPrefixSumWithShuffle_s(in, blockSize, 0, 0, s_shuffle, s_blockSum);
			Then("the results should be valid") {
				vector<big> f_shuffle = readFromFile<big>("TestData//4waysort//shuffle.bin", 20);
				vector<cl_int> f_blockSum = readFromFile<cl_int>("TestData//4waysort//blockSum.bin", 20);

				cl_int success = true;
				for (int i = 0; i < 20; ++i) {
					success &= (0 == compareBig(&f_shuffle[i], &s_shuffle[i]));
					success &= (f_blockSum[i] == s_blockSum[i]);
				}

				And_then("the series results should match the parallel results") {
					cl_int error = 0;
					vector<big> p_shuffle;
					vector<cl_int> p_blockSum;
					cl::Buffer b_in, b_blkSum, b_shuffle;
					error |= CLFW::getBuffer(b_in, "in", 20 * sizeof(big));
					error |= CLFW::Upload<big>(in, b_in);
					error |= BigFourWayPrefixSumAndShuffle_p(b_in, 20, blockSize, 0, 0, b_blkSum, b_shuffle);
					error |= CLFW::Download<big>(b_shuffle, 20, p_shuffle);
					error |= CLFW::Download<cl_int>(b_blkSum, 20, p_blockSum);
					Require(error == CL_SUCCESS);

					success = true;
					for (cl_int i = 0; i < 20; ++i) {
						success &= (compareBig(&p_shuffle[i], &s_shuffle[i]) == 0);
						success &= (p_blockSum[i] == s_blockSum[i]);
					}
					Require(success == true);
				}
			}
		}
	}
}
Scenario("Move Four Way Shuffled Elements (bigs)", "[sort][4way]")
{
	Given("the shuffled elements, block sums, and prefix block sums produced by the 4 way frequency count") {
		vector<big> f_shuffle = readFromFile<big>("TestData//4waysort//shuffle.bin", 20);
		vector<cl_int> f_blockSum = readFromFile<cl_int>("TestData//4waysort//blockSum.bin", 20);
		vector<cl_int> f_prefixBlockSum = readFromFile<cl_int>("TestData//4waysort//prefixBlockSum.bin", 20);
		When("we use the block sum and prefix block sum to move these shuffled elements in series") {
			vector<big> s_result;
			MoveBigElements_s(f_shuffle, f_blockSum, f_prefixBlockSum, 4, 0, 0, s_result);
			Then("the results should be valid") {
				vector<big> f_result = readFromFile<big>("TestData//4waysort//result.bin", 20);
				cl_int success = true;
				for (cl_int i = 0; i < 20; ++i) {
					success &= (compareBig(&s_result[i], &f_result[i]));
				}
				And_then("the series results should match the parallel results") {
					cl_int error = 0;
					vector<big> p_result;
					vector<cl_int> p_blockSum;
					cl::Buffer b_shuffle, b_blkSum, b_prefixBlkSum, b_result;
					error |= CLFW::getBuffer(b_shuffle, "shuffle", 20 * sizeof(big));
					error |= CLFW::getBuffer(b_blkSum, "blkSum", 20 * sizeof(cl_int));
					error |= CLFW::getBuffer(b_prefixBlkSum, "prefixBlkSum", 20 * sizeof(cl_int));
					error |= CLFW::Upload<big>(f_shuffle, b_shuffle);
					error |= CLFW::Upload<cl_int>(f_blockSum, b_blkSum);
					error |= CLFW::Upload<cl_int>(f_prefixBlockSum, b_prefixBlkSum);
					error |= MoveBigElements_p(b_shuffle, 20, b_blkSum, b_prefixBlkSum, 4, 0, 0, b_result);
					error |= CLFW::Download<big>(b_result, 20, p_result);
					Require(error == CL_SUCCESS);

					success = true;
					for (cl_int i = 0; i < 20; ++i) {
						success &= (compareBig(&p_result[i], &s_result[i]) == 0);
					}
					Require(success == true);
				}
			}
		}
	}
}
Scenario("Four Way Radix Sort (bigs)", "[sort][4way]") {
	Given("an unsorted set of big") {
		cl_int numElements = a_lot;
		vector<big> input(numElements);
		for (int i = 0; i < numElements; ++i) {
			input[i] = { (cl_ulong)(numElements - i), 0 };
		}
		When("we sort that data using the parallel 4 way radix sorter") {
			vector<big> result;
			vector<big> result2;
			cl_int error = 0;
			cl::Buffer b_input, b_other;
			error |= CLFW::getBuffer(b_input, "input", numElements * sizeof(big));
			error |= CLFW::Upload<big>(input, b_input);
			error |= RadixSortBig_p(b_input, numElements, 48, "");
			error |= CLFW::Download<big>(b_input, numElements, result);

			Require(error == CL_SUCCESS);

			Then("the results should be valid") {
				cl_int success = true;
				for (int i = 0; i < a_lot; ++i) {
					big temp = { i + 1, 0 };
					success &= (compareBig(&result[i], &temp) == 0);
				}
				Require(success == true);
			}
		}
	}
}
Scenario("Four Way Radix Sort (<big, cl_int> by Key)", "[sort][4way]") {
	Given("An arbitrary set of unsigned key and integer value pairs") {
		vector<big> keys(a_lot);
		vector<cl_int> values(a_lot);
		for (int i = 0; i < a_lot; ++i) {
			keys[i] = makeBig(a_lot - i);
			values[i] = a_lot - i;
		}

		When("these pairs are sorted by key in parallel") {
			cl_int error = 0;
			cl::Buffer b_keys, b_values;
			error |= CLFW::getBuffer(b_keys, "b_keys", a_lot * sizeof(big));
			error |= CLFW::getBuffer(b_values, "b_values", a_lot * sizeof(cl_int));
			error |= CLFW::Upload<big>(keys, b_keys);
			error |= CLFW::Upload<cl_int>(values, b_values);
			Require(error == CL_SUCCESS);
			error |= RadixSortBigToInt_p(b_keys, b_values, a_lot, 20, "");
			Require(error == CL_SUCCESS);
			Then("The key value pairs are ordered by keys assending") {
				vector<big> keys_out_p(a_lot);
				vector<cl_int> values_out_p(a_lot);
				error |= CLFW::Download<big>(b_keys, a_lot, keys_out_p);
				error |= CLFW::Download<cl_int>(b_values, a_lot, values_out_p);
				int success = true;
				for (int i = 0; i < a_lot; ++i) {
					success &= (values_out_p[i] && values_out_p[i] == i + 1);
					big temp = makeBig(i + 1);
					success &= (compareBig(&keys_out_p[i], &temp) == 0);
				}
				Require(success == true);
			}
		}
	}
}
Scenario("Four Way Radix Sort (<cl_int, cl_int> by Key)", "[sort][4way]") {
	Given("An arbitrary set of unsigned key and integer value pairs") {
		vector<cl_int> keys(a_lot);
		vector<cl_int> values(a_lot);
		for (int i = 0; i < a_lot; ++i) {
			keys[i] = a_lot - i;
			values[i] = a_lot - i;
		}

		When("these pairs are sorted by key in parallel") {
			cl_int error = 0;
			cl::Buffer b_keys, b_values;
			error |= CLFW::getBuffer(b_keys, "b_keys", a_lot * sizeof(cl_int));
			error |= CLFW::getBuffer(b_values, "b_values", a_lot * sizeof(cl_int));
			error |= CLFW::Upload<cl_int>(keys, b_keys);
			error |= CLFW::Upload<cl_int>(values, b_values);
			Require(error == CL_SUCCESS);
			error |= RadixSortIntToInt_p(b_keys, b_values, a_lot, 20, "");
			Require(error == CL_SUCCESS);
			Then("The key value pairs are ordered by keys assending") {
				vector<cl_int> keys_out_p(a_lot);
				vector<cl_int> values_out_p(a_lot);
				error |= CLFW::Download<cl_int>(b_keys, a_lot, keys_out_p);
				error |= CLFW::Download<cl_int>(b_values, a_lot, values_out_p);
				int success = true;
				for (int i = 0; i < a_lot; ++i) {
					success &= (values_out_p[i] && values_out_p[i] == i + 1);
					success &= (keys_out_p[i] && keys_out_p[i] == i + 1);
				}
				Require(success == true);
			}
		}
	}
}
//Scenario("Parallel Radix Sort", "[1][sort][integration][failing][disabled]") {
//	Given("An arbitrary set of numbers") {
//		vector<cl_ulong> small_input(a_few);
//		vector<cl_ulong> large_input(a_lot);
//
//		for (cl_ulong i = 0; i < a_few; ++i) small_input[i] = a_few - i;
//		for (cl_ulong i = 0; i < a_lot; ++i) large_input[i] = a_lot - i;
//
//		When("these numbers are sorted in parallel") {
//			cl_int error = 0;
//			cl::Buffer b_small_input, b_large_input;
//			error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(cl_ulong));
//			error |= CLFW::getBuffer(b_large_input, "b_large_input", a_lot * sizeof(cl_ulong));
//			error |= CLFW::Upload<cl_ulong>(small_input, b_small_input);
//			error |= CLFW::Upload<cl_ulong>(large_input, b_large_input);
//			error |= OldRadixSort_p(b_small_input, a_few, 20);
//			error |= OldRadixSort_p(b_large_input, a_lot, 20);
//			Require(error == CL_SUCCESS);
//			Then("the numbers are ordered assending") {
//				vector<cl_ulong> small_output_p(a_few), large_output_p(a_lot);
//				error |= CLFW::Download<cl_ulong>(b_small_input, a_few, small_output_p);
//				error |= CLFW::Download<cl_ulong>(b_large_input, a_lot, large_output_p);
//				int success = true;
//				for (cl_ulong i = 0; i < a_few; ++i) {
//					big temp = makeBig(i + 1);
//					success &= (small_output_p[i] == i + 1);
//				}
//				Require(success == true);
//				for (cl_ulong i = 0; i < a_lot; ++i) {
//					success &= (large_output_p[i] == i + 1);
//				}
//				Require(success == true);
//			}
//		}
//	}
//}
//Scenario("Parallel Radix Sort (Pairs by Key)", "[2][sort][integration]") {
//	Given("An arbitrary set of unsigned key and integer value pairs") {
//		vector<cl_int> small_keys_in(a_few);
//		vector<cl_int> small_values_in(a_few);
//		vector<cl_int> large_keys_in(a_lot);
//		vector<cl_int> large_values_in(a_lot);
//
//		for (int i = 0; i < a_few; ++i) small_keys_in[i] = small_values_in[i] = a_few - i;
//		for (int i = 0; i < a_lot; ++i) large_keys_in[i] = large_values_in[i] = a_lot - i;
//
//		When("these pairs are sorted by key in parallel") {
//			cl_int error = 0;
//			cl::Buffer b_small_keys, b_small_values, b_large_keys, b_large_values;
//			error |= CLFW::getBuffer(b_small_keys, "b_small_keys", a_few * sizeof(cl_int));
//			error |= CLFW::getBuffer(b_small_values, "b_small_values", a_few * sizeof(cl_int));
//			error |= CLFW::getBuffer(b_large_keys, "b_large_keys", a_lot * sizeof(cl_int));
//			error |= CLFW::getBuffer(b_large_values, "b_large_values", a_lot * sizeof(cl_int));
//			error |= CLFW::Upload<cl_int>(small_keys_in, b_small_keys);
//			error |= CLFW::Upload<cl_int>(small_values_in, b_small_values);
//			error |= CLFW::Upload<cl_int>(large_keys_in, b_large_keys);
//			error |= CLFW::Upload<cl_int>(large_values_in, b_large_values);
//			Require(error == CL_SUCCESS);
//			error |= OldRadixSortPairsByKey(b_small_keys, b_small_values, a_few);
//			Require(error == CL_SUCCESS);
//			error |= OldRadixSortPairsByKey(b_large_keys, b_large_values, a_lot);
//			Require(error == CL_SUCCESS);
//			Then("The key value pairs are ordered by keys assending") {
//				vector<cl_int> small_keys_out_p(a_few), small_values_out_p(a_few);
//				vector<cl_int> large_keys_out_p(a_lot), large_values_out_p(a_lot);
//				error |= CLFW::Download<cl_int>(b_small_keys, a_few, small_keys_out_p);
//				error |= CLFW::Download<cl_int>(b_small_values, a_few, small_values_out_p);
//				error |= CLFW::Download<cl_int>(b_large_keys, a_lot, large_keys_out_p);
//				error |= CLFW::Download<cl_int>(b_large_values, a_lot, large_values_out_p);
//				int success = true;
//				for (int i = 0; i < a_few; ++i)
//					success &= (small_keys_out_p[i] == small_values_out_p[i] && small_values_out_p[i] == i + 1);
//				Require(success == true);
//				for (int i = 0; i < a_lot; ++i)
//					success &= (large_keys_out_p[i] == large_values_out_p[i] && large_values_out_p[i] == i + 1);
//				Require(success == true);
//			}
//		}
//	}
//}

Scenario("Unique Sorted big", "[sort][unique]") {
	Given("An ascending sorted set of bigs") {
		vector<big> small_zpoints = readFromFile<big>("TestData//few_non-unique_s_zpoints.bin", a_few);
		When("those bigs are uniqued in parallel") {
			cl_int error = 0, newSmallSize, newLargeSize;
			cl::Buffer b_small_zpoints, b_unique_small_zpoints;
			error |= CLFW::getBuffer(b_small_zpoints, "b_small_zpoints", a_few * sizeof(big));
			error |= CLFW::Upload<big>(small_zpoints, b_small_zpoints);
			error |= UniqueSorted(b_small_zpoints, a_few, "a", newSmallSize);
			vector<big> p_small_zpoints(newSmallSize);
			error |= CLFW::Download<big>(b_small_zpoints, newSmallSize, p_small_zpoints);
			Then("the resulting set should match the uniqued series set.") {
				auto sm_last = unique(small_zpoints.begin(), small_zpoints.end(), weakEqualsBig);
				small_zpoints.erase(sm_last, small_zpoints.end());
				Require(small_zpoints.size() == newSmallSize);
				cl_int success = 1;
				for (int i = 0; i < small_zpoints.size(); ++i)
					success &= (compareBig(&small_zpoints[i], &p_small_zpoints[i]) == 0);
				Require(success == true);
			}
		}
	}
}
Scenario("Unique Sorted big color pairs", "[sort][unique]") {
	Given("An ascending sorted set of bigs") {
		vector<big> small_keys(a_few);
		vector<cl_int> small_values(a_few);

		for (int i = 0; i < a_few; ++i) {
			small_values[i] = i / 2;
			small_keys[i] = makeBig(i / 2);
		}

		When("those bigs are uniqued in parallel") {
			cl_int error = 0, newSmallSize, newLargeSize;
			cl::Buffer b_small_keys, b_unique_small_keys;
			cl::Buffer b_small_values, b_unique_small_values;
			error |= CLFW::getBuffer(b_small_keys, "b_small_zpoints", a_few * sizeof(big));
			error |= CLFW::getBuffer(b_small_values, "b_small_values", a_few * sizeof(cl_int));
			error |= CLFW::Upload<big>(small_keys, b_small_keys);
			error |= CLFW::Upload<cl_int>(small_values, b_small_values);
			error |= UniqueSortedBUIntPair(b_small_keys, b_small_values, a_few, "a", newSmallSize);
			vector<big> p_small_zpoints(newSmallSize);
			error |= CLFW::Download<big>(b_small_keys, newSmallSize, p_small_zpoints);
			Then("the resulting set should match the uniqued series set.") {
				auto sm_last = unique(small_keys.begin(), small_keys.end(), weakEqualsBig);
				small_keys.erase(sm_last, small_keys.end());
				Require(small_keys.size() == newSmallSize);
				cl_int success = 1;
				for (int i = 0; i < small_keys.size(); ++i)
					success &= (compareBig(&small_keys[i], &p_small_zpoints[i]) == 0);
				Require(success == true);
			}
		}
	}
}
