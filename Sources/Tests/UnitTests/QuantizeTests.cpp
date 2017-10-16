#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"
#include "ZOrder/ZOrder.hpp"
#include "Quantize/Quantize.hpp"

Scenario("Quantize Points", "[quantize]") {
	Given("A bounding box and a resolution") {
		floatn minimum = make_floatn(0.0, 0.0);
		floatn maximum = make_floatn(1000.0, 1000.0);
		BoundingBox2D bb = BB2D_initialize(&minimum, &maximum);
		int resolution_width = 64;
		Given("an arbitrary float2, say a bb min, bb max, and a point in the middle") {
			floatn middle = make_float2(500.0, 500.0);
			Then("those points can be quantized, or mapped, to the cooresponding quantized integer.") {
				intn quantized_min = QuantizePoint(&minimum, &minimum, resolution_width, bb.maxwidth);
				intn quantized_max = QuantizePoint(&maximum, &minimum, resolution_width, bb.maxwidth);
				intn quantized_middle = QuantizePoint(&middle, &minimum, resolution_width, bb.maxwidth);
				Require(quantized_min.x == 0);
				Require(quantized_min.y == 0);
				Require(quantized_max.x == resolution_width - 1);
				Require(quantized_max.x == resolution_width - 1);
				Require(quantized_middle.x == resolution_width / 2);
				Require(quantized_middle.y == resolution_width / 2);
			}
		}
		Given("an arbitrary set of float2s within the bounding box") {
			vector<float2> small_input = generateDeterministicRandomFloat2s(a_few, 0, 0.0, 1000.0);
			When("these points are quantized in series") {
				vector<int2> small_output_s(a_few);
				Quantize2DPoints_s(small_input, bb, resolution_width, small_output_s);
				Then("the series results match the parallel results") {
					cl_int error = 0;
					cl::Buffer b_small_input, b_small_output;
					vector<intn> small_output_p(a_few);
					error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(int2));
					error |= CLFW::Upload<floatn>(small_input, b_small_input);
					error |= Quantize2DPoints_p(b_small_input, a_few, bb, resolution_width, "a", b_small_output);
					error |= CLFW::Download<intn>(b_small_output, a_few, small_output_p);
					Require(error == CL_SUCCESS);
					int success = true;
					for (int i = 0; i < a_few; ++i) {
						success &= (small_output_s[i] == small_output_p[i]);
					}
					Require(success == true);
				}
			}
		}
	}
}