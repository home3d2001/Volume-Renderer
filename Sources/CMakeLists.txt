# -----------------------------------------------------
# Add every subdirectory
# -----------------------------------------------------
MACRO(GETSUBDIRLIST RESULT CURDIR)
  FILE(GLOB CHILDREN RELATIVE ${CURDIR} ${CURDIR}/*)
  SET(DIRLIST "")
  FOREACH(CHILD ${CHILDREN})
    IF(IS_DIRECTORY ${CURDIR}/${CHILD})
      LIST(APPEND DIRLIST ${CHILD})
    ENDIF()
  ENDFOREACH()
  SET(${RESULT} ${DIRLIST})
ENDMACRO()

GETSUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

FOREACH(SUBDIR ${SUBDIRS})
  ADD_SUBDIRECTORY(${SUBDIR})
ENDFOREACH()

MACRO(GETFILES RESULT SUFFIX)
LIST(APPEND _${SUFFIX} "")
FOREACH(SUBDIR ${SUBDIRS})
	LIST(APPEND _${SUFFIX} ${${SUBDIR}_${SUFFIX}})
ENDFOREACH()
SET(${RESULT} ${_${SUFFIX}})
ENDMACRO()

# -----------------------------------------------------
# Standard project source files
# -----------------------------------------------------

# Fetch files
GETFILES(_SRC SRC)
GETFILES(_TEST TEST)
GETFILES(_KERNEL KRNL)

# Unit Test Target Source Files
set(QUADTREE_TEST ${_SRC} 
	${_TEST} 
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  PARENT_SCOPE)

# SBVR Target Source Files
set(SBVR_SRC ${_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Main/SBVRMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  PARENT_SCOPE)

# RCVR Target Source Files 
set(RCVR_SRC ${_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Main/RCVRMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  PARENT_SCOPE)

# SBVR_VR Target Source Files
set(OCTREE_VR_SRC ${_SRC} 
  ${CMAKE_CURRENT_SOURCE_DIR}/Events/vr.h
	${CMAKE_CURRENT_SOURCE_DIR}/Events/OctreeVREvents.h
	${CMAKE_CURRENT_SOURCE_DIR}/Events/OctreeVREvents.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Main/OctreeVRMain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
  PARENT_SCOPE)

# Kernel Source Files
set(KERNEL_SRC ${_KERNEL} 
	PARENT_SCOPE)
