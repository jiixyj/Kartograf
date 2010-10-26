qt4_automoc(MainForm.cpp)

add_library(MainForm MainForm)

target_link_libraries(MainForm assemble ${QT_LIBRARIES})
