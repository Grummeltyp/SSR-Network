CXXFLAGS = -std=c++11 -O1 -Wall -Wcast-align  
CPPFLAGS = -D_WEBSOCKETPP_CPP11_STL_ -I.
LDLIBS = -lboost_system

all: broadcast_server

clean:
	$(RM) broadcast_server
