CC=g++
GTKMM=gtkmm-2.4
CFLAGS=`pkg-config $(GTKMM) --cflags`
LDFLAGS=`pkg-config $(GTKMM) --libs`
SOURCES=$(wildcard src/*.cpp)
DEPS=$(wildcard src/*.hpp)
#SOURCES=bitboard.cpp board.cpp buffer.cpp gameinfo.cpp gui.cpp main.cpp minimax.cpp move.cpp movelist.cpp piece.cpp position.cpp window.cpp
#DEPS=bitboard.hpp board.hpp buffer.hpp gameinfo.hpp gui.hpp minimax.hpp move.hpp movelist.hpp piece.hpp position.hpp window.hpp
EXECUTABLE=chess

all: $(SOURCES) $(DEPS)
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) -o $(EXECUTABLE)
