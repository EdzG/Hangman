#include <iostream>
#include <memory>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "functions.h"
#include "db.h"

using namespace ftxui;

namespace {
enum Page { kMenu = 0, kStartMenu = 1, kAbout = 2, kCreateWordSet = 3, kGame = 4 };
}

int main() {
	sqlite3* db = db_open("hangman.db");
	if (!db) {
		std::cerr << "Could not open the database. Exiting." << std::endl;
		return 1;
	}

	auto screen = ScreenInteractive::Fullscreen();

	int active_page = kMenu;
	std::shared_ptr<Game> current_game;

	// These three screens carry per-visit state (word set list, wizard
	// progress, live game state), so they're rebuilt fresh every time the
	// user navigates to them rather than being constructed once up front.
	Component start_menu_host = std::make_shared<ComponentBase>();
	Component create_word_set_host = std::make_shared<ComponentBase>();
	Component game_host = std::make_shared<ComponentBase>();

	auto go_to_menu = [&] { active_page = kMenu; };

	auto enter_start_menu = [&] {
		start_menu_host->DetachAllChildren();
		start_menu_host->Add(StartMenuComponent(db, [&](int set_id) {
			if (set_id == -1) {
				go_to_menu();
				return;
			}
			current_game = std::make_shared<Game>(db, set_id);
			game_host->DetachAllChildren();
			game_host->Add(current_game->GameComponent([&](bool /*won*/) { go_to_menu(); }));
			active_page = kGame;
			}));
		active_page = kStartMenu;
	};

	auto enter_create_word_set = [&] {
		create_word_set_host->DetachAllChildren();
		create_word_set_host->Add(CreateWordSetComponent(db, [&] { go_to_menu(); }));
		active_page = kCreateWordSet;
	};

	auto main_menu = MainMenuComponent([&](int choice) {
		switch (choice) {
		case 1: enter_start_menu(); break;
		case 2: active_page = kAbout; break;
		case 3: enter_create_word_set(); break;
		default: screen.Exit(); break;
		}
		});

	auto about = AboutComponent(go_to_menu);

	auto root = Container::Tab(
		{ main_menu, start_menu_host, about, create_word_set_host, game_host },
		&active_page);

	screen.Loop(root);

	sqlite3_close(db);
	return 0;
}
