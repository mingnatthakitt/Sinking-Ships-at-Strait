#include "Cell.h"
#include "Ship.h"
#include "Common.h"

using namespace std;

Cell::Cell() : type(CellType::EMPTY), hit(false), shipPtr(nullptr), ownerID(NO_OWNER) {}

Cell::~Cell() {}

void Cell::setType(CellType type) {
    this->type = type;
}

CellType Cell::getType() const {
    return type;
}

void Cell::setHit(bool hit) {
    this->hit = hit;
}

bool Cell::isHit() const {
    return hit;
}

void Cell::setShip(Ship* ship) {
    this->shipPtr = ship;
}

Ship* Cell::getShip() const {
    return shipPtr;
}

string Cell::display(bool isCursor, bool isSelected) const {
    string icon = " ~ "; // default water
    string color = BLUE;

    if (type == CellType::ISLAND) {
        icon = " ^ ";
        color = GREEN;
    } else if (type == CellType::SHIP) {
        if (shipPtr) {
            icon = " ";
            icon += shipPtr->getIcon();
            icon += " ";
        } else {
            icon = " S ";
        }
        color = (ownerID == PLAYER_HUMAN) ? MAGENTA : RED_BOLD;
    } else if (type == CellType::SUBMARINE) {
        if (ownerID == PLAYER_HUMAN) {
            if (shipPtr) {
                icon = " ";
                icon += shipPtr->getIcon();
                icon += " ";
            } else {
                icon = " I ";
            }
            color = MAGENTA;
        } else {
            // AI Submarine hidden unless hit OR it has fired enough shots to be revealed
            bool revealed = hit || (shipPtr && !shipPtr->isHidden());
            if (revealed) {
                if (shipPtr) {
                    icon = " ";
                    icon += shipPtr->getIcon();
                    icon += " ";
                } else {
                    icon = " S ";
                }
                color = RED_BOLD;
            } else {
                icon = " ~ ";
                color = BLUE;
            }
        }
    }

    if (hit) {
        if (type == CellType::SHIP || type == CellType::SUBMARINE) {
            string icon = " X ";
            string color = RED;
            if (isSelected) color = YELLOW; // Highlight selected ship even if hit
            return color + icon + RESET;
        } else if (type == CellType::ISLAND) {
            return YELLOW " # " RESET;
        } else {
            return BLUE " o " RESET;
        }
    }

    if (isCursor) return HIGHLIGHT_BG + icon + RESET;
    if (isSelected && (type == CellType::SHIP || type == CellType::SUBMARINE)) {
        return YELLOW + icon + RESET;
    }
    return color + icon + RESET;
}
