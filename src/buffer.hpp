#pragma once

#include <set>
#include <boost/assign.hpp>
#include "piece.hpp"
#include "position.hpp"

#define BUFFERSIZE 120

namespace ChessProject {

typedef std::set<int> intlist;

// The buffer class is used for dealing with the buffer board - a 120-length array that represents an 8 x 8 chess board surrounded by a buffer zone
//that allows for detection of moves that will take a piece off the board. Whilst moves are being generated, each piece will check their position
//plus a certain offset to see if it is on the board using something like "int newPosition = Buffer::Board[Buffer::Coords[currentPosition] + directionOffset]".
// A negative position indicates one that is off the board.
struct Buffer {
	// These direction constants represent the position offset for a move along one tile.
	static const int N;
	static const int S;
	static const int W;
	static const int E;
	static const int NE;
	static const int SE;
	static const int SW;
	static const int NW;
	// The 120-length buffer board.
	static const position Board[BUFFERSIZE];
	// This array is used to translate a position on the 8 x 8 chess board onto the buffer board.
	static const int Coords[BOARDSIZE];
	// Array of direction offsets for each individual piece type.
	static const intlist Offset[NUM_PIECE_TYPES];
	// Array of offsets for pawn advance.
	static const int PawnOffset[NUM_COLORS];
};

}
