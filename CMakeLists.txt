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
project(Kartograf)
cmake_minimum_required(VERSION 2.6.4)
set(CMAKE_MODULE_PATH ${${PROJECT_NAME}_SOURCE_DIR}/cmake)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR}/lib)
if(IS_DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}/include)
  include_directories(SYSTEM ${${PROJECT_NAME}_SOURCE_DIR}/include)
endif()


######################################
##### Library setup              #####
######################################

##### Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS filesystem system REQUIRED)
if(WIN32)
  find_package(Boost COMPONENTS thread_win32 REQUIRED)
else()
  find_package(Boost COMPONENTS thread REQUIRED)
endif()

##### GTK2
find_package(GTK2 COMPONENTS gtk gtkmm)
if(GTK2_FOUND)
  include_directories(SYSTEM ${GTK2_INCLUDE_DIRS})
  include_directories(SYSTEM "/usr/include/gdk-pixbuf-2.0/")
  include_directories(SYSTEM "/usr/lib/pangomm-1.4/include/")
  include_directories(SYSTEM "/usr/include/freetype2/")
endif()

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
  include_directories(SYSTEM ${TBB_INCLUDE_DIR})
else()
  message(STATUS "Intel TBB library not found!")
endif()
mark_as_advanced(TBB_INCLUDE_DIR TBB_LIBRARY)
message(STATUS ${TBB_INCLUDE_DIR})
message(STATUS ${TBB_LIBRARY})


######################################
##### Compiler setup             #####
######################################
include("${CMAKE_MODULE_PATH}/CompilerFlags.txt")  # This sets VALID_FLAGS
set(CMAKE_CXX_FLAGS "${VALID_FLAGS}")
include("${CMAKE_MODULE_PATH}/CrossCompile.txt")   # This is for MinGW cross compile support

if(CMAKE_COMPILER_IS_GNUCXX AND COMPILER_VERSION STRLESS "4.2.2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem${Boost_INCLUDE_DIR} -isystem${QT_QTGUI_INCLUDE_DIR} -isystem${QT_QTCORE_INCLUDE_DIR} -isystem${TBB_INCLUDE_DIR}")
endif()


add_subdirectory(src)


if(APPLE)
  set(APPS ${${PROJECT_NAME}_BINARY_DIR}/nbtparse.app)  # paths to executables
  set(LIBS "")
  set(DIRS "")   # directories to search for prerequisites
  INSTALL(CODE "
     include(BundleUtilities)
     fixup_bundle(\"${APPS}\"   \"${LIBS}\"   \"${DIRS}\")
     " COMPONENT Runtime)
endif()
