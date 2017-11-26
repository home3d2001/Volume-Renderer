add_subdirectory(Boxes)
add_subdirectory(Box)
add_subdirectory(Cameras)
add_subdirectory(GUI)
add_subdirectory(ImagePlane)
add_subdirectory(Polylines)
add_subdirectory(SlicedVolume)
add_subdirectory(RaycastVolume)

set (Entities_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/Entity.h
	${CMAKE_CURRENT_SOURCE_DIR}/Transform.h
	${CMAKE_CURRENT_SOURCE_DIR}/Scene.h
	${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
	${Boxes_SRC}
	${Box_SRC}
	${Cameras_SRC}
	${Polylines_SRC}
	${ImagePlane_SRC}
	${SlicedVolume_SRC}
	${RaycastVolume_SRC}
	${GUI_SRC}
	PARENT_SCOPE)

set (Entities_KRNL
	${Boxes_KRNL}
	${Polylines_KRNL}
	${ImagePlane_KRNL}
	${SlicedVolume_KRNL}
	${RaycastVolume_KRNL}
	PARENT_SCOPE) 