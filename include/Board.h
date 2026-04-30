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
    
    
    int getSize() const;
    Cell& getCell(int r, int c);
    bool isValid(int r, int c) const;

    bool canPlaceShip(int r, int c, Ship* ship, bool vertical);
    void placeShip(int r, int c, Ship* ship, bool vertical, int owner);
    void removeShip(Ship* ship);
    bool tryMoveShip(Ship* ship, int dr, int dc, int owner);

    void display(int cursorR, int cursorC, string title, string status = "", Ship* activeShip = nullptr);

private:
    int size;
    Cell** grid; 
};

#endif 