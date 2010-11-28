# See LICENSE file for copyright and license details.
if(GTK2_FOUND)
  add_executable(gtkmmtest main)
  target_link_libraries(gtkmmtest ${GTK2_LIBRARIES})
endif()
