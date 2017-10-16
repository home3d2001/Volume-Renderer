#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/Reduction/Reduction.hpp"

Scenario("Additive Reduction", "[reduction]") {
	Given("N random integers") {
		vector<cl_int>small_input = generateDeterministicRandomIntegers(a_few);
		vector<cl_int>large_input(a_lot, 1);
		When("we reduce these numbers in series") {
			int small_output_s, large_output_s;
			Reduce_s(small_input, small_output_s);
			Reduce_s(large_input, large_output_s);
			Then("we get the summation of those integers") {
				cl_int small_actual = 0;
				cl_int large_actual = 0;
				for (int i = 0; i < a_few; ++i)
					small_actual += small_input[i];
				for (int i = 0; i < a_lot; ++i)
					large_actual += large_input[i];
				Require(small_output_s == small_actual);
				Require(large_output_s == large_actual);
			}
			Then("the series results match the parallel results") {
				cl_int small_output_p, large_output_p;
				cl::Buffer b_small_input, b_large_input, b_small_output, b_large_output;
				cl_int error = 0;
				error |= CLFW::getBuffer(b_small_input, "small", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_large_input, "large", a_lot * sizeof(cl_int));
				error |= CLFW::Upload<cl_int>(small_input, b_small_input);
				error |= CLFW::Upload<cl_int>(large_input, b_large_input);
				Require(error == CL_SUCCESS);
				error |= Reduce_p(b_small_input, a_few, "a", b_small_output);
				error |= CLFW::Download<cl_int>(b_small_output, 0, small_output_p);
				error |= Reduce_p(b_large_input, a_lot, "b", b_large_output);
				error |= CLFW::Download<cl_int>(b_large_output, 0, large_output_p);
				Require(small_output_p == small_output_s);
				Require(large_output_p == large_output_s);
			}
		}
	}
}