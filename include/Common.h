#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>

using namespace std;

enum class WeaponType { TORPEDO, ARCH };

// Grid configuration
const int GRID_SIZE = 25;

// ANSI Color for terminal visuals
#define RESET           "\033[0m"
#define RED             "\033[31m"
#define RED_BOLD        "\033[1;31m"
#define GREEN           "\033[32m"
#define YELLOW          "\033[33m"
#define BLUE            "\033[34m"
#define BLUE_BOLD       "\033[1;34m"
#define MAGENTA         "\033[35m"
#define CYAN            "\033[36m"
#define WHITE           "\033[37m"
#define BOLD            "\033[1m"
#define ORANGE          "\033[38;5;208m"
#define HIGHLIGHT_BG    "\033[47;30m" 

enum class CellType {
    EMPTY,
    ISLAND,
    SHIP,
    SUBMARINE
};

// Player identifiers
const int PLAYER_HUMAN = 0;
const int PLAYER_AI    = 1;
const int NO_OWNER     = -1;

enum class Key {
    UP, DOWN, LEFT, RIGHT,
    ENTER, SPACE,
    WEAPON_TOGGLE,
    MODE_TOGGLE,
    SAVE_GAME,
    QUIT,
    UNKNOWN
};

#endif 
