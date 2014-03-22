#pragma once

#include <set>
#include <boost/assign.hpp>
#include "position.hpp"

#define NUM_COLORS 2
#define NUM_PIECE_TYPES 6

namespace ChessProject {

class Piece {
public:
	enum Color {
		WHITE = 0,
		BLACK = 1
	};
	enum Type {
		BISHOP = 0,
		KING = 1,
		KNIGHT = 2,
		PAWN = 3,
		QUEEN = 4,
		ROOK = 5,
		NONE = -1
	};
	// Array of ascii characters that represent each type of piece.
	static const char Ascii[NUM_COLORS][NUM_PIECE_TYPES];
	static const std::string ColorString[NUM_COLORS];
	static const std::string TypeString[NUM_PIECE_TYPES];
	// This array represents whether or not a piece can slide.
	static const bool CanSlide[NUM_PIECE_TYPES];
	// The weighting applied to every bit of material on the board for evaluation.
	static const int MaterialWorth[NUM_PIECE_TYPES];
	static const int GetPositionBonus(Color color, Type type, position pos);
	// Pawn structure evaluation weightings.
	static const int DoubledPawnPenalty = -10;
	static const int IsolatedPawnPenalty = -20;
	static const int PassedPawnBonus = 20;
	static const int EndGamePassedPawnBonus = 100;
	static const int BackwardPawnPenalty = -10;
	static const int BothBishopsBonus = 20;
	// Rook evaluation weightings.
	static const int RookOnOpenFile = 10;
	static const int RookOnSeventhRank = 10;
	// The board's initial set up.
	static const positionlist InitialSetup[NUM_COLORS][NUM_PIECE_TYPES];
	// The rank a pawn needs to advance from in order to promote.
	static const int PawnPromotionRank[NUM_COLORS];
	Color color;
	Type type;
	position pos;
	Piece(Color color, Type type, position pos);
private:
	// The values in the piece table for each piece is added to its material worth in the evaluation function.
	static const int BonusTable[NUM_PIECE_TYPES][BOARDSIZE];
	// The flipped board provides the coordinates required for the black player to look up their square table value.
	static const int FlippedBoard[BOARDSIZE];
};

typedef std::set<Piece*> PieceList;

}
