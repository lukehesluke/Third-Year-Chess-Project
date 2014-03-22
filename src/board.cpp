#include "board.hpp"

namespace ChessProject {

Board::~Board() {
	cleanup();
}

void Board::init() {
	cleanup();
	// Set up board to starting positions.
	for (int color = 0; color < NUM_COLORS; color++) {
		for (int type = 0; type < NUM_PIECE_TYPES; type++) {
			for (positionlist::iterator it = Piece::InitialSetup[color][type].begin();
				 it != Piece::InitialSetup[color][type].end(); it++) {
				addPiece((Piece::Color)color, (Piece::Type)type, *it);
			}
		}
	}
}

void Board::cleanup() {
	// Initialise board to null values.
	for (position pos = 0; pos < BOARDSIZE; pos++)
		internalBoard[pos] = 0;
	for (int color = 0; color < NUM_COLORS; color++) {
		king[color] = 0;
		pawnStructure[color].reset();
		// Free piece pointers.
		for (PieceList::iterator pieceItr = pieceList[color].begin(); pieceItr != pieceList[color].end(); pieceItr++) {
			delete *pieceItr;
		}
		pieceList[color].clear();
	}
	// Free captured pieces.
	for (PieceList::iterator pieceItr = captured.begin(); pieceItr != captured.end(); pieceItr++) {
		delete *pieceItr;
	}
	captured.clear();
}

int Board::materialEval(Piece::Color color) {
	int eval = 0;
	for (int colorIt = 0; colorIt < NUM_COLORS; colorIt++) {
		int colorWeighting = color == colorIt ? 1 : -1;
		int numBishops = 0;
		for (PieceList::iterator pieceItr = pieceList[colorIt].begin(); pieceItr != pieceList[colorIt].end(); pieceItr++) {
			Piece *piece = *pieceItr;
			eval += Piece::MaterialWorth[piece->type] * colorWeighting;
			// The king's piece table is a cautious one. If the board is in end game, disregard it.
			if (piece->type != Piece::KING || !isEndGame()) {
				eval += Piece::GetPositionBonus((Piece::Color)colorIt, piece->type, piece->pos) * colorWeighting;
			}
			// Add bonus for rook on an open or semi-open file.
			if (piece->type == Piece::ROOK) {
				if ((pawnStructure[colorIt] & Bitboard::File[Position::File(piece->pos)]).none())
					eval += Piece::RookOnOpenFile * colorWeighting;
			} else if (piece->type == Piece::BISHOP) {
				numBishops++;
			}
		}
		// Apply a bonus for having both (or more) bishops.
		if (numBishops > 1) eval += Piece::BothBishopsBonus * colorWeighting;
	}
	return eval;
}

bool Board::insufficientMaterial() const {
	bool existsKnight = false;
	bool existsLightBishop = false;
	bool existsDarkBishop = false;
	for (int colorIt = 0; colorIt < NUM_COLORS; colorIt++) {
		for (PieceList::iterator pieceItr = pieceList[colorIt].begin(); pieceItr != pieceList[colorIt].end(); pieceItr++) {
			Piece *piece = *pieceItr;
			if (piece->type == Piece::PAWN || piece->type == Piece::QUEEN || piece->type == Piece::ROOK) return false;
			if (piece->type == Piece::KNIGHT) {
				// A knight is only considered insufficient material if it is the only piece that isn't a king.
				if (existsKnight || existsLightBishop || existsDarkBishop) return false;
				else existsKnight = true;
			} else if (piece->type == Piece::BISHOP) {
				if (Position::IsLightTile(piece->pos)) existsLightBishop = true;
				else existsDarkBishop = true;
				// If there are bishops on both even and odd squares, there is not insufficient material. A bishop and a knight
				//or any non-minor piece is also not draw-worthy.
				if ((existsLightBishop && existsDarkBishop) || existsKnight) return false;
			}
		}
	}
	return true;
}

bool Board::isEndGame() const {
	// If there are 12 pieces or less, return true.
	return (pieceList[Piece::WHITE].size() + pieceList[Piece::BLACK].size()) <= 12;
}

int Board::pawnEval(Piece::Color color) {
	int passed = 0;
	int isolated = 0;
	int doubled = 0;
	int backward = 0;
	for (int colorIt = 0; colorIt < NUM_COLORS; colorIt++) {
		int colorWeighting = (color == colorIt ? 1 : -1);
		// Calculate number of backward pawns. If the space in front of a pawn is being attacked by an enemy pawn and not being defended by an ally pawn,
		//the pawn is considered to be a backward pawn as it cannot advance without sacrificing itself.
		bitboard advance = pawnStructure[colorIt];
		if (colorIt == Piece::WHITE) advance <<= NUM_FILES;
		else advance >>= NUM_FILES;
		// Set backwardsPawns to those spaces ahead of pawns of this color that are being attacked by enemy pawns, but not by allied pawns.
		bitboard backwardsPawns = advance & Bitboard::GetPawnAttacks((Piece::Color)!colorIt, pawnStructure[!colorIt]);
		backwardsPawns &= ~Bitboard::GetPawnAttacks((Piece::Color)colorIt, pawnStructure[colorIt]);
		backward += colorWeighting * backwardsPawns.count();
		// Iterate over every pawn, determining whether or not they are isolated or passed.
		for (PieceList::iterator pieceItr = pieceList[colorIt].begin(); pieceItr != pieceList[colorIt].end(); pieceItr++) {
			Piece *piece  = *pieceItr;
			if (piece->type != Piece::PAWN) continue;
			if ((Bitboard::PassedPawnEval[colorIt][piece->pos] & pawnStructure[!colorIt]).none()) passed += colorWeighting;
			if ((Bitboard::IsolatedPawnEval[piece->pos] & pawnStructure[colorIt]).none()) isolated += colorWeighting;
		}
		// Iterate over every file, determining the number of doubled pawns per file.
		for (int file = 0; file < NUM_FILES; file++) {
			int numPawns = (Bitboard::File[file] & pawnStructure[colorIt]).count();
			if (numPawns > 0) doubled += (numPawns - 1) * colorWeighting;
		}
	}
	// At the end game, a passed pawn is considered much more powerful.
	if (isEndGame()) passed *= Piece::EndGamePassedPawnBonus;
	else passed *= Piece::PassedPawnBonus;
	isolated *= Piece::IsolatedPawnPenalty;
	doubled *= Piece::DoubledPawnPenalty;
	backward *= Piece::BackwardPawnPenalty;
	return passed + isolated + doubled + backward;
}

void Board::addPiece(Piece::Color color, Piece::Type type, position pos) {
	Piece *piece = new Piece(color, type, pos);
	// Insert into various structures.
	pieceList[color].insert(piece);
	internalBoard[pos] = piece;
	if (type == Piece::PAWN) pawnStructure[color][pos] = true;
	else if (type == Piece::KING) king[color] = piece;
}

void Board::move(Piece *piece, position to) {
	// If the object is coming out of capture.
	if (piece->pos < 0) {
		captured.erase(piece);
		pieceList[piece->color].insert(piece);
	} else {
		pawnStructure[piece->color][piece->pos] = false;
		internalBoard[piece->pos] = 0;
	}
	piece->pos = to;
	// If the object is being captured.
	if (to < 0) {
		pieceList[piece->color].erase(piece);
		captured.insert(piece);
	} else {
		internalBoard[to] = piece;
		if (piece->type == Piece::PAWN) pawnStructure[piece->color][piece->pos] = true;
	}
}

Piece* Board::getPiece(position pos) const {
	return internalBoard[pos];
}

Piece* Board::getKing(Piece::Color color) const {
	return king[color];
}

PieceList::const_iterator Board::firstPieceItr(Piece::Color color) const {
	return pieceList[color].begin();
}

PieceList::const_iterator Board::endPieceItr(Piece::Color color) const {
	return pieceList[color].end();
}

void Board::executeMove(const Move &move) {
	// Move object first, to avoid the subject overwriting it in the event of a capture.
	if (move.object) {
		this->move(move.object, move.object_to);
	}
	this->move(move.subject, move.subject_to);
	if (move.type >= Move::PROMOTION) {
		// Promote accordingly.
		pawnStructure[move.subject->color][move.subject_to] = false;
		move.subject->type = (Piece::Type)(move.type - Move::PROMOTION);
	}
}

void Board::reverseMove(const Move &move) {
	// Move subject first, to avoid the object overwriting it in the event of reversing a capture.
	this->move(move.subject, move.subject_from);
	if (move.type >= Move::PROMOTION) {
		// Unpromote back to pawn.
		pawnStructure[move.subject->color][move.subject_from] = true;
		move.subject->type = Piece::PAWN;
	}
	if (move.object) {
		this->move(move.object, move.object_from);
	}
}

void Board::print() const {
	std::cout << "   A  B  C  D  E  F  G  H" << std::endl << std::endl;
	for (int rank = NUM_RANKS - 1; rank >= 0; rank--) {
		std::cout << rank + 1 << ' ';
		for (int file = 0; file < NUM_FILES; file++) {
			Piece *piece = getPiece(Position::ToInt(file, rank));
			std::cout << ' ';
			if (!piece) std::cout << '-';
			else std::cout << Piece::Ascii[piece->color][piece->type];
			std::cout << ' ';
		}
		std::cout << ' ' << rank + 1 << std::endl;
	}
	std::cout << std::endl << "   A  B  C  D  E  F  G  H" << std::endl;
}

}
