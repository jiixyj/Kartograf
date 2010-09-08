cmake_minimum_required(VERSION 2.8)
project(Cartographer)

set(${PROJECT_NAME}_MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${${PROJECT_NAME}_MAIN_DIR}/cmake)
set(EXECUTABLE_OUTPUT_PATH ${${PROJECT_NAME}_BINARY_DIR})

find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
include(${QT_USE_FILE})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -s")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -Wfatal-errors -isystem${${PROJECT_NAME}_MAIN_DIR}/include -isystem${QT_INCLUDE_DIR} -isystem${QT_QTGUI_INCLUDE_DIR} -isystem${QT_QTCORE_INCLUDE_DIR} -Wall -Wextra -Wshadow -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Winit-self -Wmissing-include-dirs -Wunused-parameter -Wunknown-pragmas -Wstrict-overflow=1 -Wfloat-equal -Wundef -Wno-endif-labels -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-format-attribute -Wno-multichar -Wpacked -Wredundant-decls -Winline -Winvalid-pch -Wdisabled-optimization -Wconversion -Wabi -Wstrict-null-sentinel -Weffc++ -Wno-non-template-friend -Wsign-promo -Wswitch-default -Wsync-nand -Wunsafe-loop-optimizations -Wlogical-op -Wvla -Wstack-protector -Wold-style-cast -Wno-pmf-conversions")

if(MINGW)
  add_definitions("-DWIN32")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all")
endif()

link_directories(
)

include_directories(
  ${${PROJECT_NAME}_MAIN_DIR}/include
)

add_subdirectory(src)
