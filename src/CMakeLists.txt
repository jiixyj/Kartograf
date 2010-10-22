add_library(nbt nbt)
add_library(tag tag)
add_library(colors colors)

add_executable(nbtparse main)
add_executable(cart main_console)

target_link_libraries(nbt colors tag ${Boost_LIBRARIES} tbb)
target_link_libraries(nbtparse nbt ${QT_LIBRARIES} MainForm)
target_link_libraries(cart nbt ${ZLIB_LIBRARIES} ${Boost_LIBRARIES})

add_subdirectory(MainForm)
