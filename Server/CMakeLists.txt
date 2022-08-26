
cmake_minimum_required (VERSION 3.8)

project ("Server")

add_executable (Server "main.cpp" "IServer.h" "CError.h" "CSerializeMap.h" "WinServer.h" "LinuxServer.h"  "WinServer.cpp" "CServerHandler.h" "CServerHandler.cpp" "IServer.cpp" "LinuxServer.cpp" "СError.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Server PROPERTY CXX_STANDARD 14)
endif()

