add_library(Color Color)
add_library(IMG IMG)
add_library(Level Level)

add_executable(cart main)

target_link_libraries(cart Color IMG Level ${ZLIB_LIBRARIES} ${QT_LIBRARIES})
