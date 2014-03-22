#pragma once

#include <set>
#include "board.hpp"
#include "buffer.hpp"
#include "move.hpp"
#include "piece.hpp"

namespace ChessProject {

struct GameInfo;

class MoveList : public std::multiset<Move, Move::Evaluator> {
public:
	// Generates all pseudo-legal moves the specified color can make. If onlyInteresting is true, only generates captures and promotions.
	void generate(Piece::Color color, Board *board, GameInfo *info, bool onlyInteresting = false);
	// Generate pseudo-legal moves for one piece.
	void generate(Piece *piece, Board *board, GameInfo *info, bool onlyInteresting = false);
	// Convenience method for human moves. If the position does not match a piece on the board of the color whose turn it is, returns false.
	bool generateForHuman(position pos, Board *board, GameInfo *info);
	// Erases all moves that leave the king in check.
	void prune(Piece::Color enemies, Board *board);
	// If the move list contains this move, returns its iterator, otherwise returns end().
	const_iterator getMove(position from, position to) const;
	// Returns true if a given position is under threat from its enemy team.
	static bool UnderThreat(position pos, Piece::Color enemies, Board *board);
	// Returns true if the specified color is in check.
	static bool InCheck(Piece::Color color, Board *board);
private:
	// Generates psuedo-legal moves for a pawn.
	void genPawnMoves(Piece *pawn, Board *board, GameInfo *info, bool onlyInteresting = false);
	// Generates pawn promotion moves for each of the piece types a pawn can promote to for a given move.
	void genPawnPromotions(Move move);
};

}
