add_subdirectory(UnitTests)
add_subdirectory(Benchmarks)

set (Tests_TEST 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/catch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/HelperFunctions.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/catchmain.cpp
	${UnitTests_TEST}
	PARENT_SCOPE)

set (Tests_BENCH 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/catch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/HelperFunctions.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkMain.cpp
	${Benchmarks_BENCH}
	PARENT_SCOPE)