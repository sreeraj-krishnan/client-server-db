CC=gcc
CXX=g++
INCLUDE=-I../common/include -I/usr/local/include -I./include
CXXFLAGS=--std=c++14 $(INCLUDE)
RM=rm -f
CPPFLAGS=
LDFLAGS=
LDLIBS=-L/usr/local/lib -lboost_system -pthread -lpthread  -lprotobuf -lodb -lodb-pgsql

COMMON_SRC_PATH=../common/src
SRC_PATH=./src

SRCS=$(COMMON_SRC_PATH)/machine_info-odb.cpp $(COMMON_SRC_PATH)/machine.pb.cc $(SRC_PATH)/client.cpp $(SRC_PATH)/server.cpp $(SRC_PATH)/persistence.cpp $(SRC_PATH)/server_config.cpp
OBJS=$(SRC_PATH)/server.o $(SRC_PATH)/persistence.o $(SRC_PATH)/client.o $(COMMON_SRC_PATH)/machine_info-odb.o $(COMMON_SRC_PATH)/machine.pb.o $(SRC_PATH)/server_config.o

all: server

server: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o server $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
