#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <unordered_set>
#include <sstream>
#include <cctype>

#include <sqlite3.h>
#include "console.h"

class Formatting {
public:

	//Converts all the characters of Input to UpperCase
	static void	charactersCase(char*);
	static void color(int);
};

//************CONSTANTS to pass to the color function (ANSI SGR codes)*******************
static const int RED = 31;
static const int PURPLE = 35;
const int GREEN = 32;
const int AQUA = 36;
const int YELLOW = 33;
const int WHITE = 37;
const int BLUE = 34;

//lenght of the screen of the game
const int START = 0, END = 80;


void print_line(int start, int end);
void print_menu_option(const std::string& str, const std::string& choice);
void print_menu();
void print_title();
void print_about();
void print_start_menu(sqlite3* db);
void print_definition(const std::string& definition);
void create_wordSet(sqlite3* db);


class Game {

public:
	Game(sqlite3* db, int id);
	~Game();
	void draw_hangman() const;
	int generate_random_number();
	void print_game_view();
	void play_game();
	void getWordInfo(int num);
private:
	sqlite3* db; // non-owning
	int setId;
	int max_word_id;
	int stage; // stage of the hangman (max stage is 6)
	char alphabet[26];
	int current_word_id;
	char* guessing;
	std::string word;
	std::string definition;
	std::unordered_set<int> usedNumbers; // to track used random numbers
};
