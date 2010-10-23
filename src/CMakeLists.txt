add_library(nbt nbt)
add_library(tag tag)
add_library(colors colors)
add_library(assemble assemble)
target_link_libraries(nbt colors tag ${Boost_LIBRARIES} tbb)
target_link_libraries(assemble nbt ${PNG_LIBRARIES})

add_executable(nbtparse main)
add_executable(cart main_console)
target_link_libraries(nbtparse nbt ${QT_LIBRARIES} MainForm ${PNG_LIBRARIES})
target_link_libraries(cart nbt assemble)

add_subdirectory(MainForm)
