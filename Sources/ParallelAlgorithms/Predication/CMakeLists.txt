# Predication kernels are seperated into their own .cl and h files for portability.
# If we want to use a subset of these kernels somewhere else, they should be 
# fairly easy to port over. 

set(Predication_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/Predication.cl
	${CMAKE_CURRENT_SOURCE_DIR}/Predication.h
	${CMAKE_CURRENT_SOURCE_DIR}/Predication.hpp
	PARENT_SCOPE)

set(Predication_KRNL
	${CMAKE_CURRENT_SOURCE_DIR}/Predication.cl
	PARENT_SCOPE)