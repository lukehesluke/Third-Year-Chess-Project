#include "movelist.hpp"
#include "gameinfo.hpp"

namespace ChessProject {

void MoveList::generate(Piece::Color color, Board *board, GameInfo *info, bool onlyInteresting) {
	for (PieceList::iterator pieceItr = board->firstPieceItr(color);
		 pieceItr != board->endPieceItr(color); pieceItr++) {
		Piece *piece = *pieceItr;
		generate(*pieceItr, board, info, onlyInteresting);
	}
}

void MoveList::generate(Piece *piece, Board *board, GameInfo *info, bool onlyInteresting) {
	// Special moves
	if (piece->type == Piece::PAWN) {
		genPawnMoves(piece, board, info, onlyInteresting);
		return;
	} else if (!onlyInteresting && piece->type == Piece::KING) {
		// Castling
		if (info->canCastle(piece->color, GameInfo::KINGSIDE, board)) {
			Piece *rook = board->getPiece(piece->pos + 3);
			if (!rook) std::cerr << "No kingside rook for color " << piece->color << std::endl;
			insert(Move(piece, piece->pos, piece->pos + 2, rook, rook->pos, rook->pos - 2, Move::NORMAL));
		}
		if (info->canCastle(piece->color, GameInfo::QUEENSIDE, board)) {
			Piece *rook = board->getPiece(piece->pos - 4);
			if (!rook) std::cerr << "No queenside rook for color " << piece->color << std::endl;
			insert(Move(piece, piece->pos, piece->pos - 2, rook, rook->pos, rook->pos + 3, Move::NORMAL));
		}
	}
	// For each offset that a piece can move in.
	for (intlist::const_iterator offsetItr = Buffer::Offset[piece->type].begin();
			offsetItr != Buffer::Offset[piece->type].end(); offsetItr++) {
		for (position to = Buffer::Board[Buffer::Coords[piece->pos] + *offsetItr];
				to >= 0; to = Buffer::Board[Buffer::Coords[to] + *offsetItr]) {
			Piece *other = board->getPiece(to);
			if (!other) {
				// If this is an empty space, add it, and keep going.
				if (!onlyInteresting) insert(Move(piece, piece->pos, to, 0, -1, -1, Move::NORMAL));
			} else if (other->color != piece->color) {
				// Capture enemy and then stop iterating. Sliding pieces cannot jump over enemies.
				insert(Move(piece, piece->pos, to, other, other->pos, -1, Move::NORMAL));
				break;
			} else {
				// Stop if the square is occupied by an allied piece.
				break;
			}
			// Only continue if this piece is a sliding piece (such as bishop, queen or rook).
			if (!Piece::CanSlide[piece->type]) break;
		}
	}
}

bool MoveList::generateForHuman(position pos, Board *board, GameInfo *info) {
	Piece *piece = board->getPiece(pos);
	if (!piece || piece->color != info->turn) return false;
	generate(piece, board, info, false);
	return true;
}

bool MoveList::UnderThreat(position pos, Piece::Color enemies, Board *board) {
	// Under threat from pawns?
	for (int i = -1; i <= 1; i += 2) {
		position enemyPos = Buffer::Board[Buffer::Coords[pos] + Buffer::PawnOffset[!enemies] + i];
		if (enemyPos < 0) continue;
		Piece *enemyPawn = board->getPiece(enemyPos);
		if (!enemyPawn) continue;
		if (enemyPawn->color == enemies && enemyPawn->type == Piece::PAWN) return true;
	}
	// Under threat from anything else?
	for (int type = 0; type < NUM_PIECE_TYPES; type++) {
		// The queen can be tested using the rook and bishop moves.
		if (type == Piece::QUEEN) continue;
		bool queenCanCapture = (type != Piece::KNIGHT);
		// For every piece type, see if the position can attack that piece type. If it can, it can be attacked by that piece type.
		for (intlist::const_iterator offsetItr = Buffer::Offset[type].begin();
			 offsetItr != Buffer::Offset[type].end(); offsetItr++) {
			for (position to = Buffer::Board[Buffer::Coords[pos] + *offsetItr];
				 to >= 0; to = Buffer::Board[Buffer::Coords[to] + *offsetItr]) {
				Piece *other = board->getPiece(to);
				if (other) {
					if (other->color == enemies && (other->type == type || (other->type == Piece::QUEEN && queenCanCapture))) return true;
					else break;
				}
				if (!Piece::CanSlide[type]) break;
			}
		}
	}
	return false;
}

bool MoveList::InCheck(Piece::Color color, Board *board) {
	return UnderThreat(board->getKing(color)->pos, (Piece::Color)!color, board);
}

MoveList::const_iterator MoveList::getMove(position from, position to) const {
	const_iterator itr;
	for (itr = begin(); itr != end(); itr++) {
		if (itr->subject_from == from && itr->subject_to == to) break;
	}
	return itr;
}

void MoveList::prune(Piece::Color enemies, Board *board) {
	std::vector<iterator> movesToPrune;
	for (iterator moveItr = begin(); moveItr != end(); moveItr++) {
		board->executeMove(*moveItr);
		// For each move, execute it on the board, and check for any danger to the king.
		//if (UnderThreat(board->getKing((Piece::Color)!enemies)->pos, enemies, board))
		if (InCheck((Piece::Color)!enemies, board))
			movesToPrune.push_back(moveItr);
		board->reverseMove(*moveItr);
	}
	for (std::vector<iterator>::iterator badMoveItr = movesToPrune.begin(); badMoveItr != movesToPrune.end(); badMoveItr++) {
		erase(*badMoveItr);
	}
}

void MoveList::genPawnMoves(Piece *pawn, Board *board, GameInfo *info, bool onlyInteresting) {
	if (!onlyInteresting || Position::Rank(pawn->pos) == Piece::PawnPromotionRank[pawn->color]) {
		position advance = Buffer::Board[Buffer::Coords[pawn->pos] + Buffer::PawnOffset[pawn->color]];
		if (advance >= 0 && !board->getPiece(advance)) {
			genPawnPromotions(Move(pawn, pawn->pos, advance, 0, -1, -1, Move::NORMAL));
			// Double advance
			if (Piece::InitialSetup[pawn->color][Piece::PAWN].count(pawn->pos) != 0) {
				advance = Buffer::Board[Buffer::Coords[advance] + Buffer::PawnOffset[pawn->color]];
				if (advance >= 0 && !board->getPiece(advance)) {
					insert(Move(pawn, pawn->pos, advance, 0, -1, -1, Move::PAWN_DOUBLE_ADVANCE));
				}
			}
		}
	}
	// Capture
	for (int i = -1; i <= 1; i += 2) {
		position capture = Buffer::Board[Buffer::Coords[pawn->pos] + Buffer::PawnOffset[pawn->color] + i];
		if (capture < 0) continue;
		Piece *other = board->getPiece(capture);
		if (!other) {
			// En passant
			if (info->enPassantTarget && info->enPassantTarget->pos == pawn->pos + i)
				insert(Move(pawn, pawn->pos, capture, info->enPassantTarget, info->enPassantTarget->pos, -1, Move::NORMAL));
		} else if (other->color != pawn->color) {
			genPawnPromotions(Move(pawn, pawn->pos, capture, other, other->pos, -1, Move::NORMAL));
		}
	}

}

void MoveList::genPawnPromotions(Move move) {
	if (Position::Rank(move.subject_from) == Piece::PawnPromotionRank[move.subject->color]) {
		move.type = Move::PROMOTION_BISHOP;
		insert(move);
		move.type = Move::PROMOTION_KNIGHT;
		insert(move);
		move.type = Move::PROMOTION_QUEEN;
		insert(move);
		move.type = Move::PROMOTION_ROOK;
		insert(move);
	} else insert(move);
}

}