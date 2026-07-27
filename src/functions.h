#pragma once
#include <functional>
#include <string>
#include <unordered_set>

#include <sqlite3.h>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// Each factory below builds a self-contained FTXUI component for one
// screen; callbacks fire when the user navigates away (picking a menu
// entry, finishing a form, etc.) so the caller can swap the active screen.
ftxui::Component MainMenuComponent(std::function<void(int)> on_select);
ftxui::Component StartMenuComponent(sqlite3* db, std::function<void(int)> on_select);
ftxui::Component AboutComponent(std::function<void()> on_back);
ftxui::Component CreateWordSetComponent(sqlite3* db, std::function<void()> on_done);

class Game {
public:
	Game(sqlite3* db, int id);
	ftxui::Component GameComponent(std::function<void(bool won)> on_finished);

private:
	void getWordInfo(int num);
	int generate_random_number();
	void guessLetter(char letter);
	ftxui::Element Render() const;

	sqlite3* db; // non-owning
	int setId;
	int max_word_id;
	int stage; // stage of the hangman (max stage is 6)
	char alphabet[26];
	int current_word_id;
	std::string guessing;
	std::string word;
	std::string definition;
	std::unordered_set<int> usedNumbers; // to track used random numbers
	bool over;
	bool won;
};
