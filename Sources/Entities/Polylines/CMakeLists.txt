set (Polylines_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.h
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.cl
	PARENT_SCOPE)

set(Polylines_KRNL
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.h
	${CMAKE_CURRENT_SOURCE_DIR}/Polylines.cl
	PARENT_SCOPE)