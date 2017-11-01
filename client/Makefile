CXX=g++
INCLUDE=-I../common/include -I/usr/local/include -I./include
CXXFLAGS=--std=c++14 $(INCLUDE)
RM=rm -f
CPPFLAGS=
LDFLAGS=
LDLIBS=-L/usr/local/lib -lboost_system -pthread -lpthread -lprotobuf

CSP=../common/src
SP=./src

SRCS=$(CSP)/machine.pb.cc $(SP)/client.cpp $(SP)/server.cpp $(SP)/client_info.cpp $(CSP)/context.cpp
OBJS=$(SP)/server.o $(SP)/client_info.o $(SP)/client.o $(CSP)/machine.pb.o  $(CSP)/context.o

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
