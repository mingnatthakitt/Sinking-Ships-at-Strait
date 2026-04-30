#include "Player.h"
#include <iostream>
#include <algorithm>
using namespace std;

Player::Player(string name, int ownerID) : name(name), ownerID(ownerID) {
}

Player::~Player() {
    for (Ship* s : ships)
        delete s;
}


void Player::addShip(Ship* ship) {
    ships.push_back(ship);
}

bool Player::allShipsSunk() const {
    if (ships.empty()) return false;
    for (Ship* s : ships) {
        if (!s->isSunk()) return false;
    }
    return true;
}

bool Player::canStillPlay() const {
    return !allShipsSunk();
}

void Player::clearShips() {
    for (Ship* s : ships) {
        delete s;
    }
    ships.clear();
}
