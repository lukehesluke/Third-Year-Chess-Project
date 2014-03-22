#include "window.hpp"

namespace ChessProject {

const std::string Window::IconFilename = "res/b_king.png";

Window::Window() :
	gui(HUMAN, HUMAN) {
}

bool Window::init() {
	set_title("Chess");
	if (!set_icon_from_file(IconFilename)) return false;
	set_resizable(false);
	// Extra height added on to fit the menubar and statusbar.
	static const int extraHeight = 40;
	set_size_request(Gui::TileSize * NUM_FILES, Gui::TileSize * NUM_RANKS + extraHeight);
	if (!gui.init(this)) return false;
	add(box);
	// Game menu
	gameMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("_New", Gtk::AccelKey('n', Gdk::CONTROL_MASK),
		sigc::mem_fun(gui, &Gui::newGame)));
	gameMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Quit", Gtk::AccelKey('q', Gdk::CONTROL_MASK),
		sigc::mem_fun(*this, &Window::hide)));
	gameMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("Select _White player", Gtk::AccelKey(),
		sigc::mem_fun(gui, &Gui::selectWhitePlayer)));
	gameMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("Select _Black player", Gtk::AccelKey(),
		sigc::mem_fun(gui, &Gui::selectBlackPlayer)));
	// Help menu
	helpMenu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Hint", Gtk::AccelKey('h', Gdk::CONTROL_MASK),
		sigc::mem_fun(gui, &Gui::hint)));
	// Set up menubar
	menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Game", gameMenu));
	menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Help", helpMenu));
	box.pack_start(menubar, Gtk::PACK_SHRINK);
	box.add(gui);
	statusbar.set_has_resize_grip(false);
	box.pack_start(statusbar, Gtk::PACK_SHRINK);
	show_all_children();
	return true;
}

void Window::setStatus(std::string msg) {
	statusbar.push(msg.c_str());
}

}
