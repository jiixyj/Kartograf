cmake_minimum_required(VERSION 2.8)
project(Cartographer)

set(${PROJECT_NAME}_MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${${PROJECT_NAME}_MAIN_DIR}/cmake)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR})

find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
set(WARNINGS "
  -Wfatal-errors
  -Wall
  -Wextra
  -Wformat=2
  -Wcomment
  -Wshadow
  -Wctor-dtor-privacy
  -Wnon-virtual-dtor
  -Wreorder
  -Winit-self
  -Wmissing-include-dirs
  -Wunused
  -Wunused-parameter
  -Wunknown-pragmas
  -Wstrict-overflow=1
  -Wstrict-aliasing
  -Wfloat-equal
  -Wundef
  -Wcast-qual
  -Wcast-align
  -Wwrite-strings
  -Wmissing-format-attribute
  -Wno-multichar
  -Wpacked
  -Wredundant-decls
  -Winvalid-pch
  -Wlong-long
  -Wdisabled-optimization
  -Wconversion
  -Wabi
  -Wstrict-null-sentinel
  -Weffc++
  -Wno-non-template-friend
  -Wsign-promo
  -Wswitch-default
  -Wsync-nand
  -Wlogical-op
  -Wvla
  -Wstack-protector
  -Wold-style-cast
  -Wno-pmf-conversions
 ")
set(NOWARNINGS "
  -Wstrict-overflow=4
  -Winline
  -Wmissing-noreturn
  -Wswitch-enum
  -Wsign-conversion
  -Waggregate-return
  -Wpadded

  -Wunsafe-loop-optimizations
 ")

set(SYSTEMINCLUDES "
  -isystem${${PROJECT_NAME}_MAIN_DIR}/include
  -isystem${QT_INCLUDE_DIR}
  -isystem${QT_QTGUI_INCLUDE_DIR}
  -isystem${QT_QTCORE_INCLUDE_DIR}
 ")
set(OPTIMIZATIONS "
  -fstack-protector-all
  -funsafe-loop-optimizations
  -fopenmp
 ")

set(CMAKE_CXX_FLAGS "-ansi -pedantic ${WARNINGS} ${SYSTEMINCLUDES} ${OPTIMIZATIONS}")
string(REPLACE "\n " "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if(MINGW)
  add_definitions("-DWIN32")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

link_directories(
)

include_directories(
  ${${PROJECT_NAME}_MAIN_DIR}/include
)

add_subdirectory(src)
