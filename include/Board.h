#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "Cell.h"

using namespace std;

class Board {
public:
    Board(int size = GRID_SIZE);
    ~Board();

    void initialize();
    void generateIslands(int count);
    
    // Grid management
    int getSize() const;
    Cell& getCell(int r, int c);
    bool isValid(int r, int c) const;

    // Ship management
    bool canPlaceShip(int r, int c, Ship* ship, bool vertical);
    void placeShip(int r, int c, Ship* ship, bool vertical, int owner);
    void removeShip(Ship* ship);
    bool tryMoveShip(Ship* ship, int dr, int dc, int owner);

    // Display
    void display(int cursorR, int cursorC, string title, string status = "", Ship* activeShip = nullptr);

private:
    int size;
    Cell** grid; // 2D array of Cells (Dynamic memory requirement)
};

#endif // BOARD_H
