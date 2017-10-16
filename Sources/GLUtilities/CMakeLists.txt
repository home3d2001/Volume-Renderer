add_subdirectory(Boxes)
add_subdirectory(Polylines)
add_subdirectory(ImagePlane)
add_subdirectory(SlicedVolume)

set (GLUtilities_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/gl_utils.h 
	${CMAKE_CURRENT_SOURCE_DIR}/gl_utils.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/Entity.h
	${CMAKE_CURRENT_SOURCE_DIR}/Scene.h
	${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/OrbitCamera.h 
	${CMAKE_CURRENT_SOURCE_DIR}/OrbitCamera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
	${CMAKE_CURRENT_SOURCE_DIR}/objload.h
	${CMAKE_CURRENT_SOURCE_DIR}/Polygons.h
	${CMAKE_CURRENT_SOURCE_DIR}/Polygons.cpp
	${Boxes_SRC}
	${Polylines_SRC}
	${ImagePlane_SRC}
	${SlicedVolume_SRC}
	PARENT_SCOPE)

set (GLUtilities_KRNL
	${Boxes_KRNL}
	${Polylines_KRNL}
	${ImagePlane_KRNL}
	${SlicedVolume_KRNL}
	PARENT_SCOPE) 