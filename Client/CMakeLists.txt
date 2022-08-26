
cmake_minimum_required (VERSION 3.8)

project ("Client")

add_executable (Client "Client.cpp" "Client.h" "CSerializeMap.h" "Process.h" "Process.cpp" "IClient.h" "CWinClient.h" "CLinuxClient.h" "CError.h" "CClientHandler.h" "CError.cpp" "IProcess.h" "CClientHandler.cpp" "CLinuxProcess.h"  "CLinuxClient.cpp" "CWinProcess.h" "CWinProcess.cpp" "CWinClient.cpp" "IClient.cpp" "CLinuxProcess.cpp " "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Client PROPERTY CXX_STANDARD 11)
endif()
