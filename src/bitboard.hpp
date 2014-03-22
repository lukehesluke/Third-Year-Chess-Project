#pragma once

#include <bitset>
#include <iostream>
#include "buffer.hpp"
#include "piece.hpp"
#include "position.hpp"

namespace ChessProject {

typedef std::bitset<BOARDSIZE> bitboard;

struct Bitboard {
	// A precalculated bitboard that sets all the bits that need to be unoccupied
	//by an enemy pawn in order for a pawn at a given position to be "passed."
	static bitboard PassedPawnEval[NUM_COLORS][BOARDSIZE];
	// This bitboard sets all the bits that need to be unoccupied by an allied pawn
	//in order for a pawn at a given position to be "isolated."
	static bitboard IsolatedPawnEval[BOARDSIZE];
	// This is a bitboard with all the bits on a given file set.
	// Used for doubled pawn evaluation.
	static bitboard File[NUM_FILES];

	// Precalculates static bitboards. Must be executed before any pawn evaluation.
	static void Precalculate();
	// Returns a bitboard with spaces that can be attacked by a pawn set.
	static bitboard GetPawnAttacks(Piece::Color color, bitboard pawns);
	static void Print(bitboard bboard);
};

}
