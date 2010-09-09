add_library(nbt nbt)

add_executable(nbtparse main)

target_link_libraries(nbtparse nbt)
