#include "minimax.hpp"

namespace ChessProject {

const int Minimax::CenterSquares[NUM_CENTER_SQUARES] = { 27, 28, 35, 36 };

int Minimax::AlphaBeta(Board *board, GameInfo *gameInfo, Move &move, int depth, const int quiescenceDepth, int alpha, int beta) {
	// If we are at the end of the normal alpha beta search, perform a quiescence search with the given quiescence depth.
	if (depth == 0) return Quiescence(board, gameInfo, quiescenceDepth, alpha, beta);
	MoveList moveList;
	// Generate move list and check game state.
	GameInfo::State state = gameInfo->updateState(board, moveList);
	switch (state) {
		case GameInfo::STALEMATE:
		case GameInfo::FIFTY_MOVE_RULE:
		// Returns a score which will always be disregarded.
		return LARGEST_NUM + 1;
		case GameInfo::CHECKMATE:
		// Return arbitrarily large negative score (But not -LARGEST_NUM, as it would be cut off).
		return -LARGE_NUM;
	}
	MoveList::iterator bestMoveItr = moveList.begin();
	for (MoveList::iterator moveItr = moveList.begin(); moveItr != moveList.end(); moveItr++) {
		Move childMove = *moveItr;
		board->executeMove(childMove);
		// Save any irreversible game info before making a move.
		GameInfo::Irreversible irreversible(gameInfo);
		gameInfo->executeMove(childMove);
		// Here, the child's evaluation is taken to be the negation of its return value, so that seperate if statements for maximising and minimising
		//aren't required.
		int childEval = -AlphaBeta(board, gameInfo, move, depth - 1, quiescenceDepth, -beta, -alpha);
		board->reverseMove(childMove);
		gameInfo->reverseMove(irreversible);
		// If the child evaluates to a score greater than or equal to beta, there is a beta cutoff. This means that there is no further point exploring this
		// node's moves, as it is known that this node will at least be as bad if not worse than another node elsewhere in the game tree.
		if (childEval >= beta) {
			// If a non-capture move caused a beta-cutoff, increase its history weighting. The depth squared is added to the heuristic so that moves near
			//the leaf nodes don't dominate the heuristic (Leaf node score would be 0 * 0).
			if (!move.isCapture()) Move::HistoryHeuristic[move.subject_from][move.subject_to] += depth * depth;
			move = *bestMoveItr;
			return beta;
		}
		// If the child's evaluation is greater than alpha, this is the best move at the moment.
		if (childEval > alpha) {
			alpha = childEval;
			bestMoveItr = moveItr;
		}
	}
	move = *bestMoveItr;
	return alpha;
}

int Minimax::Quiescence(Board *board, GameInfo *gameInfo, int depth, int alpha, int beta) {
	// Evaluate the node in its current state. If it causes a beta-cutoff, assume that there will be no move further down the
	//game tree that will result in a better evaluation. Otherwise, set it as the lower bound, alpha.
	int nodeEvaluation = Eval(board, gameInfo);
	if (nodeEvaluation >= beta) return beta;
	if (nodeEvaluation > alpha) alpha = nodeEvaluation;
	MoveList moveList;
	// If the node is in check, consider every move. Otherwise, just consider captures/promotions.
	moveList.generate(gameInfo->turn, board, gameInfo, !MoveList::InCheck(gameInfo->turn, board));
	moveList.prune((Piece::Color)!gameInfo->turn, board);
	for (MoveList::iterator moveItr = moveList.begin(); moveItr != moveList.end(); moveItr++) {
		Move move = *moveItr;
		board->executeMove(move);
		// Save any irreversible game info before making a move.
		GameInfo::Irreversible irreversible(gameInfo);
		gameInfo->executeMove(move);
		int childEval = 0;
		// If we are at depth 0, just get the score of the board (The score is negated as it is measured relative to the opposite color).
		if (depth == 0) childEval = -Eval(board, gameInfo);
		else childEval = -Quiescence(board, gameInfo, depth - 1, -beta, -alpha);
		board->reverseMove(move);
		gameInfo->reverseMove(irreversible);
		if (childEval >= beta) return beta;
		if (childEval > alpha) alpha = childEval;
	}
	return alpha;
}

int Minimax::Eval(Board *board, GameInfo *gameInfo) {
	// If the game cannot be won from this board, return a dismissable score.
	if (board->isEndGame() && board->insufficientMaterial()) return LARGEST_NUM + 1;
	int eval = 0;
	// Evaluate number of pieces left on each side.
	eval += board->materialEval(gameInfo->turn);
	// Pawn structure evaluations.
	eval += board->pawnEval(gameInfo->turn);
	// Add a bonus to a side for being able to attack the center squares.
	for (int i = 0; i < NUM_CENTER_SQUARES; i++) {
		if (MoveList::UnderThreat(CenterSquares[i], gameInfo->turn, board)) eval += CenterControlBonus;
		if (MoveList::UnderThreat(CenterSquares[i], (Piece::Color)!gameInfo->turn, board)) eval -= CenterControlBonus;
	}
	return eval;
}

}
