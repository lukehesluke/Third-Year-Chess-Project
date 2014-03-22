#include "position.hpp"

namespace ChessProject {

int Position::File(position pos) {
	return pos & 7;
}

int Position::Rank(position pos) {
	return pos >> 3;
}

position Position::ToInt(int file, int rank) {
	return (rank << 3) | file;
}

bool Position::IsLightTile(position pos) {
	return ((File(pos) & 1) + (Rank(pos) & 1)) & 1;
}

std::string Position::ToAlgebraic(position pos) {
	std::string algebraic;
	algebraic.push_back('a' + File(pos));
	algebraic.push_back('1' + Rank(pos));
	return algebraic;
}

position Position::FromAlgebraic(std::string str) {
	if (str.size() != 2) return -1;
	char file = str[0] - 'a';
	char rank = str[1] - '1';
	if (file < 0 || file >= NUM_FILES || rank < 0 || rank >= NUM_RANKS) return -1;
	return ToInt(file, rank);
}

}
