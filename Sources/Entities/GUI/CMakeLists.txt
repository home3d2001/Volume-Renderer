add_subdirectory(2D_Desktop_TransferFunction)
add_subdirectory(2DEditor)
add_subdirectory(CurveEditor)
set (GUI_SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
	${CMAKE_CURRENT_SOURCE_DIR}/Canvas.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Canvas.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Button.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Button.cpp
	${2D_DESKTOP_TRANSFERFUNCTION_SRC}
	${2D_EDITOR_SRC}
	${CURVE_EDITOR_SRC}
	PARENT_SCOPE)
