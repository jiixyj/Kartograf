# See LICENSE file for copyright and license details.
add_library(nbt nbt)
add_library(tag tag)
add_library(colors colors)
add_library(image image)
add_library(assemble assemble)
add_library(settings settings)
target_link_libraries(nbt tag image settings ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
target_link_libraries(tag ${ZLIB_LIBRARY})
target_link_libraries(image colors)
target_link_libraries(assemble nbt ${PNG_LIBRARY} ${TBB_LIBRARY})

add_executable(kartograf_cli main_console)
target_link_libraries(kartograf_cli assemble ${Boost_PROGRAM_OPTIONS_LIBRARY})

if(QT4_FOUND)
  add_executable(kartograf_qt4 MACOSX_BUNDLE main)
  target_link_libraries(kartograf_qt4 MainGUI)

  add_subdirectory(MainForm)
endif()
