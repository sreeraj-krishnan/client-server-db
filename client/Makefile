CXX=g++
INCLUDE=-I../common/include -I/usr/local/include -I./include
CXXFLAGS=--std=c++14 $(INCLUDE)
RM=rm -f
CPPFLAGS=
LDFLAGS=
LDLIBS=-L/usr/local/lib -lboost_system -pthread -lpthread -lprotobuf

COMMON_SRC_PATH=../common/src
SRC_PATH=./src

SRCS=$(COMMON_SRC_PATH)/machine.pb.cc $(SRC_PATH)/client.cpp $(SRC_PATH)/server.cpp $(SRC_PATH)/client_info.cpp
OBJS=$(SRC_PATH)/server.o $(SRC_PATH)/client_info.o $(SRC_PATH)/client.o $(COMMON_SRC_PATH)/machine.pb.o 

all: client

client: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o client $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS)  -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
