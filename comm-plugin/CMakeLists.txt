add_library(comm-plugin STATIC
  cpptest_gtest_xml_listener.cpp
  cpptest_gtest_xml_listener.h
)

target_include_directories(comm-plugin PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(comm-plugin PUBLIC gtest)
