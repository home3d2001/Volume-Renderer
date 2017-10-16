#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/Scan/Scan.hpp"

Scenario("Inclusive Summation Scan", "[scan]") {
	Given("N random integers") {
		vector<cl_int>small_input = generateDeterministicRandomIntegers(a_few);
		vector<cl_int>large_input = generateDeterministicRandomIntegers(a_lot);
		When("we scan these integers in series") {
			vector<cl_int>small_output_s(a_few), large_output_s(a_lot);
			StreamScan_s(small_input, small_output_s);
			StreamScan_s(large_input, large_output_s);
			Then("the predication for each number will be 1 only if the i'th bit matches \"compared\"") {
				int success = true;
				Require(small_input[0] == small_output_s[0]);
				for (int i = 1; i < a_few; ++i)
					success &= (small_output_s[i] == small_output_s[i - 1] + small_input[i]);
				Require(large_input[0] == large_output_s[0]);
				for (int i = 1; i < a_lot; ++i)
					success &= (large_output_s[i] == large_output_s[i - 1] + large_input[i]);
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				vector<cl_int>small_output_p(a_few), large_output_p(a_lot);
				cl::Buffer b_small_input, b_large_input, b_small_output, b_large_output;
				cl_int error = 0;
				error |= CLFW::getBuffer(b_small_input, "smallin", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_large_input, "largein", a_lot * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_output, "smallout", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_large_output, "largeout", a_lot * sizeof(cl_int));
				error |= CLFW::Upload<cl_int>(small_input, b_small_input);
				error |= CLFW::Upload<cl_int>(large_input, b_large_input);
				error |= StreamScan_p(b_small_input, a_few, "a", b_small_output);
				error |= CLFW::DefaultQueue.finish();
				error |= StreamScan_p(b_large_input, a_lot, "b", b_large_output);
				error |= CLFW::DefaultQueue.finish();
				error |= CLFW::Download<cl_int>(b_small_output, a_few, small_output_p);
				error |= CLFW::Download<cl_int>(b_large_output, a_lot, large_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i)
					success &= small_output_p[i] == small_output_s[i];
				for (int i = 0; i < a_lot; ++i)
					success &= large_output_p[i] == large_output_s[i];
				Require(success == true);
			}
		}
	}
}
