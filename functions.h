#pragma once
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <vector>
#include <random>
#include <unordered_set>
#include <sstream>
#include <cctype>

//database
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


class Formatting {
public:

	//Converts all the characters of Input to UpperCase
	static void	charactersCase(char*);
	static void color(int);
};

//************CONSTANTS to pass to the color function*******************
static const int RED = 12;
static const int PURPLE = 13;
const int GREEN = 10;
const int AQUA = 11;
const int YELLOW = 14;
const int WHITE = 15;
const int BLUE = 9;

//lenght of the screen of the game
const int START = 0, END = 80;


void print_line(int start, int end);
void print_menu_option(const std::string& str, const std::string& choice);
void print_menu();
void print_title();
void print_about();
void print_start_menu();
void print_definition(const std::string& definition);
void create_wordSet();


class Game {

public:
	Game(int id);
	~Game();
	void draw_hangman() const;
	int generate_random_number();
	void print_game_view();
	void play_game();
	void getWordInfo(int num);
private:
	int setId;
	int max_word_id;
	int stage; // stage of the hangman (max stage is 6)
	char alphabet[26];
	int current_word_id;
	char* guessing; 
	std::string word;
	std::string definition;
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::PreparedStatement* pstmt;
	std::unordered_set<int> usedNumbers; // to track used random numbers
};