#include "GameManager.h"
#include <iostream>

using namespace std;

int main() {
    try {
        GameManager game;
        game.start();
    } catch (...) {
        InputHandler::disableRawMode();
        cerr << "An unexpected error occurred. Terminal restored.\n";
    }
    return 0;
}
