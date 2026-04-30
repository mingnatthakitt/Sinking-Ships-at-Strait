#ifndef CELL_H
#define CELL_H

#include "Common.h"

using namespace std;

class Ship; 

class Cell {
public:
    Cell();
    ~Cell();

    void setType(CellType type);
    CellType getType() const;

    void setHit(bool hit);
    bool isHit() const;

    void setShip(Ship* ship);
    Ship* getShip() const;

    void setOwner(int owner) { ownerID = owner; }
    int getOwner() const { return ownerID; }

    // Display functionality
    string display(bool isCursor, bool isSelected = false) const;

private:
    CellType type;
    bool hit;
    Ship* shipPtr;
    int ownerID;
};

#endif 
