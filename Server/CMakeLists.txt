
cmake_minimum_required (VERSION 3.8)

project ("Server")

add_executable (Server "main.cpp"  "CError.h" "CSerializeMap.h"     "CServerHandler.h" "CServerHandler.cpp" "IServer.cpp"  "CError.cpp" "LinuxServer.cpp" "LinuxServer.h" "WinServer.cpp" "WinServer.h" "IServer.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Server PROPERTY CXX_STANDARD 14)
endif()

