add_library(nbt nbt)
add_library(tag tag)
add_library(colors colors)
add_library(assemble assemble)
target_link_libraries(nbt colors tag ${Boost_LIBRARIES} ${TBB_LIBRARY})
target_link_libraries(tag ${ZLIB_LIBRARY})
target_link_libraries(assemble nbt ${PNG_LIBRARY})

add_executable(nbtparse main)
add_executable(cart main_console)
target_link_libraries(nbtparse MainForm)
target_link_libraries(cart assemble)

add_subdirectory(MainForm)
