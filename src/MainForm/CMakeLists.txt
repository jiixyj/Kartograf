# See LICENSE file for copyright and license details.
qt4_automoc(MainForm.cpp)
add_library(MainForm MainForm)
target_link_libraries(MainForm assemble ${QT_LIBRARIES})

qt4_automoc(MainGUI.cpp)
add_library(MainGUI MainGUI)
target_link_libraries(MainGUI MainForm)
