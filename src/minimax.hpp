#pragma once

#include "board.hpp"
#include "gameinfo.hpp"
#include "move.hpp"
#include "movelist.hpp"

#define LARGEST_NUM 1000000
#define LARGE_NUM 100000
#define NUM_CENTER_SQUARES 4

namespace ChessProject {

class Minimax {
public:
	// Recursively evaluates board to a given depth using alpha-beta pruning.
	static int AlphaBeta(Board *board, GameInfo *gameInfo, Move &move, int depth, const int quiescenceDepth,
						 int alpha = -LARGEST_NUM,
						 int beta = LARGEST_NUM);
	// Searches through interesting moves. Only evaluates when a position is quiet, or when a certain depth has been reached.
	static int Quiescence(Board *board, GameInfo *gameInfo, int depth, int alpha, int beta);
private:
	// The positions of the center squares of the board.
	static const int CenterSquares[NUM_CENTER_SQUARES];
	// The bonus given to a team for attacking a center square.
	static const int CenterControlBonus = 20;
	// Evaluates the board. Includes the following evaluations:
	// 1 - Relative material worth + piece square bonuses.
	// 2 - Backward/Doubled/Isolated/Passed pawn evaluation.
	// 3 - Control of center squares.
	// 4 - Rooks on semi-open/open files.
	static int Eval(Board *board, GameInfo *gameInfo);
};

}
