cmake_minimum_required (VERSION 2.6)
include(${CMAKE_CURRENT_SOURCE_DIR}/CopyIfDifferent.txt)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Must go before the project command
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING "Minimum OS X deployment version")

project (Volume_Renderer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(SRC_PATH "Sources")

#------------------------------------------------------------
# Options will go here
#------------------------------------------------------------
option(BUILD_VR "Build the VR target" OFF)

#------------------------------------------------------------
# Use C++ 11
#------------------------------------------------------------

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

#------------------------------------------------------------
# External dependencies
#------------------------------------------------------------

find_package(OpenGL REQUIRED)
include_directories(SYSTEM ${OPENGL_INCLUDE_DIR})

# use GLFW
find_package(GLFW REQUIRED)
include_directories(SYSTEM ${GLFW_INCLUDE_DIR})

# use GLEW
find_package(GLEW REQUIRED)
include_directories(SYSTEM ${GLEW_INCLUDE_DIR})

# use GLM
find_package(GLM REQUIRED)
include_directories(SYSTEM ${GLM_INCLUDE_DIR})

# use OpenCL
find_package(OpenCL REQUIRED)
include_directories(SYSTEM ${OPENCL_INCLUDE_DIR})

# use CLFW (find OpenCL must be included first.)
add_subdirectory(Dependencies/CLFW)
include_directories(${CLFW_INCLUDE_DIR})

# use OpenCV 
# find_package(OpenCV REQUIRED)
# include_directories(SYSTEM ${OpenCV_INCLUDE_DIR})

set(LIBRARIES 
    ${OPENCL_LIBRARY} 
    ${OPENGL_LIBRARIES} 
    ${GLFW_LIBRARIES} 
    ${GLEW_LIBRARY} 
    ${OpenCV_LIBRARIES})

# use OpenVR
if (BUILD_VR)
	find_package(OpenVR REQUIRED)
	include_directories(SYSTEM ${OPENVR_INCLUDE_DIRS})
endif(BUILD_VR)

#------------------------------------------------------------
# Source Files/Shaders/Kernels (Uses a recursive cmake tree)
#------------------------------------------------------------
add_subdirectory(Sources)

# Copy list of OpenCL sources to file for opencl runtime
string(REPLACE ";" "\n" opencl_sources "${KERNEL_SRC}")
file(WRITE Sources/OpenCL/opencl_sources.txt ${opencl_sources})

# For IDE folder hierarchy generation
file(
    GLOB_RECURSE SRC_LIST 
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SRC_PATH}/*.c*"
    "${SRC_PATH}/*.h*"
    "${SRC_PATH}/*.txt*"
    "${SRC_PATH}/*.frag*"
    "${SRC_PATH}/*.vert*"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Sources)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)

# UNIT TEST TARGET
# add_executable (Unit_Tests "${QUADTREE_TEST}")
# target_link_libraries (Unit_Tests ${LIBRARIES} CLFW)

# SLICE BASED TARGET
add_executable (SBVR "${SBVR_SRC}")
target_link_libraries (SBVR ${LIBRARIES} CLFW)

# RAY CASTED VOLUME RENDERER
add_executable (RCVR "${RCVR_SRC}")
target_link_libraries (RCVR ${LIBRARIES} CLFW)

# SLICE BASED VR Target
if(BUILD_VR)
add_executable (SBVR_VR "${OCTREE_VR_SRC}")
target_link_libraries (SBVR_VR ${LIBRARIES} CLFW)
endif(BUILD_VR)


# For IDE folder hierarchy generation
foreach(_source IN ITEMS ${SRC_LIST})
	get_filename_component(_source_path "${_source}" PATH)
	string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
	source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()  

	# Temporary target to copy the kernel sources to where the main target is built.
copy_touched (
 ${CMAKE_SOURCE_DIR}/Sources
 ${CMAKE_CURRENT_BINARY_DIR}/Sources
 CopySourcesToBinary
)
if(BUILD_VR)
add_dependencies(SBVR_VR CopySourcesToBinary)
endif(BUILD_VR)
# add_dependencies(2D_Benchmarks CopySourcesToBinary)
# add_dependencies(2D_Quadtree CopySourcesToBinary)
