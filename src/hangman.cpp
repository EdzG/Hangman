#include <iostream>
#include "functions.h"
#include "db.h"

int main() {
    console_init();

    sqlite3* db = db_open("hangman.db");
    if (!db) {
        std::cerr << "Could not open the database. Exiting." << std::endl;
        return 1;
    }

    int game_choice = 1;
    while (game_choice != 4) {
        clear_screen();
        print_menu();
        std::cin >> game_choice;

        if (game_choice == 1) {
            int option;
            print_start_menu(db);
            std::cin >> option;
            if (option == -1) continue;
            else {
                Game hangman_game(db, option);
                hangman_game.play_game();
            }
        }
        else if (game_choice == 2) {
            print_about();
            std::cin >> game_choice;
        }
        else if (game_choice == 3) {
            create_wordSet(db);
        }
        else break;
    }

    sqlite3_close(db);
    return 0;
}
