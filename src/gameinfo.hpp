#pragma once

#include <bitset>
#include "board.hpp"
#include "move.hpp"
#include "movelist.hpp"
#include "piece.hpp"
#include "position.hpp"

namespace ChessProject {

// The game info class holds all important information about a game in its current state that is not held by the board class.
struct GameInfo {
	// Used for castling. Kingside represents castling to the right. Queenside represents castling to the left.
	enum Side {
		KINGSIDE = 0,
		QUEENSIDE
	};
	// Information about whether or not a game is complete.
	enum State {
		NORMAL,
		CHECKMATE,
		STALEMATE,
		FIFTY_MOVE_RULE
	};
	// There is some information about the game state that can not be taken back, once a move has been executed. In order to maintain consistency
	//when taking back a move, some information has to be saved prior to executing a move that will later be reversed (for example in the game tree).
	struct Irreversible {
		int castlingUnavailability;
		int fiftyMoveRuleCounter;
		Piece *enPassantTarget;
		bool inCheck;
		Irreversible(GameInfo *info);
	};
	// The castling mask is an 8 x 8 array representing positions on the board where castling availability is lost.
	static const int CastlingMask[BOARDSIZE];
	static const int CastlingFlag[NUM_COLORS][NUM_SIDES];
	// Castling availability is represented by 4 bits. The bits are encoded as such:
	// 1st - White kingside
	// 2nd - White queenside
	// 3rd - Black kingside
	// 4th - Black queenside
	// A 0 means that castling is available, and a 1 means that it is not.
	int castlingUnavailability;
	// Number of full turns since the game has started. A full turn is a white move followed by a black move.
	int numTurns;
	// The fifty move rule states that if fifty full moves have passed since the last pawn advance or capture move, the game is considered drawn.
	// This member stores the number of plies since the last pawn advance/capture. The game is drawn when this number reaches 100.
	int fiftyMoveRuleCounter;
	// The color whose turn it is to move.
	Piece::Color turn;
	// If a pawn performs a double advance, it can be captured en passant. This member points to the piece that is now the en passant target.
	// If no piece is the en passant target, this is set to null.
	Piece *enPassantTarget;
	// Whether or not the game is in check.
	bool inCheck;
	void init();
	// Returns true if the specified color can castle the specified side.
	bool canCastle(Piece::Color color, Side side, Board *board);
	// Update game info corresponding to this move and increment turn.
	void executeMove(const Move &move);
	// Take back a move. This method also requires extra parameters for members that cannot be restored.
	void reverseMove(const Irreversible &irreversible);
	// Updates the game state including whether or not the game has ended. Returns pruned move list for color now in play.
	State updateState(Board *board, MoveList &moveList);
};

}
