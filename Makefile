CXXFLAGS = -std=c++11 -O1 -W -Wall -pedantic -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Wconversion\
-Wno-unused-parameter -Wno-ignored-qualifiers -Wno-long-long -Wno-conversion
CPPFLAGS = -D_WEBSOCKETPP_CPP11_STL_ -I.
LDLIBS = -lboost_system

all: subscribe_server

clean:
	$(RM) subscribe_server
