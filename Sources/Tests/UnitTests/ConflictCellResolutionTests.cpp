///* Ambiguous cell resolution kernels */
//Scenario("Sample required resolution points", "[selected][resolution]") {
//	Given("a set of conflicts and the quantized points used to build the original octree") {
//		cl_int numConflicts = readFromFile<cl_int>("TestData//simple//numConflicts.bin");
//		cl_int numPoints = readFromFile<cl_int>("TestData//simple//numPoints.bin");
//		vector<Conflict> conflicts = readFromFile<Conflict>("TestData//simple//conflicts.bin", numConflicts);
//		vector<intn> qpoints = readFromFile<intn>("TestData//simple//qpoints.bin",  numPoints);
//		
//		When("we sample the information required to resolve these conflicts in series") {
//			vector<ConflictInfo> conflictInfo_s;
//			vector<cl_int> numPtsPerConflict_s;
//			GetResolutionPointsInfo_s(conflicts, qpoints, conflictInfo_s, numPtsPerConflict_s);
//			Then("the series results match the parallel results") {
//				cl_int error = 0;
//				vector<ConflictInfo> conflictInfo_p(numConflicts);
//				vector<cl_int> numPtsPerConflict_p(numConflicts);
//				cl::Buffer b_conflicts, b_qpoints, b_conflictInfo, b_numPtsPerConflict;
//				error |= CLFW::getBuffer(b_conflicts, "b_conflicts", numConflicts * sizeof(Conflict));
//				error |= CLFW::getBuffer(b_qpoints, "b_qpoints", numPoints * sizeof(intn));
//				error |= CLFW::Upload<Conflict>(conflicts, b_conflicts);
//				error |= CLFW::Upload<intn>(qpoints, b_qpoints);
//				error |= GetResolutionPointsInfo_p(b_conflicts, numConflicts, b_qpoints, b_conflictInfo, b_numPtsPerConflict);
//				error |= CLFW::Download<ConflictInfo>(b_conflictInfo, numConflicts, conflictInfo_p);
//				error |= CLFW::Download<cl_int>(b_numPtsPerConflict, numConflicts, numPtsPerConflict_p);
//				Require(error == CL_SUCCESS);
//				cl_int success = true;
//				for (int i = 0; i < numConflicts; ++i) {
//					success &= compareConflictInfo(&conflictInfo_s[i], &conflictInfo_p[i]);
//					success &= (numPtsPerConflict_s[i] == numPtsPerConflict_p[i]);
//					if (!success) {
//						success &= compareConflictInfo(&conflictInfo_s[i], &conflictInfo_p[i]);
//						success &= (numPtsPerConflict_s[i] == numPtsPerConflict_p[i]);
//					}
//				}
//				Require(success == true);
//			}
//			Then("our results are valid") {
//				vector<ConflictInfo> conflictInfo_f = readFromFile<ConflictInfo>("TestData//simple//conflictInfo.bin", numConflicts);
//				vector<cl_int>numPtsPerConflict_f = readFromFile<cl_int>("TestData//simple//numPtsPerConflict.bin", numConflicts);
//				cl_int success = true;
//				for (int i = 0; i < numConflicts; ++i) {
//					success &= compareConflictInfo(&conflictInfo_s[i], &conflictInfo_f[i]);
//					success &= (numPtsPerConflict_s[i] == numPtsPerConflict_f[i]);
//					if (!success) {
//						success &= compareConflictInfo(&conflictInfo_s[i], &conflictInfo_f[i]);
//						success &= (numPtsPerConflict_s[i] == numPtsPerConflict_f[i]);
//					}
//				}
//				Require(success == true);
//			}
//		}
//	}
//}
//Scenario("Predicate Conflict To Point", "[predication][resolution]") {
//	Given("the scanned number of resolution points to create per conflict") {
//		cl_int numConflicts = readFromFile<cl_int>("./TestData/simple/numConflicts.bin");
//		cl_int numResPts = readFromFile<cl_int>("TestData//simple//numResPts.bin");
//		vector<cl_int> scannedNumPtsPerConflict = readFromFile<cl_int>("TestData//simple//scannedNumPtsPerConflict.bin", numConflicts);
//		When("we predicate the first point cooresponding to a conflict") {
//			vector<cl_int> predicates(numResPts, 0);
//			predPntToConflict(scannedNumPtsPerConflict.data(), predicates.data(), 0);
//			predPntToConflict(scannedNumPtsPerConflict.data(), predicates.data(), numConflicts - 2);
//			predPntToConflict(scannedNumPtsPerConflict.data(), predicates.data(), 10);
//			Require(predicates[4] == 1);
//			Require(predicates[56] == 1);
//			Require(predicates[72] == 1);
//		}
//		When("we predicate the first points in series") {
//			vector<cl_int> predication_s;
//			PredicatePointToConflict_s(scannedNumPtsPerConflict, numResPts, predication_s);
//			Then("the series results match the parallel results") {
//				cl_int error = 0;
//				vector<cl_int> predication_p;
//				cl::Buffer b_scannedNumPtsPerConflict, b_predication;
//				error |= CLFW::getBuffer(b_scannedNumPtsPerConflict, "b_scannedNumPtsPerConflict", numConflicts * sizeof(cl_int));
//				error |= CLFW::Upload<cl_int>(scannedNumPtsPerConflict, b_scannedNumPtsPerConflict);
//				error |= PredicatePointToConflict_p(b_scannedNumPtsPerConflict, numConflicts, numResPts, b_predication);
//				error |= CLFW::Download<cl_int>(b_predication, numResPts, predication_p);
//				Require(error == CL_SUCCESS);
//				cl_int success = true;
//				for (int i = 0; i < numConflicts; ++i) {
//					success &= (predication_s[i] == predication_p[i]);
//				}
//				Require(success == true);
//			}
//		}
//	}
//}
//Scenario("Get resolution points", "[resolution]") {
//	Given("a set of conflicts and cooresponding conflict infos, a resolution point to conflict mapping, "
//		+ "and the original quantized points used to build the octree") {
//		cl_int numConflicts = readFromFile<cl_int>("TestData//simple//numConflicts.bin");
//		cl_int numResPts = readFromFile<cl_int>("TestData//simple//numResPts.bin");
//		cl_int numPts = readFromFile<cl_int>("TestData//simple//numPoints.bin");
//		vector<Conflict> conflicts = readFromFile<Conflict>("TestData//simple//conflicts.bin", numConflicts);
//		vector<ConflictInfo> conflictInfo = readFromFile<ConflictInfo>("TestData//simple//conflictInfo.bin", numConflicts);
//		vector<cl_int> pntToConflict = readFromFile<cl_int>("TestData//simple//pntToConflict.bin", numResPts);
//		vector<cl_int> scannedNumPtsPerConflict = readFromFile<cl_int>("TestData//simple//scannedNumPtsPerConflict.bin", numConflicts);
//		vector<intn> qpoints = readFromFile<intn>("TestData//simple//qpoints.bin", numPts);
//		vector<cl_int>numPtsPerConflict_f = readFromFile<cl_int>("TestData//simple//numPtsPerConflict.bin", numConflicts);
//		vector<cl_int> test(numPtsPerConflict_f.size());
//		When("we use this data to get the resolution points in series") {
//			vector<intn> resPts_s;
//			GetResolutionPoints_s(conflicts, conflictInfo, scannedNumPtsPerConflict, pntToConflict, numResPts, qpoints, resPts_s);
//			Then("these points should be valid") {
//				vector<intn> resPts_f = readFromFile<intn>("TestData//simple//resPts.bin", numResPts);
//				cl_int success = true;
//				for (int i = 0; i < numResPts; ++i) success &= (resPts_s[i] == resPts_f[i]);
//				Require(success == true);
//			}
//			Then("the series results match the parallel results") {
//				cl_int error = 0;
//				vector<intn> resPts_p;
//				cl::Buffer b_conflicts, b_conflictInfo, b_scannedNumPtsPerConflict, b_pntToConflict, b_qpoints, b_resPts;
//				error |= CLFW::getBuffer(b_conflicts, "b_conflicts", numConflicts * sizeof(Conflict));
//				error |= CLFW::getBuffer(b_conflictInfo, "b_conflictInfo", numConflicts * sizeof(ConflictInfo));
//				error |= CLFW::getBuffer(b_scannedNumPtsPerConflict, "b_scannedNumPtsPerConflict", numConflicts * sizeof(cl_int));
//				error |= CLFW::getBuffer(b_pntToConflict, "b_pntToConflict", numResPts * sizeof(cl_int));
//				error |= CLFW::getBuffer(b_qpoints, "b_qpoints", numPts * sizeof(intn));
//				error |= CLFW::Upload<Conflict>(conflicts, b_conflicts);
//				error |= CLFW::Upload<ConflictInfo>(conflictInfo, b_conflictInfo);
//				error |= CLFW::Upload<cl_int>(scannedNumPtsPerConflict, b_scannedNumPtsPerConflict);
//				error |= CLFW::Upload<cl_int>(pntToConflict, b_pntToConflict);
//				error |= CLFW::Upload<intn>(qpoints, b_qpoints);
//				error |= GetResolutionPoints_p(b_conflicts, b_conflictInfo, b_scannedNumPtsPerConflict, numResPts, b_pntToConflict, b_qpoints, b_resPts);
//				error |= CLFW::Download<intn>(b_resPts, numResPts, resPts_p);
//				Require(error == CL_SUCCESS);
//				cl_int success = true;
//				for (int i = 0; i < numResPts; ++i) success &= (resPts_s[i] == resPts_p[i]);
//				Require(success == true);
//			}
//		}
//	}
//}