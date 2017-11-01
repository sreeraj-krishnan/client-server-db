CC=gcc
CXX=g++
INCLUDE=-I../common/include -I/usr/local/include -I./include
CXXFLAGS=--std=c++14 $(INCLUDE)
RM=rm -f
CPPFLAGS=
LDFLAGS=
LDLIBS=-L/usr/local/lib -lboost_system -pthread -lpthread  -lprotobuf -lodb -lodb-pgsql

CSP=../common/src
SP=./src

SRCS=$(CSP)/machine_info-odb.cpp $(CSP)/machine.pb.cc $(SP)/client.cpp $(SP)/server.cpp $(SP)/persistence.cpp $(SP)/server_config.cpp $(CSP)/context.cpp
OBJS=$(SP)/server.o $(SP)/persistence.o $(SP)/client.o $(CSP)/machine_info-odb.o $(CSP)/machine.pb.o $(SP)/server_config.o $(CSP)/context.o

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
