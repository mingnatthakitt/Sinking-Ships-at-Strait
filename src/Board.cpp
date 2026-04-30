#include "Board.h"
#include "Ship.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <set>

using namespace std;

/**
 * @brief Constructor for the Board class
 * @param size The dimension of the square grid
 */
Board::Board(int size) : size(size) {
  // Coding element 3: Dynamic memory management
  grid = new Cell *[size];
  for (int i = 0; i < size; ++i) {
    grid[i] = new Cell[size];
  }
}

/**
 * @brief Destructor for the Board class. Frees dynamic memory.
 */
Board::~Board() {
  for (int i = 0; i < size; ++i) {
    delete[] grid[i];
  }
  delete[] grid;
}

/**
 * @brief Initializes the grid with empty cells.
 */
void Board::initialize() {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      grid[i][j].setType(CellType::EMPTY);
      grid[i][j].setHit(false);
      grid[i][j].setShip(nullptr);
    }
  }
}

/**
 * @brief Randomly generates islands on the board.
 * @param count Number of islands to generate.
 */
void Board::generateIslands(int count) {
  // Coding element 1: Generation of random events
  // Patterns defined by relative coordinates {(dr, dc), ...}
  vector<vector<pair<int, int>>> patterns = {
      {{0, 0}, {1, 1}},                 // 1. Diagonal 2x2
      {{0, 0}, {1, 0}, {2, 0}, {2, 1}}, // 2. L-shape 3x2
      {{0, 0}, {0, 1}, {0, 2}, {0, 3}}, // 3. Block 4x1
      {{0, 1},
       {0, 2},
       {1, 0},
       {1, 1},
       {1, 2},
       {1, 3},
       {2, 1},
       {2, 2}} // 4. Block 3 rows (2,4,2)
  };

  int generated = 0;
  int attempts = 0;
  while (generated < count && attempts < 500) {
    attempts++;
    int r = rand() % (size - 4); // Buffer for largest pattern
    int c = rand() % (size - 4);
    int type = rand() % patterns.size();
    auto &pattern = patterns[type];

    // Ensure no islands touch: check 1-cell radius around every coordinate in
    // the pattern
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
      // Place the island cells
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

/**
 * @brief Returns the grid size.
 * @return int size
 */
int Board::getSize() const { return size; }

/**
 * @brief Returns a reference to the cell at (r, c).
 * @param r row index
 * @param c column index
 * @return Cell&
 */
Cell &Board::getCell(int r, int c) { return grid[r][c]; }

/**
 * @brief Checks if the given coordinates are within the board.
 * @param r row index
 * @param c column index
 * @return true if valid, false otherwise
 */
bool Board::isValid(int r, int c) const {
  return r >= 0 && r < size && c >= 0 && c < size;
}

/**
 * @brief Checks if a ship can be placed at the given location.
 * @param r start row index
 * @param c start column index
 * @param length ship length
 * @param vertical true if ship is vertical
 * @return true if placement is valid
 */
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

/**
 * @brief Places a ship pointer onto the grid.
 * @param r start row index
 * @param c start column index
 * @param length ship length
 * @param vertical true if vertical
 * @param ship pointer to the Ship object
 */
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

  // Temporarily remove to check placement
  removeShip(ship);

  if (canPlaceShip(newR, newC, ship, vert)) {
    placeShip(newR, newC, ship, vert, owner);
    ship->setPosition(newR, newC, vert);
    return true;
  } else {
    // Restore
    placeShip(oldR, oldC, ship, vert, owner);
    return false;
  }
}

/**
 * @brief Displays the board to the terminal with ANSI colors.
 * @param isEnemyView if true, hides unhit ships.
 * @param title Title of the board display.
 */
void Board::display(int cursorR, int cursorC, string title,
                    string status, Ship *activeShip) {
  // Clear screen for interactivity
  cout << "\033[2J\033[H";

  cout << BOLD << YELLOW << "  === " << title << " ===" << RESET << "\n";
  if (!status.empty()) {
    cout << BOLD << CYAN << "  " << status << RESET << "\n";
  }
  cout << "  (Arrows: Move/Cursor, W: Change Ship, R: Toggle Move Mode, "
               "Space/Enter: Action, S: Save Game, Q: Quit)\n";

  // Count undetected enemy submarines
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

  // Print top border
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

  // Print bottom border
  cout << "   +" << string(size * 3, '-') << "+\n";

  // Legend
  cout << BOLD << WHITE << "  LEGEND:\n" << RESET;
  cout << "  [S],[I] Submarine      - Bullet: Arch (1x1)\n";
  cout << "  [B]     Big Boat       - Bullet: Piercing Torpedo (Line)\n";
  cout << "  [M]     Missile Vessel - Bullet: Arch (3x3)\n";
  cout << "  [C]     Cruiser        - Bullet: Arch (1x1)\n";
  cout << "  [L]     Long Boat      - Bullet: Piercing Torpedo (Line)\n";
  cout << "\n";
}
