#include "bitboard.hpp"

namespace ChessProject {

bitboard Bitboard::PassedPawnEval[NUM_COLORS][BOARDSIZE];
bitboard Bitboard::IsolatedPawnEval[BOARDSIZE];
bitboard Bitboard::File[NUM_FILES];

void Bitboard::Precalculate() {
	// Passed pawn evals and pawn attack offsets.
	for (int color = 0; color < NUM_COLORS; color++) {
		int upOffset = Buffer::PawnOffset[color];
		for (position pos = 0; pos < BOARDSIZE; pos++) {
			for (int i = -1; i <= 1; i++) {
				for (int j = 1, p = Buffer::Board[Buffer::Coords[pos] + (upOffset * j) + i];
					 p > 0; j++, p = Buffer::Board[Buffer::Coords[pos] + (upOffset * j) + i]) {
					PassedPawnEval[color][pos][p] = true;
				}
			}
		}
	}
	// Isolated pawn evals.
	for (position pos = 0; pos < BOARDSIZE; pos++) {
		for (int relFile = -1; relFile <= 1; relFile += 2) {
			for (int relRank = Buffer::S; relRank <= Buffer::N; relRank += Buffer::N) {
				position p = Buffer::Board[Buffer::Coords[pos] + relFile + relRank];
				if (p > 0) IsolatedPawnEval[pos][p] = true;
			}
		}
	}
	// File bitboards.
	for (int file = 0; file < NUM_FILES; file++) {
		for (int rank = 0; rank < NUM_RANKS; rank++) {
			File[file][Position::ToInt(file, rank)] = true;
		}
	}
}

bitboard Bitboard::GetPawnAttacks(Piece::Color color, bitboard pawns) {
	if (color == Piece::WHITE) pawns <<= NUM_FILES;
	else pawns >>= NUM_FILES;
	pawns = ((pawns & ~File[0]) >> 1) | ((pawns & ~File[NUM_FILES - 1]) << 1);
	return pawns;
}

void Bitboard::Print(bitboard bboard) {
	for (int rank = NUM_RANKS - 1; rank >= 0; rank--) {
		for (int file = 0; file < NUM_FILES; file++) {
			std::cout << bboard[Position::ToInt(file, rank)] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

}
