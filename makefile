CC = g++
CXXFLAGS = -c -std=c++0x -O1 -Wall -Wcast-align -lboost_system -D_WEBSOCKETPP_CPP11_STL_ -I.

all:
	$(CC) $(CXXFLAGS) broadcast_server.cpp
