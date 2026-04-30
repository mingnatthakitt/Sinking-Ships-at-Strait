#include "Board.h"
#include "Ship.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <set>

using namespace std;


Board::Board(int size) : size(size) {
  
  grid = new Cell *[size];
  for (int i = 0; i < size; ++i) {
    grid[i] = new Cell[size];
  }
}


Board::~Board() {
  for (int i = 0; i < size; ++i) {
    delete[] grid[i];
  }
  delete[] grid;
}


void Board::initialize() {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      grid[i][j].setType(CellType::EMPTY);
      grid[i][j].setHit(false);
      grid[i][j].setShip(nullptr);
    }
  }
}


void Board::generateIslands(int count) {

  vector<vector<pair<int, int>>> patterns = {
      {{0, 0}, {1, 1}},                 
      {{0, 0}, {1, 0}, {2, 0}, {2, 1}}, 
      {{0, 0}, {0, 1}, {0, 2}, {0, 3}}, 
      {{0, 1},
       {0, 2},
       {1, 0},
       {1, 1},
       {1, 2},
       {1, 3},
       {2, 1},
       {2, 2}} 
  };

  int generated = 0;
  int attempts = 0;
  while (generated < count && attempts < 500) {
    attempts++;
    int r = rand() % (size - 4); // Buffer for largest pattern
    int c = rand() % (size - 4);
    int type = rand() % patterns.size();
    auto &pattern = patterns[type];

    bool tooClose = false;
    for (auto &p : pattern) {
      int targetR = r + p.first;
      int targetC = c + p.second;

      for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
          int checkR = targetR + dr;
          int checkC = targetC + dc;
          if (isValid(checkR, checkC)) {
            if (grid[checkR][checkC].getType() == CellType::ISLAND) {
              tooClose = true;
              break;
            }
          }
        }
        if (tooClose)
          break;
      }
      if (tooClose)
        break;
    }

    if (!tooClose) {
      // island cells
      for (auto &p : pattern) {
        int nr = r + p.first;
        int nc = c + p.second;
        if (isValid(nr, nc)) {
          grid[nr][nc].setType(CellType::ISLAND);
        }
      }
      generated++;
    }
  }
}


int Board::getSize() const { return size; }


Cell &Board::getCell(int r, int c) { return grid[r][c]; }


bool Board::isValid(int r, int c) const {
  return r >= 0 && r < size && c >= 0 && c < size;
}


bool Board::canPlaceShip(int r, int c, Ship *ship, bool vertical) {
  int w = ship->getWidth();
  int h = ship->getHeight();
  if (vertical)
    swap(w, h);

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int nr = r + i;
      int nc = c + j;
      if (!isValid(nr, nc) || grid[nr][nc].getType() != CellType::EMPTY) {
        return false;
      }
    }
  }
  return true;
}

void Board::placeShip(int r, int c, Ship *ship, bool vertical, int owner) {
  int w = ship->getWidth();
  int h = ship->getHeight();
  if (vertical)
    swap(w, h);

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int nr = r + i;
      int nc = c + j;
      grid[nr][nc].setType(ship->getName() == "Submarine" ? CellType::SUBMARINE
                                                          : CellType::SHIP);
      grid[nr][nc].setShip(ship);
      grid[nr][nc].setOwner(owner);
      grid[nr][nc].setHit(false); // Reset hit status for new position
    }
  }
}

void Board::removeShip(Ship *ship) {
  int r = ship->getStartR();
  int c = ship->getStartC();
  int w = ship->getWidth();
  int h = ship->getHeight();
  if (ship->isVertical())
    swap(w, h);

  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      int nr = r + i;
      int nc = c + j;
      if (isValid(nr, nc) && grid[nr][nc].getShip() == ship) {
        grid[nr][nc].setType(CellType::EMPTY);
        grid[nr][nc].setShip(nullptr);
        grid[nr][nc].setOwner(NO_OWNER);
      }
    }
  }
}

bool Board::tryMoveShip(Ship *ship, int dr, int dc, int owner) {
  int oldR = ship->getStartR();
  int oldC = ship->getStartC();
  bool vert = ship->isVertical();

  int newR = oldR + dr;
  int newC = oldC + dc;

  // heck placement
  removeShip(ship);

  if (canPlaceShip(newR, newC, ship, vert)) {
    placeShip(newR, newC, ship, vert, owner);
    ship->setPosition(newR, newC, vert);
    return true;
  } else {
    
    placeShip(oldR, oldC, ship, vert, owner);
    return false;
  }
}


void Board::display(int cursorR, int cursorC, string title,
                    string status, Ship *activeShip) {

  cout << "\033[2J\033[H";

  cout << BOLD << YELLOW << "  === " << title << " ===" << RESET << "\n";
  if (!status.empty()) {
    cout << BOLD << CYAN << "  " << status << RESET << "\n";
  }
  cout << "  (Arrows: Move/Cursor, W: Change Ship, R: Toggle Move Mode, "
               "Space/Enter: Action, S: Save Game, Q: Quit)\n";

  // undetected enemy submarines
  set<Ship *> hiddenSubs;
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (grid[i][j].getType() == CellType::SUBMARINE &&
          grid[i][j].getOwner() == PLAYER_AI) {
        Ship *s = grid[i][j].getShip();
        if (s && s->isHidden()) {
          hiddenSubs.insert(s);
        }
      }
    }
  }

  if (!hiddenSubs.empty()) {
    cout << BOLD << ORANGE << "  WARNING!!! " << hiddenSubs.size()
              << " invisible enemy submarines undetected" << RESET << "\n";
  } else {
    cout << "\n";
  }

  
  cout << "   +" << string(size * 3, '-') << "+\n";

  for (int i = 0; i < size; ++i) {
    cout << " |";
    for (int j = 0; j < size; ++j) {
      bool isCursor = (i == cursorR && j == cursorC);
      bool isSelected =
          (activeShip != nullptr && grid[i][j].getShip() == activeShip);
      cout << grid[i][j].display(isCursor, isSelected);
    }
    cout << "|\n";
  }

  
  cout << "   +" << string(size * 3, '-') << "+\n";


  cout << BOLD << WHITE << "  LEGEND:\n" << RESET;
  cout << "  [S],[I] Submarine      - Bullet: Arch (1x1)\n";
  cout << "  [B]     Big Boat       - Bullet: Piercing Torpedo (Line)\n";
  cout << "  [M]     Missile Vessel - Bullet: Arch (3x3)\n";
  cout << "  [C]     Cruiser        - Bullet: Arch (1x1)\n";
  cout << "  [L]     Long Boat      - Bullet: Piercing Torpedo (Line)\n";
  cout << "\n";
}
