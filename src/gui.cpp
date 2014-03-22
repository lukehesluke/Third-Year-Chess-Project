#include "gui.hpp"
#include "window.hpp"

namespace ChessProject {

const std::string Gui::SpritesheetFilename = "res/spritesheet50x50.png";
const double Gui::LightTileRed = 0.7;
const double Gui::LightTileGreen = 0.7;
const double Gui::LightTileBlue = 0.7;
const double Gui::DarkTileRed = 0.3;
const double Gui::DarkTileGreen = 0.3;
const double Gui::DarkTileBlue = 0.3;
const double Gui::HighlightBonus = 2.0;
const double Gui::HighlightPenalty = 0.5;

Gui::Gui(Player white, Player black) :
	movingFrom(-1),
	finished(false) {
	players[Piece::WHITE] = white;
	players[Piece::BLACK] = black;
	board = new Board;
	info = new GameInfo;
	board->init();
	info->init();
}

bool Gui::init(Window *window) {
	this->window = window;
	// Add mouse click handling.
	add_events(Gdk::BUTTON_PRESS_MASK);
	signal_event().connect(sigc::mem_fun(*this, &Gui::handleEvent));
	// Load sprite sheet form file.
	try {
		spritesheet = Gdk::Pixbuf::create_from_file(SpritesheetFilename);
	} catch (const Glib::FileError &e) {
		std::cerr << "Error loading image: " << e.what() << std::endl;
		return false;
	} catch (const Gdk::PixbufError &e) {
		std::cerr << "Error loading image: " << e.what() << std::endl;
		return false;
	}
	updateTurn();
	return true;
}

void Gui::newGame() {
	board->init();
	info->init();
	hintMove = Move();
	movingFrom = -1;
	finished = false;
	moveList.clear();
	draw();
	updateTurn();
	std::cout << "New Game" << std::endl;
}

void Gui::hint() {
	static const int depth = 4;
	static const int quiescenceDepth = 8;
	int eval = Minimax::AlphaBeta(board, info, hintMove, depth, quiescenceDepth);
	std::cout << "How about move " << hintMove.toAlgebraic() << " with evaluation " << eval << std::endl;
	draw();
}

void Gui::selectWhitePlayer() {
	Gtk::MessageDialog dialog("Choose white player", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
	dialog.set_secondary_text("Please choose whether white is an AI or human player");
	dialog.add_button("Human", HUMAN);
	dialog.add_button("AI", AI);
	int result = dialog.run();
	players[Piece::WHITE] = (Player)result;
	std::cout << "White player set to: " << result << std::endl;
	updateTurn();
}

void Gui::selectBlackPlayer() {
	Gtk::MessageDialog dialog("Choose black player", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
	dialog.set_secondary_text("Please choose whether black is an AI or human player");
	dialog.add_button("Human", HUMAN);
	dialog.add_button("AI", AI);
	int result = dialog.run();
	players[Piece::BLACK] = (Player)result;
	std::cout << "Black player set to: " << result << std::endl;
	updateTurn();
}

void Gui::draw(Piece *piece) {
	// The spritesheet is ordered to correlate with the Piece::Color and Piece::Type enums, so drawing from it is rather convenient.
	spritesheet->render_to_drawable(get_window(), get_style()->get_black_gc(), piece->type * TileSize, piece->color * TileSize,
		Position::File(piece->pos) * TileSize, (NUM_RANKS - 1 - Position::Rank(piece->pos)) * TileSize, TileSize, TileSize,
		Gdk::RGB_DITHER_NONE, 0, 0);
}

void Gui::updateTurn() {
	if (finished) return;
	std::cout << "Turn " << info->numTurns << std::endl;
	std::cout << "50 move rule counter: " << info->fiftyMoveRuleCounter << std::endl;
	std::string statusMsg = Piece::ColorString[info->turn] + "'s turn to move. ";
	MoveList ml;
	// Determine whether the game has ended or is in check. And generate list of possible moves.
	GameInfo::State state = info->updateState(board, ml);
	if (info->inCheck) statusMsg += "CHECK";
	switch (state) {
		case GameInfo::CHECKMATE:
			statusMsg = "CHECKMATE! Victory for " + Piece::ColorString[!(Piece::Color)info->turn] + "!";
			finished = true;
		break;
		case GameInfo::STALEMATE:
			std::cout << "Stalemate. The game is drawn." << std::endl;
			finished = true;
		break;
		case GameInfo::FIFTY_MOVE_RULE:
			std::cout << "Draw due to fifty move rule." << std::endl;
			finished = true;
		break;
	}
	std::cout << statusMsg << std::endl;
	if (players[info->turn] == AI) statusMsg += " Please click to continue...";
	window->setStatus(statusMsg);
}

void Gui::doAiMove() {
	if (finished) return;
	Move move;
	static const int depth = 3;
	static const int quiescenceDepth = 8;
	// AI searches game tree at specified depths to decide next move.
	int eval = Minimax::AlphaBeta(board, info, move, depth, quiescenceDepth);
	std::cout << "Executed move " << move.toAlgebraic() << " with evaluation " << eval << std::endl;
	board->executeMove(move);
	info->executeMove(move);
	draw();
	updateTurn();
}

void Gui::draw() {
	if (!window->is_visible()) return;
	Cairo::RefPtr<Cairo::Context> context = get_window()->create_cairo_context();
	// Draw checkered board.
	for (position p = 0; p < BOARDSIZE; p++) {
		double red, green, blue;
		if (Position::IsLightTile(p)) {
			red = LightTileRed;
			green = LightTileGreen;
			blue = LightTileBlue;
		}
		else {
			red = DarkTileRed;
			green = DarkTileGreen;
			blue = DarkTileBlue;
		}
		// Highlight any possible move.
		if (hintMove.subject && (hintMove.subject_from == p || hintMove.subject_to == p)) {
			red *= HighlightBonus;
			green *= HighlightPenalty;
			blue *= HighlightPenalty;
		} else if (movingFrom >= 0 && (moveList.getMove(movingFrom, p) != moveList.end())) {
			red *= HighlightPenalty;
			green *= HighlightPenalty;
			blue *= HighlightBonus;
		}
		context->set_source_rgb(red, green, blue);
		context->rectangle(Position::File(p) * TileSize, (NUM_RANKS - 1 - Position::Rank(p)) * TileSize, TileSize, TileSize);
		context->fill();
	}
	// Draw pieces.
	for (int color = 0; color < 2; color++) {
		for (PieceList::const_iterator pieceItr = board->firstPieceItr((Piece::Color)color);
			 pieceItr != board->endPieceItr((Piece::Color)color); pieceItr++) {
			draw(*pieceItr);
		}
	}
}

bool Gui::handleEvent(GdkEvent *event) {
	if (event->type == Gdk::EXPOSE) draw();
	if (finished) return true;
	if (event->type == Gdk::BUTTON_PRESS) {
		if (players[info->turn] == AI) {
			doAiMove();
			return true;
		}
		// Ascertain the position of the click.
		position pos = Position::ToInt((int)event->button.x / TileSize, NUM_RANKS - 1 - ((int)event->button.y / TileSize));
		std::cout << "Click at " << Position::ToAlgebraic(pos) << std::endl;
		// If choosing a piece to move..
		if (movingFrom < 0) {
			std::cout << "Moving from..." << std::endl;
			if (moveList.generateForHuman(pos, board, info)) {
				std::cout << "Legal piece to move!" << std::endl;
				moveList.prune((Piece::Color)!info->turn, board);
				movingFrom = pos;
				// Draw highlighted moves.
				draw();
			} else std::cout << "Cannot move this piece!" << std::endl;
		// If choosing a destination..
		} else {
			std::cout << "Moving to..." << std::endl;
			MoveList::const_iterator moveItr = moveList.getMove(movingFrom, pos);
			if (moveItr == moveList.end()) {
				// Invalid move.
				std::cout << "Invalid move!" << std::endl;
			} else {
				// Execute move.
				std::cout << "Executing move" << std::endl;
				Move move = *moveItr;
				if (move.type >= Move::PROMOTION) {
					move.type = (Move::Type)(Move::PROMOTION + handlePromotion());
				}
				board->executeMove(move);
				info->executeMove(move);
				updateTurn();
				// Reset hint.
				hintMove = Move();
			}
			movingFrom = -1;
			moveList.clear();
			// Redraw
			draw();
		}
	}
	return true;
}

Piece::Type Gui::handlePromotion() {
	Gtk::MessageDialog dialog("Promotion!", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
	dialog.set_secondary_text("Please choose a piece type to promote to");
	dialog.add_button("Bishop", Piece::BISHOP);
	dialog.add_button("Knight", Piece::KNIGHT);
	dialog.add_button("Queen", Piece::QUEEN);
	dialog.add_button("Rook", Piece::ROOK);
	int result = dialog.run();
	std::cout << "RESULT: " << result << std::endl;
	return (Piece::Type)result;
}

}
