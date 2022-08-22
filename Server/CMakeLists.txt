
cmake_minimum_required (VERSION 3.8)

project ("Server")


add_executable (Server "Server.cpp" "Server.h" "CSerializeMap.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Server PROPERTY CXX_STANDARD 11)
endif()

