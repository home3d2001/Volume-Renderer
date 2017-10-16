set (UnitTests_TEST
	${CMAKE_CURRENT_SOURCE_DIR}/BinaryRadixTreeTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/CompactionTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ConflictCellDetectionTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ConflictCellResolutionTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OctreeTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PredicationTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/QuantizeTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ReductionTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScanTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SortTests.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ZOrderTests.cpp
	PARENT_SCOPE)