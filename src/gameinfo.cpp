#include "gameinfo.hpp"

namespace ChessProject {

GameInfo::Irreversible::Irreversible(GameInfo *info) :
	castlingUnavailability(info->castlingUnavailability),
	fiftyMoveRuleCounter(info->fiftyMoveRuleCounter),
	enPassantTarget(info->enPassantTarget),
	inCheck(info->inCheck) { }

const int GameInfo::CastlingMask[BOARDSIZE] = {
	CastlingFlag[Piece::WHITE][QUEENSIDE], 0, 0, 0, CastlingFlag[Piece::WHITE][QUEENSIDE] | CastlingFlag[Piece::WHITE][KINGSIDE], 0, 0, CastlingFlag[Piece::WHITE][KINGSIDE],
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	0									 , 0, 0, 0, 0																			, 0, 0, 0									,
	CastlingFlag[Piece::BLACK][QUEENSIDE], 0, 0, 0, CastlingFlag[Piece::BLACK][QUEENSIDE] | CastlingFlag[Piece::BLACK][KINGSIDE], 0, 0, CastlingFlag[Piece::BLACK][KINGSIDE]
};

const int GameInfo::CastlingFlag[NUM_COLORS][NUM_SIDES] = {
	{ 1 << 0, 1 << 1 }, // WHITE
	{ 1 << 2, 1 << 3 } // BLACK
};

void GameInfo::init() {
	castlingUnavailability = 0;
	numTurns = 0;
	fiftyMoveRuleCounter = 0;
	turn = Piece::WHITE;
	enPassantTarget = 0;
	inCheck = false;
}

bool GameInfo::canCastle(Piece::Color color, Side side, Board *board) {
	// Castling cannot occur whilst the king is in check.
	if (inCheck || (castlingUnavailability & CastlingFlag[color][side])) return false;
	position kingPos = (color == Piece::WHITE ? 4 : 60);
	// Check all positions between the king and rook. If they are empty, castling can occur.
	if (side == KINGSIDE) {
		return !board->getPiece(kingPos + 1) && !board->getPiece(kingPos + 2);
	} else {
		return !board->getPiece(kingPos - 1) && !board->getPiece(kingPos - 2) && !board->getPiece(kingPos - 3);
	}
}

void GameInfo::executeMove(const Move &move) {
	// Update castling unavailability.
	castlingUnavailability |= CastlingMask[move.subject_from];
	castlingUnavailability |= CastlingMask[move.subject_to];
	// Clear en passant target.
	enPassantTarget = 0;
	if (move.type == Move::PAWN_DOUBLE_ADVANCE) {
		enPassantTarget = move.subject;
	}
	// Reset fifty move rule counter if a pawn moved or a capture was made.
	if (move.subject->type == Piece::PAWN || move.isCapture()) {
		fiftyMoveRuleCounter = -1;
	}
	// Increment turn.
	fiftyMoveRuleCounter++;
	turn = (Piece::Color)!turn;
	if (turn == Piece::WHITE) numTurns++;
	inCheck = false;
}

void GameInfo::reverseMove(const Irreversible &irreversible) {
	this->castlingUnavailability = irreversible.castlingUnavailability;
	this->fiftyMoveRuleCounter = irreversible.fiftyMoveRuleCounter;
	this->enPassantTarget = irreversible.enPassantTarget;
	this->inCheck = irreversible.inCheck;
	// Decrement turn.
	if (turn == Piece::WHITE) numTurns--;
	turn = (Piece::Color)!turn;
}

GameInfo::State GameInfo::updateState(Board *board, MoveList &moveList) {
	moveList.clear();
	if (board->insufficientMaterial()) return STALEMATE;
	// If fifty full moves have passed since the last pawn advance / capture, end game.
	if (fiftyMoveRuleCounter >= 100) return FIFTY_MOVE_RULE;
	State state = NORMAL;
	// If king is under threat, we are in check.
	if (MoveList::InCheck(turn, board)) inCheck = true;
	moveList.generate(turn, board, this);
	moveList.prune((Piece::Color)!turn, board);
	// If there are no moves available, the game is either won or drawn.
	if (moveList.empty()) {
		if (inCheck) state = CHECKMATE;
		else state = STALEMATE;
	}
	return state;
}

}
