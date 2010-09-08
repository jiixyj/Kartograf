add_library(Color Color)
add_library(IMG IMG)
add_library(Level Level)

add_executable(cart main)

target_link_libraries(IMG Color)
target_link_libraries(Level IMG ${QT_LIBRARIES})
target_link_libraries(cart Level)
