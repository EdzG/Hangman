#include "functions.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

using namespace ftxui;

//***********Top-level screen components******************

ftxui::Component MainMenuComponent(std::function<void(int)> on_select) {
	auto entries = std::make_shared<std::vector<std::string>>(std::vector<std::string>{
		"START", "ABOUT", "CREATE WORD SET", "QUIT"
	});
	auto selected = std::make_shared<int>(0);

	MenuOption option;
	option.on_enter = [on_select, selected] { on_select(*selected + 1); };
	auto menu = Menu(entries.get(), selected.get(), option);

	return Renderer(menu, [menu, entries, selected] {
		return vbox({
			text("HANGMAN") | bold | hcenter,
			separator(),
			menu->Render(),
		}) | border | size(WIDTH, GREATER_THAN, 30);
	});
}

ftxui::Component StartMenuComponent(sqlite3* db, std::function<void(int)> on_select) {
	auto ids = std::make_shared<std::vector<int>>();
	auto labels = std::make_shared<std::vector<std::string>>();

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, "SELECT setId, setName FROM wordSet", -1, &stmt, nullptr);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		ids->push_back(sqlite3_column_int(stmt, 0));
		labels->push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
	}
	sqlite3_finalize(stmt);
	ids->push_back(-1);
	labels->push_back("Go Back");

	auto selected = std::make_shared<int>(0);
	MenuOption option;
	option.on_enter = [on_select, ids, selected] { on_select((*ids)[*selected]); };
	auto menu = Menu(labels.get(), selected.get(), option);

	return Renderer(menu, [menu, labels, ids, selected] {
		return vbox({
			text("Choose a word set") | bold | hcenter,
			separator(),
			menu->Render(),
		}) | border | size(WIDTH, GREATER_THAN, 30);
	});
}

ftxui::Component AboutComponent(std::function<void()> on_back) {
	auto renderer = Renderer([](bool) {
		std::string body =
			"Hangman is a word-guessing game where one player thinks of a word and "
			"the other player tries to guess it by suggesting letters. The word is "
			"represented by a series of blank spaces, each corresponding to a "
			"letter in the word. A part of a stick figure (the hangman) is drawn "
			"for each incorrect letter guessed. If a correct letter is chosen, all "
			"the blank spaces belonging to that letter will be filled. The game is "
			"won if the guesser identifies the word before completing the hangman "
			"drawing. Conversely, the game is lost if the entire hangman is drawn "
			"before the word is guessed. In this version of the game, the player's "
			"word will be chosen at random from a database. The player will also "
			"be given the definition of the word that is to be guessed. The game "
			"also gives the player an option to create their own set of words.";
		return vbox({
			text("About") | bold | hcenter,
			separator(),
			paragraph(body),
			separator(),
			text("Press Enter to go back") | dim | hcenter,
		}) | border | size(WIDTH, LESS_THAN, 80);
	});

	return CatchEvent(renderer, [on_back](Event event) {
		if (event == Event::Return || event == Event::Escape) {
			on_back();
			return true;
		}
		return false;
	});
}

//***********Create Word Set wizard******************

namespace {
struct WordSetState {
	sqlite3* db = nullptr;
	int step = 0; // 0=set name, 1=word count, 2=word[i], 3=definition[i], 4=done
	std::string setName;
	std::string wordCountStr;
	int numOfWords = 0;
	sqlite3_int64 setDbId = 0;
	int currentWordIndex = 0;
	std::string currentWordName;
	std::string currentDefinition;
	std::string error;
};
}

ftxui::Component CreateWordSetComponent(sqlite3* db, std::function<void()> on_done) {
	auto state = std::make_shared<WordSetState>();
	state->db = db;

	auto name_input = Input(&state->setName, "set name");
	auto count_input = Input(&state->wordCountStr, "number of words");
	auto word_input = Input(&state->currentWordName, "word");
	auto def_input = Input(&state->currentDefinition, "definition");

	auto submit = [state]() {
		state->error.clear();
		switch (state->step) {
		case 0:
			if (state->setName.empty()) {
				state->error = "Set name can't be empty.";
				return;
			}
			state->step = 1;
			break;
		case 1: {
			try {
				size_t pos = 0;
				state->numOfWords = std::stoi(state->wordCountStr, &pos);
				if (pos != state->wordCountStr.size() || state->numOfWords <= 0) {
					throw std::invalid_argument("");
				}
			}
			catch (...) {
				state->error = "Enter a positive whole number.";
				return;
			}
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(state->db, "INSERT INTO wordSet(setName, numOfWords) VALUES (?, ?)", -1, &stmt, nullptr);
			sqlite3_bind_text(stmt, 1, state->setName.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, state->numOfWords);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			state->setDbId = sqlite3_last_insert_rowid(state->db);
			state->step = 2;
			break;
		}
		case 2:
			if (state->currentWordName.empty()) {
				state->error = "Word can't be empty.";
				return;
			}
			state->step = 3;
			break;
		case 3: {
			if (state->currentDefinition.empty()) {
				state->error = "Definition can't be empty.";
				return;
			}
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(state->db, "INSERT INTO Words(wordName, definition, setId) VALUES (?, ?, ?)", -1, &stmt, nullptr);
			sqlite3_bind_text(stmt, 1, state->currentWordName.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 2, state->currentDefinition.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 3, static_cast<int>(state->setDbId));
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			state->currentWordName.clear();
			state->currentDefinition.clear();
			state->currentWordIndex++;
			state->step = (state->currentWordIndex >= state->numOfWords) ? 4 : 2;
			break;
		}
		default:
			break;
		}
	};

	auto renderer = Renderer([state, name_input, count_input, word_input, def_input](bool) {
		Elements body;
		body.push_back(text("Create a new word set") | bold | hcenter);
		body.push_back(separator());

		switch (state->step) {
		case 0:
			body.push_back(text("Enter the name of the set:"));
			body.push_back(name_input->Render());
			break;
		case 1:
			body.push_back(text("Set: " + state->setName));
			body.push_back(text("How many words do you want to add?"));
			body.push_back(count_input->Render());
			break;
		case 2:
			body.push_back(text("Word " + std::to_string(state->currentWordIndex + 1) +
				" / " + std::to_string(state->numOfWords)));
			body.push_back(text("Enter the word:"));
			body.push_back(word_input->Render());
			break;
		case 3:
			body.push_back(text("Word " + std::to_string(state->currentWordIndex + 1) +
				" / " + std::to_string(state->numOfWords) + ": " + state->currentWordName));
			body.push_back(text("Enter its definition:"));
			body.push_back(def_input->Render());
			break;
		default:
			body.push_back(text("Done! \"" + state->setName + "\" has been saved.") | color(Color::Green));
			body.push_back(text("Press Enter to go back to the menu.") | dim);
			break;
		}

		if (!state->error.empty()) {
			body.push_back(separator());
			body.push_back(text(state->error) | color(Color::Red));
		}

		return vbox(body) | border | size(WIDTH, LESS_THAN, 80);
	});

	return CatchEvent(renderer, [state, name_input, count_input, word_input, def_input, submit, on_done](Event event) {
		if (event == Event::Return) {
			if (state->step == 4) {
				on_done();
			}
			else {
				submit();
			}
			return true;
		}
		ftxui::Component active;
		switch (state->step) {
		case 0: active = name_input; break;
		case 1: active = count_input; break;
		case 2: active = word_input; break;
		case 3: active = def_input; break;
		default: break;
		}
		return active ? active->OnEvent(event) : false;
	});
}

//***********Game******************

Game::Game(sqlite3* db, int id)
	: db(db), setId(id), max_word_id(0), stage(0), current_word_id(0), over(false), won(false) {
	for (int i = 0; i < 26; i++) {
		alphabet[i] = ' ';
	}

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, "SELECT numOfWords FROM wordSet WHERE setId = ?", -1, &stmt, nullptr);
	sqlite3_bind_int(stmt, 1, setId);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		max_word_id = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	int num = generate_random_number();
	getWordInfo(num);
}

int Game::generate_random_number() {
	if (usedNumbers.size() >= static_cast<size_t>(max_word_id)) {
		throw std::runtime_error("All possible numbers have been used.");
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(1, max_word_id);

	int randomNumber;
	do {
		randomNumber = distr(gen);
	} while (usedNumbers.find(randomNumber) != usedNumbers.end());

	usedNumbers.insert(randomNumber);
	return randomNumber;
}

void Game::getWordInfo(int num) {
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, "SELECT wordName, definition FROM Words WHERE setId = ?", -1, &stmt, nullptr);
	sqlite3_bind_int(stmt, 1, setId);

	bool gotInfo = false;
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		if (num == 1) {
			word = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			definition = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			gotInfo = true;
			break;
		}
		num--;
	}
	sqlite3_finalize(stmt);

	if (!gotInfo) {
		throw std::runtime_error("Word not found in the database.");
	}

	guessing.assign(word.length(), '_');
	for (size_t i = 0; i < word.length(); i++) {
		if (word[i] == ' ') guessing[i] = ' ';
	}
}

void Game::guessLetter(char letter) {
	if (over) return;

	letter = static_cast<char>(std::toupper(static_cast<unsigned char>(letter)));
	if (letter < 'A' || letter > 'Z') return;

	int index = letter - 'A';
	if (alphabet[index] != ' ') return; // already guessed
	alphabet[index] = letter;

	bool inWord = false;
	for (size_t i = 0; i < word.length(); i++) {
		if (std::toupper(static_cast<unsigned char>(word[i])) == letter) {
			guessing[i] = letter;
			inWord = true;
		}
	}

	if (!inWord) stage++;

	bool wordGuessed = guessing.find('_') == std::string::npos;
	if (wordGuessed) {
		over = true;
		won = true;
	}
	else if (stage >= 6) {
		over = true;
		won = false;
	}
}

namespace {
// clang-format off
const std::vector<std::vector<std::string>> kHangmanStages = {
	{"+---+", "|   |", "    |", "    |", "    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "    |", "    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "|   |", "    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "/|   |", "    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "/|\\  |", "    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "/|\\  |", "/    |", "    |", "========="},
	{"+---+", "|   |", "O   |", "/|\\  |", "/ \\  |", "    |", "========="},
};
// clang-format on
}

ftxui::Element Game::Render() const {
	Color stage_color = Color::White;
	if (stage > 4) stage_color = Color::Red;
	else if (stage > 2) stage_color = Color::Yellow;

	Elements hangman_lines;
	for (const std::string& line : kHangmanStages[static_cast<size_t>(stage)]) {
		hangman_lines.push_back(text(line));
	}
	Element hangman_art = vbox(hangman_lines) | color(stage_color) | bold;

	Elements alphabet_row;
	for (int i = 0; i < 26; i++) {
		char letter = alphabet[i];
		std::string s(1, letter == ' ' ? static_cast<char>('A' + i) : letter);
		Element e = text(s);
		if (letter == ' ') {
			e = e | dim;
		}
		else {
			bool correct = std::any_of(word.begin(), word.end(), [&](char c) {
				return std::toupper(static_cast<unsigned char>(c)) == letter;
			});
			e = e | color(correct ? Color::Green : Color::Red) | bold;
		}
		alphabet_row.push_back(e);
		alphabet_row.push_back(text(" "));
	}

	std::string blanks;
	for (char g : guessing) {
		blanks += g;
		blanks += ' ';
	}

	Element status;
	if (over) {
		status = won
			? text("You guessed it! Press any key to continue...") | color(Color::Green) | bold
			: text("Game over. The word was: " + word + " -- press any key to continue...") | color(Color::Red) | bold;
	}
	else {
		status = text("Type a letter to guess (" + std::to_string(6 - stage) + " misses left)") | dim;
	}

	return vbox({
		text("HANGMAN") | bold | hcenter,
		separator(),
		hangman_art | hcenter,
		separator(),
		paragraph(definition) | border,
		separator(),
		hbox(alphabet_row) | hcenter,
		separator(),
		text("Word: " + blanks) | hcenter,
		separator(),
		status | hcenter,
	}) | border | size(WIDTH, LESS_THAN, 90);
}

ftxui::Component Game::GameComponent(std::function<void(bool)> on_finished) {
	auto renderer = Renderer([this](bool) { return Render(); });

	return CatchEvent(renderer, [this, on_finished](Event event) {
		if (over) {
			on_finished(won);
			return true;
		}
		if (event.is_character() && event.character().size() == 1) {
			char c = event.character()[0];
			if (std::isalpha(static_cast<unsigned char>(c))) {
				guessLetter(c);
				return true;
			}
		}
		return false;
	});
}
