#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ParallelAlgorithms/Compaction/Compaction.hpp"

Scenario("Integer Compaction", "[compaction]") {
	Given("N random integers, an arbitrary predication, and an inclusive prefix sum of that predication") {
		vector<cl_int>small_input = generateDeterministicRandomIntegers(a_few);
		vector<cl_int> small_pred(a_few), small_addr(a_few), small_output_s(a_few);
		/* In this example, odd indexes are compacted to the left. */
		for (int i = 0; i < a_few; ++i) { small_pred[i] = i % 2; small_addr[i] = (i + 1) / 2; }
		When("these integers are compacted in series") {
			Compact_s(small_input, small_pred, small_addr, small_output_s);
			Then("elements predicated true are moved to their cooresponding addresses.") {
				int success = true;
				for (int i = 0; i < a_few / 2; ++i) success &= (small_output_s[i] == small_input[(i * 2) + 1]);
				Require(success == true);
			}
			Then("elements predicated false are placed after the last truely predicated element and in their original order.") {
				int success = true;
				for (int i = a_few / 2; i < a_few; ++i) success &= (small_output_s[i] == small_input[(i - (a_few / 2)) * 2]);
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<cl_int> small_output_p(a_few);
				cl::Buffer b_small_input, b_small_pred, b_small_addr, b_small_output;
				error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_pred, "b_small_pred", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_addr, "b_small_addr", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_output, "b_small_output", a_few * sizeof(cl_int));
				error |= CLFW::Upload<cl_int>(small_input, b_small_input);
				error |= CLFW::Upload<cl_int>(small_pred, b_small_pred);
				error |= CLFW::Upload<cl_int>(small_addr, b_small_addr);
				error |= Compact_p(b_small_input, b_small_pred, b_small_addr, a_few, b_small_output);
				error |= CLFW::Download<cl_int>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= (small_output_p[i] == small_output_s[i]);
				Require(success == true);
			}
		}
	}
}
Scenario("Big Unsigned Compaction", "[compaction]") {
	Given("N random bigs, an arbitrary predication, and an inclusive prefix sum of that predication") {
		vector<big>small_input = generateDeterministicRandomBigs(a_few);
		vector<cl_int> small_pred(a_few), small_addr(a_few);
		vector<big> small_output_s(a_few);
		/* In this example, odd indexes are compacted to the left. */
		for (int i = 0; i < a_few; ++i) { small_pred[i] = i % 2; small_addr[i] = (i + 1) / 2; }
		When("these integers are compacted in series") {
			BigCompact_s(small_input, small_pred, small_addr, small_output_s);
			Then("elements predicated true are moved to their cooresponding addresses.") {
				int success = true;
				for (int i = 0; i < a_few / 2; ++i) success &= (compareBig(&small_output_s[i], &small_input[(i * 2) + 1]) == 0);
				Require(success == true);
			}
			Then("elements predicated false are placed after the last truely predicated element and in their original order.") {
				int success = true;
				for (int i = a_few / 2; i < a_few; ++i) success &= (compareBig(&small_output_s[i], &small_input[(i - (a_few / 2)) * 2]) == 0);
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<big> small_output_p(a_few);
				cl::Buffer b_small_input, b_small_pred, b_small_addr, b_small_output;
				error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(big));
				error |= CLFW::getBuffer(b_small_pred, "b_small_pred", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_addr, "b_small_addr", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_output, "b_small_output", a_few * sizeof(big));
				error |= CLFW::Upload<big>(small_input, b_small_input);
				error |= CLFW::Upload<cl_int>(small_pred, b_small_pred);
				error |= CLFW::Upload<cl_int>(small_addr, b_small_addr);
				error |= BigCompact_p(b_small_input, a_few, b_small_pred, b_small_addr, b_small_output);
				error |= CLFW::Download<big>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= (compareBig(&small_output_p[i], &small_output_s[i]) == 0);
				Require(success == true);
			}
		}
	}
}
Scenario("Conflict Compaction", "[conflict][compaction]") {
	Given("N random conflicts, an arbitrary predication, and an inclusive prefix sum of that predication") {
		vector<Conflict>small_input = generateDeterministicRandomConflicts(a_few);
		vector<cl_int> small_pred(a_few), small_addr(a_few);
		vector<Conflict>small_output_s(a_few);
		/* In this example, odd indexes are compacted to the left. */
		for (int i = 0; i < a_few; ++i) { small_pred[i] = i % 2; small_addr[i] = (i + 1) / 2; }
		When("these conflicts are compacted in series") {
			CompactConflicts_s(small_input, small_pred, small_addr, small_output_s);
			Then("elements predicated true are moved to their cooresponding addresses.") {
				int success = true;
				for (int i = 0; i < a_few / 2; ++i) success &= (compareConflict(&small_output_s[i], &small_input[(i * 2) + 1]));
				Require(success == true);
			}
			Then("elements predicated false are placed after the last truely predicated element and in their original order.") {
				int success = true;
				for (int i = a_few / 2; i < a_few; ++i) success &= (compareConflict(&small_output_s[i], &small_input[(i - (a_few / 2)) * 2]));
				Require(success == true);
			}
			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<Conflict> small_output_p(a_few);
				cl::Buffer b_small_input, b_small_pred, b_small_addr, b_small_output;
				error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(Conflict));
				error |= CLFW::getBuffer(b_small_pred, "b_small_pred", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_addr, "b_small_addr", a_few * sizeof(cl_int));
				error |= CLFW::getBuffer(b_small_output, "b_small_output", a_few * sizeof(Conflict));
				error |= CLFW::Upload<Conflict>(small_input, b_small_input);
				error |= CLFW::Upload<cl_int>(small_pred, b_small_pred);
				error |= CLFW::Upload<cl_int>(small_addr, b_small_addr);
				error |= CompactConflicts_p(b_small_input, b_small_pred, b_small_addr, a_few, b_small_output);
				error |= CLFW::Download<Conflict>(b_small_output, a_few, small_output_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) success &= (compareConflict(&small_output_p[i], &small_output_s[i]));
				Require(success == true);
			}
		}
	}
}