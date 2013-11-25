CC = g++
CXXFLAGS = -std=c++11 -O1 -Wall -Wcast-align  -D_WEBSOCKETPP_CPP11_STL_ -I.
LDFLAGS = -lboost_system 

all: broadcast_server

broadcast_server:
	$(CC) $(CXXFLAGS) broadcast_server.cpp -o broadcast_server $(LDFLAGS)

clean:
	$(RM) broadcast_server
