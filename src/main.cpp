#include "Game.h"

int main(){
    Game game("path/to/config"); //calls the constructor -> calls the init() method from the constructor
    game.run(); // runs the main loop
    return 0;
}