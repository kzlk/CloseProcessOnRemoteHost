
cmake_minimum_required (VERSION 3.8)

project ("untitled3")

add_executable (untitled3   "CSerializeMap.h"   "IClient.h" "CWinClient.h" "CLinuxClient.h" "CError.h" "CClientHandler.h" "CError.cpp" "IProcess.h" "CClientHandler.cpp" "CLinuxProcess.h"  "CLinuxProcess.cpp" "CWinProcess.h" "CWinProcess.cpp" "CWinClient.cpp" "IClient.cpp" "CLinuxProcess.cpp"  "CLinuxClient.cpp" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET untitled3 PROPERTY CXX_STANDARD 11)
endif()
