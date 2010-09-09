add_library(Level Level)
add_library(render render)

add_executable(cart main)

target_link_libraries(Level ${QT_LIBRARIES} render)
target_link_libraries(cart Level)

add_subdirectory(nbt)
