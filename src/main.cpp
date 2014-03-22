#include <cstdlib>
#include <gtkmm/main.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/window.h>
#include "gui.hpp"
#include "window.hpp"
using namespace ChessProject;


int main(int argc, char **argv) {
	Bitboard::Precalculate();
	Gtk::Main kit(argc, argv);
	Window window;
	if (!window.init()) {
		return EXIT_FAILURE;
	}
	Gtk::Main::run(window);
	return EXIT_SUCCESS;
}
