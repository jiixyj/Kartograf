add_library(nbt nbt)
add_library(tag tag)

add_executable(nbtparse main)

target_link_libraries(nbt tag ${QT_LIBRARIES})
target_link_libraries(nbtparse nbt MainForm)

add_subdirectory(MainForm)
