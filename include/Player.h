#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "Board.h"
#include "Ship.h"
using namespace std;

class Player {
public:
    Player(string name, int ownerID);
    virtual ~Player();

    string getName() const { return name; }
    int getOwnerID() const { return ownerID; }
    vector<Ship*>& getShips() { return ships; }

    void addShip(Ship* ship);
    bool allShipsSunk() const;

    virtual bool canStillPlay() const;
    void clearShips();

protected:
    string name;
    int ownerID;
    vector<Ship*> ships;
};

#endif // PLAYER_H
