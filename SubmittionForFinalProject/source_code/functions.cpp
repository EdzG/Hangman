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
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, k);
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

void print_start_menu() {
	system("cls");
	print_title(); 
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://localhost:3306", "root", "password");

	con->setSchema("hangman");
	 
	sql::Statement* stmt;
	stmt = con->createStatement();
	std::string selectDataSQL = "SELECT * FROM wordSet";
	sql::ResultSet* res = stmt->executeQuery(selectDataSQL);

	print_line(START, END);
	int count = 0;
	while (res->next()) {
		std::string setName = res->getString("setName");
		std::string setId = std::to_string(res->getInt("setId"));
		print_menu_option(setName, setId);
		count++;
	}
	print_menu_option("Go Back", "-1");
	print_line(START, END);
	delete res;
	delete stmt;
	delete con;
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
	system("cls");
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

Game::Game(int id) : setId(id), max_word_id(0), stage(0), driver(nullptr), con(nullptr), stmt(nullptr), res(nullptr), pstmt(nullptr), current_word_id(0) {
	for (int i = 0; i < 26; i++) {
		alphabet[i] = ' ';
	}

	// Connecting to the database
	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://localhost:3306", "root", "password");
	con->setSchema("hangman");
	stmt = con->createStatement();

	std::string selectDataSQL = "SELECT numOfWords FROM wordSet WHERE setId = " + std::to_string(setId);
	res = stmt->executeQuery(selectDataSQL);

	if (res->next()) {
		max_word_id = res->getInt("numOfWords");
	}
	//choosing a word from the database; 
	int num = generate_random_number(); 
	
	getWordInfo(num); 

}

// Destructor
Game::~Game() {
	if (res) delete res;
	if (stmt) delete stmt;
	if (con) delete con;
	if (pstmt) delete pstmt; 
	if (guessing) delete[] guessing;
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
	std::string selectDataSQL = "SELECT * FROM words WHERE setId = " + std::to_string(setId);
	res = stmt->executeQuery(selectDataSQL);
	bool gotInfo = false; 
	while (res->next()) {
		if (num == 1) {
			word = res->getString("wordName");
			definition = res->getString("definition");
			gotInfo = true; 
			break; 
		}
		num--;
	}
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
	system("cls"); 
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
		system("pause"); 
	}
	else {
		std::cout << "Game over! The word was: " << word << std::endl;
		system("pause");
	}
}

void create_wordSet() {

	system("cls"); 
	print_title(); 
	print_line(START, END); 
	std::string description = "Creating a new set of words, please follow the instructions below!"; 
	print_definition(description); 
	print_line(START, END); 
	// Connecting to the database
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::PreparedStatement* pstmt;

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://localhost:3306", "root", "password");
	con->setSchema("hangman");
	stmt = con->createStatement();

	std::string setName;
	int numOfWords;

	std::cout << "Enter the name of the set of words: ";
	std::cin >> setName;
	std::cout << "Enter the number of words you want to insert: ";
	std::cin >> numOfWords;
	std::cout << std::endl; 
	try {
		// Use a prepared statement to prevent SQL injection
		std::string insertDataSQL = "INSERT INTO wordSet(setName, numOfWords) VALUES (?, ?)";
		std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(insertDataSQL));
		pstmt->setString(1, setName);
		pstmt->setInt(2, numOfWords);
		pstmt->execute();
		pstmt.reset();

		std::string selectDataSQL = "SELECT setId FROM wordSet WHERE setName = ?";
		pstmt.reset(con->prepareStatement(selectDataSQL));
		pstmt->setString(1, setName);
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

		int id = 0;
		if (res->next()) {
			id = res->getInt("setId");
		}
		res.reset();
		pstmt.reset();

		std::string wordName, def;
		for (int i = 0; i < numOfWords; i++) {
			std::cout << "Enter the word: ";
			std::cin >> wordName;
			std::cout << "Enter the definition: ";
			std::cin.ignore(); // Ignore the newline character left in the input buffer
			std::getline(std::cin, def);

			insertDataSQL = "INSERT INTO Words(wordName, definition, setId) VALUES (?, ?, ?)";
			pstmt.reset(con->prepareStatement(insertDataSQL));
			pstmt->setString(1, wordName);
			pstmt->setString(2, def);
			pstmt->setInt(3, id);
			pstmt->execute();
			pstmt.reset();
			std::cout << std::endl; 
		}
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQL error: " << e.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	std::cout << "Congrats, the data has been entered! " << std::endl;
	system("pause");
}


