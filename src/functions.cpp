#include "functions.h"

//*******************Only color function was used ****
void Formatting::charactersCase(char* input)
{
	for (int i = 0; input[i] != '\0'; ++i)
	{
		if (input[i] >= 'a' && input[i] <= 'z')
		{
			input[i] = input[i] - 32;
		}
	}
}

void Formatting::color(int k)
{
	std::cout << "\x1b[" << k << "m";
}

//***********Game GUI functions******************

void print_line(int start, int end) {
	for (int i = start; i < end; i++) {
		if (i == 0 || i == 79) std::cout << "+";
		else std::cout << "-";
	}
	std::cout << std::endl;
}
void print_menu_option(const std::string& str, const std::string& choice) {
	size_t length = END - str.length();
	std::cout << str;
	for (size_t i = 0; i < length; i++) {
		if (i == length - choice.length() - 1) std::cout << choice << "|";
		else std::cout << " ";
	}
	std::cout << std::endl;
}

void print_title() {
	std::cout << std::endl;
	std::cout << std::setw(40) << "HANGMAN" << std::endl;
}
void print_menu() {

	Formatting::color(PURPLE);
	std::string start = "|START";
	std::string about = "|ABOUT";
	std::string quit = "|QUIT";
	std::string create_word_set = "|CREATE WORD SET";
	print_title();
	print_line(START, END);
	print_menu_option(start, "1");
	print_menu_option(about, "2");
	print_menu_option(create_word_set, "3");
	print_menu_option(quit, "4");
	print_line(START, END);

}

void print_start_menu(sqlite3* db) {
	clear_screen();
	print_title();

	print_line(START, END);
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, "SELECT setId, setName FROM wordSet", -1, &stmt, nullptr);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::string setName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		std::string setId = std::to_string(sqlite3_column_int(stmt, 0));
		print_menu_option(setName, setId);
	}
	sqlite3_finalize(stmt);
	print_menu_option("Go Back", "-1");
	print_line(START, END);
}

void print_definition(const std::string& definition) {
	std::istringstream iss(definition);
	std::string word;
	std::vector<std::string> words;

	// Split the string into words
	while (iss >> word) {
		words.push_back(word);
	}
	std::string line = "|  ";
	for (const auto& w : words) {
		// Check if adding the next word would exceed the line length
		if (line.length() + w.length() + 2 > 79) { // 79 because "|  " adds 2 at the start and 1 for space, and we need room for the closing "|"
			std::cout << line;
			std::cout << std::string(80 - line.length() - 1, ' ') << "|" << std::endl; // Padding and closing
			line = "|  " + w + " ";
		}
		else {
			line += w + " ";
		}
	}

	// Print the last line if there's any remaining content
	if (!line.empty()) {
		std::cout << line;
		std::cout << std::string(80 - line.length() - 1, ' ') << "|" << std::endl; // Padding and closing
	}
}

void print_about() {
	clear_screen();
	print_line(START, END);
	std::cout << "|  Hangman is a word-guessing game where one player thinks of a word and the   |" << std::endl;
	std::cout << "|  other player tries to guess it by suggesting letters. The word is           |" << std::endl;
	std::cout << "|  represented by a series of blank spaces, each corresponding to a letter     |" << std::endl;
	std::cout << "|  in the word. A part of a stick figure (the hangman) is drawn for each       |" << std::endl;
	std::cout << "|  incorrect letter guessed. If a correct letter is chosen, all the blank      |" << std::endl;
	std::cout << "|  spaces belonging to that letter will be filled. The game is won if the      |" << std::endl;
	std::cout << "|  guesser identifies the word before completing the hangman drawing.          |" << std::endl;
	std::cout << "|  Conversely, the game is lost if the entire hangman is drawn before the      |" << std::endl;
	std::cout << "|  word is guessed. In this version of the game, the player's word will be     |" << std::endl;
	std::cout << "|  chosen at random from a database. The player will also be given the         |" << std::endl;
	std::cout << "|  definition of the word that is to be guessed. The game also gives the       |" << std::endl;
	std::cout << "|  player an option to create their own set of words.                          |" << std::endl;
	std::cout << "|                                                                              |" << std::endl;
	std::cout << "|                                                                              |" << std::endl;
	std::cout << "|  GO BACK                                                               -1    |" << std::endl;
	print_line(START, END);
}


//*******************Game Functions**********************************

Game::Game(sqlite3* db, int id) : db(db), setId(id), max_word_id(0), stage(0), current_word_id(0), guessing(nullptr) {
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

	//choosing a word from the database;
	int num = generate_random_number();

	getWordInfo(num);

}

// Destructor
Game::~Game() {
	delete[] guessing;
}

// draw_hangman method
void Game::draw_hangman() const{
	std::vector<std::string> hangmanStages = {
	"                                +---+\n"
	"                                |   |\n"
	"                                    |\n"
	"                                    |\n"
	"                                    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                                    |\n"
	"                                    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                                |   |\n"
	"                                    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                               /|   |\n"
	"                                    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                               /|\\  |\n"
	"                                    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                               /|\\  |\n"
	"                               /    |\n"
	"                                    |\n"
	"                               =========",
	"                                +---+\n"
	"                                |   |\n"
	"                                O   |\n"
	"                               /|\\  |\n"
	"                               / \\  |\n"
	"                                    |\n"
	"                               ========="
	};

	if (stage >= 0 && stage < hangmanStages.size()) {
		std::cout << hangmanStages[stage] << std::endl;
	}
	else {
		std::cerr << "Invalid stage: " << stage << std::endl;
	}
}


int Game::generate_random_number() {
	if (usedNumbers.size() >= max_word_id) {
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


	// Delete the old guessing array if it exists to avoid memory leaks
	delete[] guessing;

	// Allocate memory for the guessing array
	size_t length = word.length();
	guessing = new char[length];
	for (size_t i = 0; i < length; i++) {
		if (word[i] == ' ') guessing[i] = ' ';
		else guessing[i] = '_';
	}
}
void Game::print_game_view() {
	clear_screen();
	print_title();
	draw_hangman();
	print_line(START, END);
	print_definition(definition);
	print_line(START, END);
	std::cout << std::endl;
	print_line(START, END);
	print_definition(alphabet);
	print_line(START, END);
	std::cout << "Guess the word: ";
	for (size_t i = 0; i < word.length(); i++) {
		std::cout << guessing[i] << ' ';
	}
	std::cout << std::endl;
}
void Game::play_game() {
	char letter, chr;
	bool wordGuessed = false;
	while (stage < 6 && !wordGuessed) {
		print_game_view();
		std::cout << "Enter a letter: ";
		std::cin >> letter;

		// Convert to uppercase
		letter = toupper(letter);

		// Check if letter has already been used
		int index = letter - 'A';
		if (alphabet[index] != ' ') {
			std::cout << "You have already guessed that letter. Try again." << std::endl;
			continue;
		}

		// Update used letters array
		alphabet[index] = letter;

		bool inWord = false;
		for (size_t i = 0; i < word.length(); i++) {
			chr = toupper(word[i]);
			if (chr == letter) {
				guessing[i] = letter;
				inWord = true;
			}
		}

		if (!inWord) {
			stage++;
		}

		// Check if the entire word is guessed
		wordGuessed = true;
		for (size_t i = 0; i < word.length(); i++) {
			if (guessing[i] == '_') {
				wordGuessed = false;
				break;
			}
		}
	}

	print_game_view();
	if (wordGuessed) {
		std::cout << "Congratulations! You've guessed the word!" << std::endl;
		pause_console();
	}
	else {
		std::cout << "Game over! The word was: " << word << std::endl;
		pause_console();
	}
}

void create_wordSet(sqlite3* db) {

	clear_screen();
	print_title();
	print_line(START, END);
	std::string description = "Creating a new set of words, please follow the instructions below!";
	print_definition(description);
	print_line(START, END);

	std::string setName;
	int numOfWords;

	std::cout << "Enter the name of the set of words: ";
	std::cin >> setName;
	std::cout << "Enter the number of words you want to insert: ";
	std::cin >> numOfWords;
	std::cout << std::endl;

	sqlite3_stmt* stmt = nullptr;
	sqlite3_prepare_v2(db, "INSERT INTO wordSet(setName, numOfWords) VALUES (?, ?)", -1, &stmt, nullptr);
	sqlite3_bind_text(stmt, 1, setName.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 2, numOfWords);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	int id = static_cast<int>(sqlite3_last_insert_rowid(db));

	std::string wordName, def;
	for (int i = 0; i < numOfWords; i++) {
		std::cout << "Enter the word: ";
		std::cin >> wordName;
		std::cout << "Enter the definition: ";
		std::cin.ignore(); // Ignore the newline character left in the input buffer
		std::getline(std::cin, def);

		sqlite3_prepare_v2(db, "INSERT INTO Words(wordName, definition, setId) VALUES (?, ?, ?)", -1, &stmt, nullptr);
		sqlite3_bind_text(stmt, 1, wordName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(stmt, 2, def.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 3, id);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		std::cout << std::endl;
	}

	std::cout << "Congrats, the data has been entered! " << std::endl;
	pause_console();
}
