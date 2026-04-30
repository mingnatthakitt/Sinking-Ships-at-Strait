#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <vector>
#include "Common.h"
using namespace std;

enum class ShipType { SUBMARINE = 0, BIGBOAT, MISSILEVESSEL, CRUISER, LONGBOAT };

class Ship {
public:
    Ship(string name, int w, int h, bool movable, WeaponType weapon);
    virtual ~Ship();

    string getName() const { return name; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getHealth() const { return health; }
    bool isMovable() const { return movable; }
    WeaponType getWeapon() const { return weapon; }
    bool isSunk() const { return health <= 0; }

    void setPosition(int r, int c, bool vertical);
    int getStartR() const { return startR; }
    int getStartC() const { return startC; }
    bool isVertical() const { return vertical; }

    virtual void takeHit();
    virtual void recordShot() { shotsFired++; }
    int getShotsFired() const { return shotsFired; }

    void setHealth(int h) { health = h; }
    void setShotsFired(int s) { shotsFired = s; }

    virtual string getDescription() const = 0;
    virtual char getIcon() const = 0;
    virtual ShipType getType() const = 0;
    
    // Pattern for weapons
    virtual vector<pair<int, int>> getWeaponPattern() const {
        return {{0, 0}}; // Default
    }

    // New: Sinks after firing
    virtual bool sinksOnFire() const { return false; }
    
    // New: Stealth logic
    virtual bool isHidden() const { return false; }

protected:
    string name;
    int width, height;
    int health;
    bool movable;
    WeaponType weapon;
    int startR, startC;
    bool vertical;
    int shotsFired;
};

// Submarine: 3x1; projectile shot; invisible until 2nd shot fire
class Submarine : public Ship {
public:
    Submarine() : Ship("Submarine", 3, 1, false, WeaponType::ARCH) {}
    string getDescription() const override { return "Stealth (3x1). Hidden until 2nd shot fired. Arch only."; }
    bool isHidden() const override { return health > 0 && shotsFired < 2; }
    char getIcon() const override { return isHidden() ? 'I' : 'S'; }
    ShipType getType() const override { return ShipType::SUBMARINE; }
};

// Big Boat: 3x2; straight shot;
class BigBoat : public Ship {
public:
    BigBoat() : Ship("Big Boat", 3, 2, false, WeaponType::TORPEDO) {}
    string getDescription() const override { return "Juggernaut (3x2). Piercing Torpedo only."; }
    char getIcon() const override { return 'B'; }
    ShipType getType() const override { return ShipType::BIGBOAT; }
};

// Missile Vessel: 1x1; 3x3 projectile; Sink after fire.
class MissileVessel : public Ship {
public:
    MissileVessel() : Ship("Missile Vessel", 1, 1, false, WeaponType::ARCH) {}
    string getDescription() const override { return "Kamikaze (1x1). 3x3 blast, sinks after firing."; }
    vector<pair<int, int>> getWeaponPattern() const override {
        vector<pair<int, int>> p;
        for(int r=-1; r<=1; r++) for(int c=-1; c<=1; c++) p.push_back({r,c});
        return p;
    }
    bool sinksOnFire() const override { return true; }
    char getIcon() const override { return 'M'; }
    ShipType getType() const override { return ShipType::MISSILEVESSEL; }
};

// Cruiser: 2x1; projectile shot
class Cruiser : public Ship {
public:
    Cruiser() : Ship("Cruiser", 2, 1, false, WeaponType::ARCH) {}
    string getDescription() const override { return "Precision Lobber (2x1). Arch shot only."; }
    char getIcon() const override { return 'C'; }
    ShipType getType() const override { return ShipType::CRUISER; }
};

// 5. Long Boat: 3x1; straight shot
class LongBoat : public Ship {
public:
    LongBoat() : Ship("Long Boat", 3, 1, false, WeaponType::TORPEDO) {}
    string getDescription() const override { return "Sniper (3x1). Piercing Torpedo only."; }
    char getIcon() const override { return 'L'; }
    ShipType getType() const override { return ShipType::LONGBOAT; }
};

#endif
