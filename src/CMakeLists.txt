add_library(Level Level)
add_library(render render)
add_library(Color Color)

add_executable(cart main)

target_link_libraries(Level ${QT_LIBRARIES} render Color)
target_link_libraries(cart Level)
