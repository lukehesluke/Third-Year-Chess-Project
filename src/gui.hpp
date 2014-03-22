#pragma once

#include <string>
#include <iostream>
#include <gtkmm/messagedialog.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/statusbar.h>
#include "board.hpp"
#include "gameinfo.hpp"
#include "minimax.hpp"
#include "movelist.hpp"
#include "piece.hpp"

namespace ChessProject {

enum Player {
	HUMAN,
	AI
};

class Window;

class Gui : public Gtk::DrawingArea {
public:
	// The filename of the piece spritesheet.
	static const std::string SpritesheetFilename;
	// Size of each tile in pixels.
	static const int TileSize = 50;
	// The color values of light/dark tiles on the board.
	static const double LightTileRed, LightTileGreen, LightTileBlue;
	static const double DarkTileRed, DarkTileGreen, DarkTileBlue;
	// If a selected piece can move to a tile, the tile is highlighted by multiplying the color value of the tile with these constants.
	static const double HighlightBonus, HighlightPenalty;
	Gui(Player white, Player black);
	// Initialize the gui.
	bool init(Window *window);
	// Start a new game.
	void newGame();
	// Suggest a move for the player. Highlight the move on the board.
	void hint();
	// Launch the player selection dialog.
	void selectWhitePlayer();
	void selectBlackPlayer();
private:
	Window *window;
	// Sprite sheet image.
	Glib::RefPtr<Gdk::Pixbuf> spritesheet;
	Board *board;
	GameInfo *info;
	Player players[NUM_COLORS];
	// Whether or not the game is finished.
	bool finished;
	// The position being moved from by the player. Set to a negative number if the player is not choosing a destination.
	position movingFrom;
	// The list of moves that the player can make.
	MoveList moveList;
	// If the user has requested a hint, the hint is stored to this move. The hint is highlighted on the board.
	Move hintMove;
	// Draw a piece from the sprite sheet onto the window.
	void draw(Piece *piece);
	// Update the turn
	void updateTurn();
	// Perform AI move.
	void doAiMove();
	// Draw board.
	void draw();
	// Handle mouse click events.
	bool handleEvent(GdkEvent *event);
	// Handle human promotion. Returns the type promoted to.
	Piece::Type handlePromotion();
};

}
