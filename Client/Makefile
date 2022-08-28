OUT	= client_app
CPP	 = g++

ifeq ($(OS),Windows_NT)
detected_OS := Windows
OBJS	=  CError.o CClientHandler.o  CWinClient.o CWinProcess.o IClient.o main.o
SOURCE	=  CError.cpp CClientHandler.cpp  CWinClient.cpp CWinProcess.cpp IClient.cpp main.cpp
HEADER	= CError.h  CClientHandler.h CSerializeMap.h CWinClient.h CWinProcess.h IClient.h IProcess.h

CXXFLAGS =  -Wextra -Wall -Wpedantic -O0 -g3 -std=c++14 -L"ws2_32" -c
LDFLAGS	 =  -lstdc++fs -lwsock32 -lWs2_32
all: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT) $(LDFLAGS)

server_app: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT)

CWinClient.o: CWinClient.cpp
	$(CPP) $(CXXFLAGS)  CWinClient.cpp 

CWinProcess.o: CWinProcess.cpp
	$(CPP) $(CXXFLAGS)  CWinProcess.cpp 
	

else
detected_OS := $(shell uname)
ifeq ($(detected_OS), Linux)
OBJS	= CLinuxProcess.o CError.o CClientHandler.o CLinuxClient.o  IClient.o main.o
SOURCE	= CLinuxProcess.cpp CError.cpp CClientHandler.cpp CLinuxClient.cpp  IClient.cpp main.cpp
HEADER	= CError.h CLinuxProcess.h CLinuxClient.h CClientHandler.h CSerializeMap.h IClient.h IProcess.h
CXXFLAGS =  -Wextra -Wall -Wpedantic -O0 -g3 -std=c++14 -c
LDFLAGS	 =  -lstdc++fs

all: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT) $(LDFLAGS)

server_app: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT)
	
CLinuxProcess.o: CLinuxProcess.cpp
	$(CPP) $(CXXFLAGS) CLinuxProcess.cpp 


CLinuxClient.o: CLinuxClient.cpp
	$(CPP) $(CXXFLAGS) CLinuxClient.cpp 

endif
endif


CError.o: CError.cpp
	$(CPP) $(CXXFLAGS)   CError.cpp 

CClientHandler.o: CClientHandler.cpp
	$(CPP) $(CXXFLAGS)   CClientHandler.cpp 


IClient.o: IClient.cpp
	$(CPP) $(CXXFLAGS)  IClient.cpp 

main.o: main.cpp
	$(CPP) $(CXXFLAGS)  main.cpp 

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)
