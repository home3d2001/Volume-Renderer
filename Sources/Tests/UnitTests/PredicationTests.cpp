#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/Predication/Predication.hpp"

Scenario("Predicate by bit", "[predication]") {
	Given("N random integers") {
		vector<cl_int>small_input = generateDeterministicRandomIntegers(a_few);
		When("we predicate these integers in series") {
			vector<cl_int>small_output_s(a_few);
			PredicateByBit_s(small_input, 2, 1, small_output_s);
			Then("the predication for each number will be 1 only if the i'th bit matches \"compared\"") {
				int success = true;
				for (int i = 0; i < a_few; ++i)
					success &= ((small_input[i] & (1 << 2)) >> 2 == 1) == small_output_s[i];
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				vector<cl_int>small_output_p(a_few);
				cl::Buffer b_small_input, b_small_output;
				cl_int error = 0;
				error |= CLFW::getBuffer(b_small_input, "small", a_few * sizeof(cl_int));
				error |= CLFW::Upload<cl_int>(small_input, b_small_input);
				error |= PredicateByBit_p(b_small_input, 2, 1, a_few, "a", b_small_output);
				error |= CLFW::Download<cl_int>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= small_output_p[i] == small_output_s[i];
				Require(success == true);
			}
		}
	}
}
Scenario("Predicate big by bit", "[predication]") {
	Given("N random bigs") {
		vector<big>small_input = generateDeterministicRandomBigs(a_few);
		When("we predicate these integers in series") {
			vector<cl_int>small_output_s(a_few);
			PredicateBUByBit_s(small_input, 2, 1, small_output_s);
			Then("the predication for each number will be 1 only if the i'th bit matches \"compared\"") {
				int success = true;
				for (int i = 0; i < a_few; ++i)
					success &= (getBigBit(&small_input[i], 0, 2) == 1) == small_output_s[i];
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				vector<cl_int>small_output_p(a_few);
				vector<cl_int>large_output_p(a_lot);
				cl::Buffer b_small_input, b_small_output;
				cl_int error = 0;
				error |= CLFW::getBuffer(b_small_input, "small", a_few * sizeof(big));
				error |= CLFW::Upload<big>(small_input, b_small_input);
				error |= PredicateBigByBit_p(b_small_input, 2, 1, a_few, "a", b_small_output);
				error |= CLFW::Download<cl_int>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= small_output_p[i] == small_output_s[i];
				Require(success == true);
			}
		}
	}
}
Scenario("Predicate Conflict", "[conflict][predication]") {
	Given("N conflicts") {
		vector<Conflict>small_input = generateDeterministicRandomConflicts(a_few);
		When("we predicate these conflicts in series") {
			vector<cl_int>small_output_s(a_few);
			PredicateConflicts_s(small_input, small_output_s);
			Then("the predication for each number will be 1 only if the i'th bit matches \"compared\"") {
				int success = true;
				for (int i = 0; i < a_few; ++i)
					success &= (small_input[i].color == -2) == small_output_s[i];
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				vector<cl_int>small_output_p(a_few);
				cl::Buffer b_small_input, b_small_output;
				cl_int error = 0;
				error |= CLFW::getBuffer(b_small_input, "small", a_few * sizeof(Conflict));
				error |= CLFW::Upload<Conflict>(small_input, b_small_input);
				error |= PredicateConflicts_p(b_small_input, a_few, "a", b_small_output);
				error |= CLFW::Download<cl_int>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= small_output_p[i] == small_output_s[i];
				Require(success == true);
			}
		}
	}
}
