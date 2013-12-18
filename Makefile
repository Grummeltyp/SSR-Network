CXXFLAGS = -std=c++11 -O1 -W -Wall -pedantic -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls -Wconversion\
-Wno-unused-parameter -Wno-ignored-qualifiers -Wno-long-long -Wno-conversion
CPPFLAGS = -D_WEBSOCKETPP_CPP11_STL_ -Iincludes
LDFLAGS += -Llibs -L/opt/local/lib -ljson_linux-gcc-4.8.2_libmt -lboost_system-mt

all: subscribe_server

clean:
	$(RM) subscribe_server
