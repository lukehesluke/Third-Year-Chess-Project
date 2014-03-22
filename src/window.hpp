#pragma once

#include <string>
#include <gtkmm/box.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/window.h>
#include "gui.hpp"

namespace ChessProject {

class Window : public Gtk::Window {
public:
	// The filename of the window's icon.
	static const std::string IconFilename;
	Window();
	bool init();
	// Push status to the status bar.
	void setStatus(std::string msg);
private:
	Gui gui;
	Gtk::VBox box;
	Gtk::MenuBar menubar;
	Gtk::Menu gameMenu, helpMenu;
	Gtk::Statusbar statusbar;
};

}
