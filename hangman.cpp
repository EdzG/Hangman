#include <iostream>
#include "functions.h"


int main() {
    int game_choice = 1; 
    while (game_choice != 4) {
        system("cls"); 
        print_menu();
        std::cin >> game_choice;
        
        if (game_choice == 1) {
            int option; 
            print_start_menu(); 
            std::cin >> option; 
            if (option == -1) continue;
            else {
                Game hangman_game(option);
                hangman_game.play_game(); 
            }
        }
        else if (game_choice == 2) {
            print_about();
            std::cin >> game_choice;
        }
        else if (game_choice == 3) {
            create_wordSet();
        }
        else exit(0);
    }
   
    
   
    return 0;
}