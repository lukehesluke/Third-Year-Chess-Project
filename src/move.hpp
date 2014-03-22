#pragma once

#include <string>
#include "piece.hpp"
#include "position.hpp"

#define NUM_MOVE_TYPES 6
#define NUM_SIDES 2

namespace ChessProject {

struct Move {
	struct Evaluator {
		// Returns the move with the best weak evaluation.
		bool operator()(const Move &move1, const Move &move2) const;
	};
	// Represents a type of move that requires special attention from the game objects.
	enum Type {
		NORMAL = 0,
		PAWN_DOUBLE_ADVANCE,
		PROMOTION,
		PROMOTION_BISHOP = PROMOTION + Piece::BISHOP,
		PROMOTION_KNIGHT = PROMOTION + Piece::KNIGHT,
		PROMOTION_QUEEN = PROMOTION + Piece::QUEEN,
		PROMOTION_ROOK = PROMOTION + Piece::ROOK
	};
	// The history heuristic gives a penalty for a move in ordering if a move with the same source and destination has already been subject to beta cutoffs
	//during alpha beta pruning.
	static int HistoryHeuristic[BOARDSIZE][BOARDSIZE];
	// Zero initializes history heuristic array.
	static void InitHistory();
	// The piece moved is the subject.
	Piece *subject;
	position subject_from, subject_to;
	// The object piece is any secondary piece that is being moved as a result of this move. This is used for two instances:
	// 1 - If a piece was being captured (or en passanted), it would be the object being moved to a negative position.
	// 2 - In the event of a castling, the object is the rook.
	// If there is no object, it is set to null.
	Piece *object;
	position object_from, object_to;
	Type type;
	// This is a weak evaluation of the move, such that moves can be ordered with better moves first. This improves the efficiency of alpha-beta pruning.
	int weakEval;
	Move();
	Move(Piece *subject, position subject_from, position subject_to,
		 Piece *object, position object_from, position object_to,
		 Type type);
	// Returns true if this move is a capture.
	bool isCapture() const;
	// Returns the string representation of the move in algebraic notation.
	std::string toAlgebraic() const;
};

}