#include "Tests/catch.hpp"
#include "Tests/HelperFunctions.hpp"

Scenario("QPoints to ZPoints", "[1][zorder]") {
	Given("the number of bits supported by the z-ordering") {
		Given("an arbitrary set of positive int2s") {
			vector<int2> small_input = generateDeterministicRandomInt2s(a_few, 1, 0, 1024);
			vector<big> small_output_s(a_few);
			When("these points are placed on a Z-Order curve in series") {
				QPoints2DToZPoints_s(small_input, 30, small_output_s);
				Then("the series results match the parallel results") {
					cl_int error = 0;
					vector<big> small_output_p(a_few), large_output_p(a_lot);
					cl::Buffer b_small_input, b_small_output;
					error |= CLFW::getBuffer(b_small_input, "b_small_input", a_few * sizeof(big));
					error |= CLFW::Upload<int2>(small_input, b_small_input);
					error |= QPoints2DToZPoints_p(b_small_input, a_few, 30, "a", b_small_output);
					error |= CLFW::Download<big>(b_small_output, a_few, small_output_p);
					Require(error == CL_SUCCESS);
					int success = true;
					for (int i = 0; i < a_few; ++i) success &= (compareBig(&small_output_s[i], &small_output_p[i]) == 0);
					Require(success == true);
				}
			}
		}
	}
}

Scenario("Get LCPs From Lines", "[2][zorder][lcp]") {
	Given("two z-order points and a line connecting them") {
		int mbits = 8;
		vector<big> p(2);
		p[0] = makeBig(240); //11110000
		p[1] = makeBig(243); //11110011
		Line l;
		l.first = 0; l.second = 1;
		When("we get the LCP from this line's points") {
			LCP lcp = {};
			GetLCPFromLine(&l, p.data(), &lcp, mbits, 0);
			Require(lcp.len == 6);
			Require(lcp.bu.blk[0] == 60);
		}
	}
	Given("N z-ordered points and lines, and the number of bits per zpoint") {
		int mbits = 64;
		vector<big> small_zpoints = readFromFile<big>("TestData//few_u_s_zpoints.bin", a_few);
		vector<big> large_zpoints = readFromFile<big>("TestData//lot_u_s_zpoints.bin", a_lot);
		vector<Line> small_lines(a_few), large_lines(a_lot);
		for (int i = 0; i < a_few; ++i) {
			Line l;
			l.first = i;
			l.second = (i == a_few - 1) ? 0 : (i + 1);
			small_lines[i] = l;
		}
		for (int i = 0; i < a_lot; ++i) {
			Line l;
			l.first = i;
			l.second = (i == a_lot - 1) ? 0 : (i + 1);
			large_lines[i] = l;
		}

		When("we compute the lcps for these lines in series") {
			vector<LCP> small_lcps_s(a_few), large_lcps_s(a_lot);
			GetLineLCP_s(small_lines, small_zpoints, mbits, small_lcps_s);
			GetLineLCP_s(large_lines, large_zpoints, mbits, large_lcps_s);
			Then("the series results match the parallel results") {
				cl_int error = 0;
				vector<LCP> small_lcps_p(a_few), large_lcps_p(a_lot);
				cl::Buffer b_small_lines, b_large_lines, b_small_zpoints, b_large_zpoints, b_small_lcps, b_large_lcps;
				error |= CLFW::getBuffer(b_small_lines, "b_small_lines", a_few * sizeof(Line));
				error |= CLFW::getBuffer(b_large_lines, "b_large_lines", a_lot * sizeof(Line));
				error |= CLFW::getBuffer(b_small_zpoints, "b_small_zpoints", a_few * sizeof(big));
				error |= CLFW::getBuffer(b_large_zpoints, "b_large_zpoints", a_lot * sizeof(big));
				error |= CLFW::Upload<Line>(small_lines, b_small_lines);
				error |= CLFW::Upload<Line>(large_lines, b_large_lines);
				error |= CLFW::Upload<big>(small_zpoints, b_small_zpoints);
				error |= CLFW::Upload<big>(large_zpoints, b_large_zpoints);
				error |= GetLineLCPs_p(b_small_lines, a_few, b_small_zpoints, mbits, b_small_lcps);
				error |= GetLineLCPs_p(b_large_lines, a_lot, b_large_zpoints, mbits, b_large_lcps);
				error |= CLFW::Download<LCP>(b_small_lcps, a_few, small_lcps_p);
				error |= CLFW::Download<LCP>(b_large_lcps, a_lot, large_lcps_p);
				Require(error == CL_SUCCESS);
				int success = true;
				for (int i = 0; i < a_few; ++i) {
					success &= (compareLCP(&small_lcps_s[i], &small_lcps_p[i]) == 0);
					if (!success)
						success &= (compareLCP(&small_lcps_s[i], &small_lcps_p[i]) == 0);
				}
				for (int i = 0; i < a_lot; ++i)
				{
					success &= (compareLCP(&large_lcps_s[i], &large_lcps_p[i]) == 0);
					if (!success) 
						success &= (compareLCP(&large_lcps_s[i], &large_lcps_p[i]) == 0);
				}
				Require(success == true);
			}
		}
	}
}