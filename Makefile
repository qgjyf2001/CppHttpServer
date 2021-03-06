all:main

CC=g++
CXXFLAGS=-std=c++17 -O3
INCLUDE=./include

THREADPOOL=./threadPool
THREADPOOLTARGET=$(THREADPOOL)/threadPool.o
TCP=./tcp
TCPTARGET=$(TCP)/tcpServer.o $(TCP)/ltServer.o
HTTP=./http
HTTPTARGET=$(HTTP)/httpParser.o $(HTTP)/http.o $(HTTP)/httpHandler.o
WEBSERVER=./webServer
WEBSERVERTARGET=$(WEBSERVER)/webServer.o
FILESYSTEM=./fileSystem
FILESYSTEMTARGET=$(FILESYSTEM)/folderHook.o
JSONPARSER=./json
JSONPARSERTARGET=$(JSONPARSER)/jsonParser.o
SAFEVECTOR=./safeVector
SAFEVECTORTARGET=$(SAFEVECTOR)/safeVector.o
TOOLS=./tools
TOOLSTARGET=$(TOOLS)/zipFolder.o
SQL=./mysqlHelper
SQLTARGET=$(SQL)/mysqlHelper.o $(SQL)/mysqlHelperImp.o

$(THREADPOOLTARGET):$(THREADPOOL)/threadPool.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(TCP)/%.o:$(TCP)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(HTTP)/%.o:$(HTTP)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(TOOLS)/%.o:$(TOOLS)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(SQL)/%.o:$(SQL)/%.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(WEBSERVERTARGET):$(WEBSERVER)/webServer.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(FILESYSTEMTARGET):$(FILESYSTEM)/folderHook.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(JSONPARSERTARGET):$(JSONPARSER)/jsonParser.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
$(SAFEVECTORTARGET):$(SAFEVECTOR)/safeVector.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@

libCppServer.a:$(THREADPOOLTARGET) $(TCPTARGET) $(HTTPTARGET) $(WEBSERVERTARGET) $(FILESYSTEMTARGET) $(JSONPARSERTARGET) $(SAFEVECTORTARGET) $(TOOLSTARGET) $(SQLTARGET)
	ar -crv $@ $^
main.o:main.cpp
	$(CC) -I$(INCLUDE) $(CXXFLAGS) -c $^ -o $@
main:main.o libCppServer.a
	$(CC) -o $@ main.o -L. -lCppServer -lpthread -lminizip -lmysqlclient
web:
	cd ./app&&yarn build
clean:
	find . -name '*.o' -type f -print -exec rm -rf {} \;
	rm libCppServer.a
	rm main

