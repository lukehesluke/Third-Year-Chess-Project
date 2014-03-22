#pragma once

#include <set>
#include <string>

#define BOARDSIZE 64
#define NUM_FILES 8
#define NUM_RANKS 8

namespace ChessProject {

typedef int position;
typedef std::set<int> positionlist;

// Positions are represented by an integer between 0 and 63.
struct Position {
	// Returns the file (or column) of a position.
	static int File(position pos);
	// Returns the rank (or row) of a position.
	static int Rank(position pos);
	// Returns true if the position is a "light" (as opposed to dark) tile on the board.
	static bool IsLightTile(position pos);
	// The integer representation of a position with a given file and rank.
	static position ToInt(int file, int rank);
	// Returns the string representation of the position in algebraic notation  (e.g. a1, g5, d7, etc).
	static std::string ToAlgebraic(position pos);
	// Returns the position represented by a string in algebraic notation.
	static position FromAlgebraic(std::string str);
};

}
