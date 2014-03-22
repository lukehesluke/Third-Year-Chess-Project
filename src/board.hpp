#pragma once

#include <cctype>
#include <stack>
#include <string>
#include "bitboard.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position.hpp"

namespace ChessProject {

class Board {
public:
	~Board();
	// Initialises all structures to their respective null values and sets up board.
	void init();
	// Deletes all piece references. Clears board.
	void cleanup();
	// Returns the material evaluation of the board relative to the specified color.
	// For each piece, adds the piece square table associated to its position on the board.
	int materialEval(Piece::Color color);
	// Returns true if there is insufficient material for a checkmate to occur. This includes:
	// 1 - king vs king
	// 2 - king & bishop vs king
	// 3 - king & knight vs king
	// 4 - king & bishop(s) vs king & bishop(s) if all bishops are on the same color square.
	bool insufficientMaterial() const;
	// Unfortunately, there is no strict definition for when the endgame begins in chess. This method returns
	//true if it seems to be the end game based on how many pieces remain.
	bool isEndGame() const;
	// Returns the evaluation of pawn structure relative to the specified color. This includes evaluation of:
	// 1 - Passed pawns.
	// 2 - Isolated pawns.
	// 3 - Doubled pawns.
	// 4 - Backward pawns.
	int pawnEval(Piece::Color color);
	// Creates a new piece. Places it on the board and piecelist, and modifies appropriate piece structures.
	// Does not check the position to see if overwriting a piece.
	void addPiece(Piece::Color color, Piece::Type type, position pos);
	// Performs a move with no checks.
	void move(Piece *piece, position to);
	Piece* getPiece(position pos) const;
	Piece* getKing(Piece::Color color) const;
	// Returns const iterator to the first element of the piece list for the specified color.
	PieceList::const_iterator firstPieceItr(Piece::Color color) const;
	// Returns end iterator of the piece list for the specified color.
	PieceList::const_iterator endPieceItr(Piece::Color color) const;
	// Executes a move - Moves the object piece first (if there is one), followed by the subject piece. Updates piece structures accordingly.
	void executeMove(const Move &move);
	// Reverses a move that has already been made. First, moves the subject back, followed by the object. Unpromotes if necessary.
	void reverseMove(const Move &move);
	void print() const;
private:
	// The board is represented by a 64 length array of Piece pointers. A null pointer means an empty square.
	Piece *internalBoard[BOARDSIZE];
	// The pieces are also held in this list, so that they can be iterated over without checking all 64 squares.
	PieceList pieceList[NUM_COLORS];
	// This is a list of all of the pieces that have been captured. It is useful to keep the pieces that have been captured in order to
	//take back a capture.
	PieceList captured;
	// The pawn structure for each color is stored as a 64-bit word with 1s as pawns and 0s as not pawns. Storing pawn structure in this
	//way helps speed up pawn evaluations.
	bitboard pawnStructure[NUM_COLORS];
	// It is often useful to directly access the kings of the board in order to check for check, checkmate and such.
	Piece *king[NUM_COLORS];
};

}
