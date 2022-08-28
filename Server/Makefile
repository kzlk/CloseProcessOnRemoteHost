OUT	= server_app
CPP	 = g++

ifeq ($(OS),Windows_NT)

OBJS	= CServerHandler.o IServer.o  main.o WinServer.o CError.o
SOURCE	= CServerHandler.cpp IServer.cpp  main.cpp WinServer.cpp CError.cpp
HEADER	= CSerializeMap.h CServerHandler.h IServer.h  WinServer.h

CXXFLAGS =  -Wextra -Wall -Wpedantic -O0 -g3 -std=c++14 -L"ws2_32" -c
LDFLAGS	 =  -lstdc++fs -lwsock32 -lWs2_32

all: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT) $(LDFLAGS)

server_app: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT)

WinServer.o: WinServer.cpp
	$(CPP) $(CXXFLAGS) WinServer.cpp 

else 

ifeq ($(shell uname), Linux)

OBJS	= CServerHandler.o IServer.o LinuxServer.o main.o CError.o
SOURCE	= CServerHandler.cpp IServer.cpp LinuxServer.cpp main.cpp CError.cpp
HEADER	= CSerializeMap.h CServerHandler.h IServer.h LinuxServer.h 

CXXFLAGS =  -Wextra -Wall -Wpedantic -O0 -g3 -std=c++14 -c
LDFLAGS	 =  -lstdc++fs
all: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT) $(LDFLAGS)

server_app: $(OBJS)
	$(CPP) -g $(OBJS) -o $(OUT)


LinuxServer.o: LinuxServer.cpp
	$(CPP) $(CXXFLAGS) LinuxServer.cpp 

endif
endif


CError.o: CError.cpp
	$(CPP) $(CXXFLAGS) CError.cpp 

CServerHandler.o: CServerHandler.cpp
	$(CPP) $(CXXFLAGS) CServerHandler.cpp 

IServer.o: IServer.cpp
	$(CPP) $(CXXFLAGS) IServer.cpp 

main.o: main.cpp
	$(CPP) $(CXXFLAGS) main.cpp 

clean:
	rm -f $(OBJS) $(OUT)

run: $(OUT)
	./$(OUT)
