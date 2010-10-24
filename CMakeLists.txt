################################################################################
# This CMakeLists.txt is devided into three sections:                          #
#                                                                              #
#    1) CMake environment setup                                                #
#    2) Library Setup                                                          #
#    3) Compiler Setup                                                         #
#                                                                              #
################################################################################


######################################
##### CMake environment setup    #####
######################################
project(Cartographer)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR})
include_directories(SYSTEM ${${PROJECT_NAME}_SOURCE_DIR}/include)


######################################
##### Library setup              #####
######################################

##### Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS filesystem system REQUIRED)

##### Qt
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include(${QT_USE_FILE})
include_directories(SYSTEM ${QT_INCLUDE_DIR})
include_directories(SYSTEM ${QT_QTGUI_INCLUDE_DIR})
include_directories(SYSTEM ${QT_QTCORE_INCLUDE_DIR})

##### PNG
find_package(PNG REQUIRED)

##### Intel Threading Building Blocks
find_path(TBB_INCLUDE_DIR "tbb/parallel_for.h")
find_library(TBB_LIBRARY tbb)
if(TBB_INCLUDE_DIR AND TBB_LIBRARY)
  set(TBB_FOUND TRUE)
  add_definitions(-DTBB_FOUND)
  include_directories(${TBB_INCLUDE_DIR})
else()
  message(STATUS "Intel TBB library not found!")
endif()
mark_as_advanced(TBB_INCLUDE_DIR TBB_LIBRARY)


######################################
##### Compiler setup             #####
######################################
include("CompilerFlags.txt")  # This sets WARNINGS and OPTIMIZATIONS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic ${WARNINGS} ${OPTIMIZATIONS}")
include("CrossCompile.txt")   # This is for MinGW cross compile support


add_subdirectory(src)
