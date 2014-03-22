#include "piece.hpp"

namespace ChessProject {

const char Piece::Ascii[NUM_COLORS][NUM_PIECE_TYPES] = {
	{ 'B', 'K', 'N', 'P', 'Q', 'R' }, // White
	{ 'b', 'k', 'n', 'p', 'q', 'r' } // Black
};

const std::string Piece::ColorString[NUM_COLORS] = {
	"WHITE",
	"BLACK"
};

const std::string Piece::TypeString[NUM_PIECE_TYPES] = {
	"BISHOP",
	"KING",
	"KNIGHT",
	"PAWN",
	"QUEEN",
	"ROOK"
};

const bool Piece::CanSlide[NUM_PIECE_TYPES] = {
	true, // Bishop
	false, // King
	false, // Knight
	false, // Pawn
	true, // Queen
	true // Rook
};

const int Piece::MaterialWorth[NUM_PIECE_TYPES] = {
	300, // Bishop
	10000, // King - arbitrarily large
	300, // Knight
	100, // Pawn
	900, // Queen
	500 // Rook
};
const int Piece::BonusTable[NUM_PIECE_TYPES][BOARDSIZE] = {
	// In order to encourage piece development near the beginning of the game, almost every piece table penalizes pieces in their starting positions.
	{ // Bishop - Centre squares are better due to increased mobility.
		 -5,  -5, -10,  -5,  -5, -10,  -5,  -5,
		 -5,   0,   0,   0,   0,   0,   0,  -5,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		 -5,   0,   5,  10,  10,   5,   0,  -5,
		 -5,   0,   5,  10,  10,   5,   0,  -5,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		 -5,   0,   0,   0,   0,   0,   0,  -5,
		 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5
	},
	{ // King - Castling is encouraged. Staying at the home front is greatly encouraged.
		  5,   5,  10,   0,  -5,   0,  10,   5,
		-10, -10, -10, -10, -10, -10, -10, -10,
		-20, -20, -20, -20, -20, -20, -20, -20,
		-20, -20, -20, -20, -20, -20, -20, -20,
		-20, -20, -20, -20, -20, -20, -20, -20,
		-20, -20, -20, -20, -20, -20, -20, -20,
		-20, -20, -20, -20, -20, -20, -20, -20,
		-20, -20, -20, -20, -20, -20, -20, -20
	},
	{ // Knight - Knights are encouraged to leave their starting positions very early. Otherwise, control of the center and mobility are most important.
		 -5, -10,  -5,  -5,  -5,  -5, -10,  -5,
		 -5,   0,   0,   0,   0,   0,   0,  -5,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		 -5,   0,   5,  10,  10,   5,   0,  -5,
		 -5,   0,   5,  10,  10,   5,   0,  -5,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		 -5,   0,   0,   0,   0,   0,   0,  -5,
		 -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5
	},
	{ // Pawn - Pawns are encouraged to promote, and allow the bishop/queen to leave their starting positions.
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,  -5,  -5,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		 40,  40,  40,  40,  40,  40,  40,  40,
		  0,   0,   0,   0,   0,   0,   0,   0
	},
	{ // Queen
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0
	},
	{ // Rook - Rooks are encouraged to occupy the seventh rank. Otherwise, any position is good.
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,
		 20,  20,  20,  20,  20,  20,  20,  20,
		  0,   0,   0,   0,   0,   0,   0,   0
	}
};

const int Piece::FlippedBoard[BOARDSIZE] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	 8,  9, 10, 11, 12, 13, 14, 15,
	 0,  1,  2,  3,  4,  5,  6,  7
};

const int Piece::GetPositionBonus(Color color, Type type, position pos) {
	if (color == Piece::WHITE) return BonusTable[type][pos];
	// Flip the position for the black player.
	return BonusTable[type][FlippedBoard[pos]];
}

const positionlist Piece::InitialSetup[NUM_COLORS][NUM_PIECE_TYPES] = {
	{ // White
		boost::assign::list_of(2)(5), // Bishop
		boost::assign::list_of(4), // King
		boost::assign::list_of(1)(6), // Knight
		boost::assign::list_of(8)(9)(10)(11)(12)(13)(14)(15), // Pawn
		boost::assign::list_of(3), // Queen
		boost::assign::list_of(0)(7) // Rook
	},
	{ // Black
		boost::assign::list_of(58)(61), // Bishop
		boost::assign::list_of(60), // King
		boost::assign::list_of(57)(62), // Knight
		boost::assign::list_of(48)(49)(50)(51)(52)(53)(54)(55), // Pawn
		boost::assign::list_of(59), // Queen
		boost::assign::list_of(56)(63) // Rook
	}
};

const int Piece::PawnPromotionRank[NUM_COLORS] = {
	NUM_RANKS - 2, // White
	1 // Black
};

Piece::Piece(Color color, Type type, position pos) :
	color(color),
	type(type),
	pos(pos) { }

}
