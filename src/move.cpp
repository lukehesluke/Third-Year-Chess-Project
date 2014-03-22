#include "move.hpp"

namespace ChessProject {

bool Move::Evaluator::operator()(const Move &move1, const Move &move2) const {
	return move1.weakEval > move2.weakEval;
}

int Move::HistoryHeuristic[BOARDSIZE][BOARDSIZE];

void Move::InitHistory() {
	for (int i = 0; i < BOARDSIZE; i++) {
		for (int j = 0; j < BOARDSIZE; j++) {
			HistoryHeuristic[i][j] = 0;
		}
	}
}

Move::Move() :
	subject(0),
	subject_from(-1),
	subject_to(-1),
	object(0),
	object_from(-1),
	object_to(-1),
	type(Move::NORMAL),
	weakEval(0) { }

Move::Move(Piece *subject, position subject_from, position subject_to,
		   Piece *object, position object_from, position object_to,
		   Type type) :
	subject(subject),
	subject_from(subject_from),
	subject_to(subject_to),
	object(object),
	object_from(object_from),
	object_to(object_to),
	type(type) {
	// Determine the evaluation for this move used for move ordering.
	// Add the position bonus for the piece being moved.
	weakEval = Piece::GetPositionBonus(subject->color, subject->type, subject_to) - Piece::GetPositionBonus(subject->color, subject->type, subject_from);
	if (type >= PROMOTION) {
		// Add a bonus equal to the worth of the piece being promoted to.
		weakEval += Piece::MaterialWorth[type - PROMOTION];
	} else if (object && object->color != subject->color) {
		// Add a bonus equal to the worth of the piece being captured. If the subject is worth less than the piece it is capturing, add a small bonus.
		weakEval += Piece::MaterialWorth[object->type] + std::max(0, Piece::MaterialWorth[object->type] - Piece::MaterialWorth[subject->type]);
	} else {
		// If this is a non-interesting move, order it using the history heuristic.
		weakEval -= HistoryHeuristic[subject_from][subject_to];
	}
}

bool Move::isCapture() const {
	return (object && subject->color != object->color);
}

std::string Move::toAlgebraic() const {
	return Position::ToAlgebraic(subject_from) + Position::ToAlgebraic(subject_to);
}

}
