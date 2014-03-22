#include "buffer.hpp"

namespace ChessProject {

const int Buffer::N = 10;
const int Buffer::S = -10;
const int Buffer::W = -1;
const int Buffer::E = 1;
const int Buffer::NE = Buffer::N + Buffer::E;
const int Buffer::SE = Buffer::S + Buffer::E;
const int Buffer::SW = Buffer::S + Buffer::W;
const int Buffer::NW = Buffer::N + Buffer::W;

const int Buffer::Board[BUFFERSIZE] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	-1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	-1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	-1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	-1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	-1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	-1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	-1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

const int Buffer::Coords[BOARDSIZE] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

const intlist Buffer::Offset[NUM_PIECE_TYPES] = {
	boost::assign::list_of(NE)(SE)(SW)(NW), // Bishop
	boost::assign::list_of(N)(NE)(E)(SE)(S)(SW)(W)(NW), // King
	boost::assign::list_of(N + NE)(E + NE)(E + SE)(S + SE)(S + SW)(W + SW)(W + NW)(N + NW), // Knight
	positionlist(), // Pawn
	boost::assign::list_of(N)(NE)(E)(SE)(S)(SW)(W)(NW), // Queen
	boost::assign::list_of(N)(E)(S)(W) // Rook
};

const int Buffer::PawnOffset[NUM_COLORS] = {
	N, // White
	S // Black
};

}
