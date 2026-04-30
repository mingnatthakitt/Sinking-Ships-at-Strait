#include "Ship.h"
using namespace std;

Ship::Ship(string name, int w, int h, bool movable, WeaponType weapon)
    : name(name), width(w), height(h), health(w * h), movable(movable), 
      weapon(weapon), startR(-1), startC(-1), vertical(false), shotsFired(0) {}

Ship::~Ship() {}

void Ship::setPosition(int r, int c, bool vertical) {
    this->startR = r;
    this->startC = c;
    this->vertical = vertical;
}

void Ship::takeHit() {
    if (health > 0)
        health--;
}
