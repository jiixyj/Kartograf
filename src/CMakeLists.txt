add_library(nbt nbt)
add_library(tag tag)
add_library(colors colors)

add_executable(nbtparse main)

target_link_libraries(nbt colors tag ${QT_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(nbtparse nbt MainForm tbb)

add_subdirectory(MainForm)
