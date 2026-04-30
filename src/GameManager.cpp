#include "GameManager.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

using namespace std;

GameManager::GameManager()
    : sharedBoard(GRID_SIZE), human(nullptr), bot(nullptr), gameOver(false),
      turnCounter(1), cursorR(0), cursorC(0), activeShipIndex(0),
      currentMode(GameMode::SHOOT), movesLeft(0), selectedLevel(1) {}

GameManager::~GameManager() {
  delete human;
  delete bot;
  InputHandler::disableRawMode();
}

void GameManager::start() {
  srand(static_cast<unsigned int>(time(nullptr)));
  InputHandler::enableRawMode();
  showLevelMenu();
  showShipSelection();
  setupGame();
  gameLoop();
}

void GameManager::showLevelMenu() {
  int cursor = 0;
  Board bgBoard(GRID_SIZE);
  bgBoard.initialize();
  bgBoard.generateIslands(5);

  // Place stationary multi-cell ships
  // Length 2
  bgBoard.getCell(0, 0).setType(CellType::SHIP);
  bgBoard.getCell(0, 0).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(0, 1).setType(CellType::SHIP);
  bgBoard.getCell(0, 1).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(24, 20).setType(CellType::SHIP);
  bgBoard.getCell(24, 20).setOwner(PLAYER_AI);
  bgBoard.getCell(24, 21).setType(CellType::SHIP);
  bgBoard.getCell(24, 21).setOwner(PLAYER_AI);
  bgBoard.getCell(18, 10).setType(CellType::SHIP);
  bgBoard.getCell(18, 10).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(18, 11).setType(CellType::SHIP);
  bgBoard.getCell(18, 11).setOwner(PLAYER_HUMAN);

  // Length 3
  bgBoard.getCell(0, 20).setType(CellType::SHIP);
  bgBoard.getCell(0, 20).setOwner(PLAYER_AI);
  bgBoard.getCell(0, 21).setType(CellType::SHIP);
  bgBoard.getCell(0, 21).setOwner(PLAYER_AI);
  bgBoard.getCell(0, 22).setType(CellType::SHIP);
  bgBoard.getCell(0, 22).setOwner(PLAYER_AI);
  bgBoard.getCell(10, 5).setType(CellType::SHIP);
  bgBoard.getCell(10, 5).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(10, 6).setType(CellType::SHIP);
  bgBoard.getCell(10, 6).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(10, 7).setType(CellType::SHIP);
  bgBoard.getCell(10, 7).setOwner(PLAYER_HUMAN);
  bgBoard.getCell(22, 3).setType(CellType::SHIP);
  bgBoard.getCell(22, 3).setOwner(PLAYER_AI);
  bgBoard.getCell(22, 4).setType(CellType::SHIP);
  bgBoard.getCell(22, 4).setOwner(PLAYER_AI);
  bgBoard.getCell(22, 5).setType(CellType::SHIP);
  bgBoard.getCell(22, 5).setOwner(PLAYER_AI);

  vector<string> titleRows = {
      "   ____ ___ _   _ _  _____ _   _  ____   ____  _   _ ___ ____  ____ ",
      "  / ___|_ _| \\ | | |/ /_ _| \\ | |/ ___| / ___|| |_| |_ _|  _ \\/ ___|",
      "  \\___ \\| ||  \\| | ' / | ||  \\| | |  _  \\___ \\|  _  || || |_) "
      "\\___ \\",
      "   ___) | || |\\  | . \\ | || |\\  | |_| |  ___) | | | || ||  __/ ___) "
      "|",
      "  |____/___|_| \\_|_|\\_\\___|_| \\_|\\____| |____/|_| |_|___|_|   "
      "|____/",
      "                                         @ S T R A I T          "};

  while (true) {
    cout << "\033[2J\033[H";

    // Draw frame top
    cout << "   +" << string(GRID_SIZE * 3, '-') << "+\n";

    for (int i = 0; i < GRID_SIZE; ++i) {
      cout << "   |";
      if (i >= 2 && i < 8) {
        // Title
        string row = titleRows[i - 2];
        int padding = (GRID_SIZE * 3 - row.length()) / 2;
        cout << CYAN << BOLD << string(padding, ' ') << row << string(GRID_SIZE * 3 - row.length() - padding, ' ') << RESET;
      } else if (i >= 12 && i < 16) {
        // Options
        vector<string> levels = {"Naval Training (Lv.1)", "Special Operation (Lv.2)", "World War (Lv.3)", "Continue Ongoing Battle"};
        string opt = (cursor == i - 12) ? "> " + levels[i - 12] + " <" : "  " + levels[i - 12] + "  ";
        int padding = (GRID_SIZE * 3 - opt.length()) / 2;
        if (cursor == i - 12)
          cout << GREEN << BOLD;
        cout << string(padding, ' ') << opt << string(GRID_SIZE * 3 - opt.length() - padding, ' ') << RESET;
      } else {
        for (int j = 0; j < GRID_SIZE; ++j) {
          cout << bgBoard.getCell(i, j).display(false);
        }
      }
      cout << "|\n";
    }

    // Draw frame bottom
    cout << "   +" << string(GRID_SIZE * 3, '-') << "+\n\n";
    cout << WHITE << "          (Use Arrows to select, Enter to confirm)"
         << RESET << "\n";

    Key k = InputHandler::getKey(); 
    if (k == Key::UP && cursor > 0)
      cursor--;
    if (k == Key::DOWN && cursor < 3)
      cursor++;
    if (k == Key::QUIT) {
      InputHandler::disableRawMode();
      exit(0);
    }
    if (k == Key::ENTER || k == Key::SPACE) {
      if (cursor == 3) {
        string filename = inputString("Input game name: ");
        if (loadGame(filename)) {
          gameLoop();
          return; // Exit after game loop finishes
        } else {
          cout << RED << "\n  Save file not found! Press any key..." << RESET;
          InputHandler::getKey();
          continue;
        }
      }
      selectedLevel = cursor + 1;
      break;
    }
  }
}

void GameManager::showShipSelection() {
  vector<string> shipNames = {"Submarine", "Big Boat", "Missile Vessel", "Cruiser", "Long Boat"};
  int cursor = 0;
  extraShipIndices.clear();

  while (true) {
    cout << "\033[2J\033[H";
    cout << BOLD << YELLOW << "  === FLEET CUSTOMIZATION ===" << RESET << "\n";
    cout << "  Default: Cruiser (Arch 1x1), Long Boat (Torpedo Line)\n";
    cout << "  Select 2 additional ships (Any of the five):\n\n";

    vector<string> bulletTypes = {"Arch (1x1)", "Torpedo (Line)", "Arch (3x3)", "Arch (1x1)", "Torpedo (Line)"};

    for (int i = 0; i < 5; ++i) {
      bool isSelected = false;
      for (int idx : extraShipIndices)
        if (idx == i)
          isSelected = true;

      if (cursor == i)
        cout << GREEN << "  > ";
      else
        cout << "    ";

      if (isSelected)
        cout << "[X] ";
      else
        cout << "[ ] ";

      cout << left << setw(15) << shipNames[i] << CYAN
           << " (Bullet: " << bulletTypes[i] << ")" << RESET << "\n";
    }

    cout << "\n  Selected: " << extraShipIndices.size() << "/2";
    if (extraShipIndices.size() == 2)
      cout << GREEN << " (Press ENTER to Start)" << RESET;
    cout << "\n  (Arrows: Move, Space: Select, Enter: Confirm)\n";

    Key k = InputHandler::getKey();
    if (k == Key::UP && cursor > 0)
      cursor--;
    if (k == Key::DOWN && cursor < 4)
      cursor++;
    if (k == Key::SPACE) {
      auto it = find(extraShipIndices.begin(), extraShipIndices.end(), cursor);
      if (it != extraShipIndices.end()) {
        extraShipIndices.erase(it);
      } else if (extraShipIndices.size() < 2) {
        extraShipIndices.push_back(cursor);
      }
    }
    if (k == Key::QUIT) {
      InputHandler::disableRawMode();
      exit(0);
    }
    if ((k == Key::ENTER) && extraShipIndices.size() == 2) {
      break;
    }
  }
}

void GameManager::setupGame() {
  sharedBoard.initialize();
  human = new Player("Commander Human", PLAYER_HUMAN);
  bot = new Player("Bot Nexus", PLAYER_AI);

  // Multiple Levels
  // Level 1: 5, Level 2: 8, Level 3: 11
  int islandCount = 2 + (selectedLevel * 3);
  sharedBoard.generateIslands(islandCount);

  placeShipsRandomly(human, extraShipIndices);

  // Bot gets extra ships based on difficulty (Lv1: 4, Lv2: 5, Lv3: 6)
  vector<int> botExtras;
  int numBotExtras = 1 + selectedLevel;
  for (int i = 0; i < numBotExtras; ++i) {
    botExtras.push_back(rand() % 5);
  }
  placeShipsRandomly(bot, botExtras);
}

void GameManager::placeShipsRandomly(Player *p, const vector<int> &extras) {
  vector<Ship *> fleet;
  // Defaults
  fleet.push_back(new Cruiser());
  fleet.push_back(new LongBoat());

  // Extras
  for (int idx : extras) {
    if (idx == 0)
      fleet.push_back(new Submarine());
    else if (idx == 1)
      fleet.push_back(new BigBoat());
    else if (idx == 2)
      fleet.push_back(new MissileVessel());
    else if (idx == 3)
      fleet.push_back(new Cruiser());
    else
      fleet.push_back(new LongBoat());
  }

  for (Ship *s : fleet) {
    bool placed = false;
    while (!placed) {
      int r = rand() % GRID_SIZE;
      int c = rand() % GRID_SIZE;
      bool vert = rand() % 2;

      int w = s->getWidth();
      int h = s->getHeight();
      if (vert)
        swap(w, h);

      bool tooClose = false;
      for (int i = -1; i <= h; ++i) {
        for (int j = -1; j <= w; ++j) {
          int nr = r + i;
          int nc = c + j;
          if (sharedBoard.isValid(nr, nc)) {
            CellType t = sharedBoard.getCell(nr, nc).getType();
            if (t == CellType::SHIP || t == CellType::SUBMARINE) {
              tooClose = true;
              break;
            }
          }
        }
        if (tooClose)
          break;
      }

      if (!tooClose && sharedBoard.canPlaceShip(r, c, s, vert)) {
        sharedBoard.placeShip(r, c, s, vert, p->getOwnerID());
        s->setPosition(r, c, vert);
        p->addShip(s);
        placed = true;
      }
    }
  }
}

void GameManager::gameLoop() {
  while (!gameOver) {
    Ship *active =
        human->getShips()[activeShipIndex % human->getShips().size()];
    while (active->isSunk()) {
      activeShipIndex++;
      active = human->getShips()[activeShipIndex % human->getShips().size()];
    }

    string weaponName = (active->getWeapon() == WeaponType::TORPEDO) ? "PIERCING TORPEDO" : "ARCH PROJECTILE";
    string statusStr =
        (currentMode == GameMode::SHOOT) ? weaponName : "MOVE MODE - " + to_string(movesLeft) + " REMAINING MOVES";
    sharedBoard.display(
        cursorR, cursorC, "SINKING SHIPS @STRAIT",
        "SELECTED: " + active->getName() + " [" + statusStr + "]", active);

    cout << MAGENTA << " Commander's Fleet: "
         << count_if(human->getShips().begin(), human->getShips().end(), [](Ship *s) { return !s->isSunk(); })
         << " | " << RED << "Bot's Fleet: " << count_if(bot->getShips().begin(), bot->getShips().end(), [](Ship *s) { return !s->isSunk(); })
         << RESET << " | Turn: " << turnCounter << "\n";

    Key k = InputHandler::getKey();
    bool turnEnded = false;

    switch (k) {
    case Key::UP:
      if (currentMode == GameMode::MOVE) {
        if (sharedBoard.tryMoveShip(active, -1, 0, PLAYER_HUMAN))
          movesLeft--;
      } else if (cursorR > 0)
        cursorR--;
      break;
    case Key::DOWN:
      if (currentMode == GameMode::MOVE) {
        if (sharedBoard.tryMoveShip(active, 1, 0, PLAYER_HUMAN))
          movesLeft--;
      } else if (cursorR < GRID_SIZE - 1)
        cursorR++;
      break;
    case Key::LEFT:
      if (currentMode == GameMode::MOVE) {
        if (sharedBoard.tryMoveShip(active, 0, -1, PLAYER_HUMAN))
          movesLeft--;
      } else if (cursorC > 0)
        cursorC--;
      break;
    case Key::RIGHT:
      if (currentMode == GameMode::MOVE) {
        if (sharedBoard.tryMoveShip(active, 0, 1, PLAYER_HUMAN))
          movesLeft--;
      } else if (cursorC < GRID_SIZE - 1)
        cursorC++;
      break;
    case Key::WEAPON_TOGGLE:
      do {
        activeShipIndex = (activeShipIndex + 1) % human->getShips().size();
      } while (human->getShips()[activeShipIndex]->isSunk());
      break;
    case Key::MODE_TOGGLE:
      if (currentMode == GameMode::SHOOT) {
        currentMode = GameMode::MOVE;
        movesLeft = 2;
      } else {
        currentMode = GameMode::SHOOT;
        movesLeft = 0;
      }
      break;
    case Key::SPACE:
    case Key::ENTER:
      if (currentMode == GameMode::SHOOT) {
        handlePlayerAction();
        turnEnded = true;
      } else if (movesLeft < 2) {
        // Confirm movement turn if made one move
        turnEnded = true;
      }
      break;
    case Key::SAVE_GAME:
      saveGame();
      break;
    case Key::QUIT:
      InputHandler::disableRawMode();
      exit(0);
      break;
    default:
      break;
    }

    if (currentMode == GameMode::MOVE && movesLeft == 0) {
      turnEnded = true;
    }

    if (turnEnded) {
      if (!bot->allShipsSunk())
        handleBotTurn();
      turnCounter++;
      currentMode = GameMode::SHOOT;
      movesLeft = 0;
    }

    if (human->allShipsSunk()) {
      sharedBoard.display(cursorR, cursorC, "GAME OVER");
      cout << RED << "DEFEAT! They now control the strait" << RESET << endl;
      saveGameHistory("Bot Nexus");
      gameOver = true;
    } else if (bot->allShipsSunk()) {
      sharedBoard.display(cursorR, cursorC, "GAME OVER");
      cout << GREEN << "VICTORY! FREEDOM FREEDOM FREEDOM!!!!" << RESET << endl;
      saveGameHistory("Commander Human");
      gameOver = true;
    }
  }
}

void GameManager::handlePlayerAction() {
  Ship *active = human->getShips()[activeShipIndex % human->getShips().size()];

  if (active->getWeapon() == WeaponType::ARCH) {
    performArchShot(cursorR, cursorC, active);
  } else {
    // Torpedo requires Line-of-Sight from the active ship
    int dirR = 0, dirC = 0;
    int sr = active->getStartR();
    int sc = active->getStartC();
    int w = active->getWidth();
    int h = active->getHeight();
    if (active->isVertical())
      swap(w, h);

    bool aligned = false;
    int fireR = -1, fireC = -1;

    // Check horizontal alignment: cursor is in a row occupied by the ship
    if (cursorR >= sr && cursorR < sr + h) {
      if (cursorC < sc) {
        aligned = true;
        dirR = 0;
        dirC = -1;
        fireR = cursorR;
        fireC = sc;
      } else if (cursorC >= sc + w) {
        aligned = true;
        dirR = 0;
        dirC = 1;
        fireR = cursorR;
        fireC = sc + w - 1;
      }
    }

    // Check vertical alignment: cursor is in a column occupied by the ship
    if (!aligned && cursorC >= sc && cursorC < sc + w) {
      if (cursorR < sr) {
        aligned = true;
        dirR = -1;
        dirC = 0;
        fireR = sr;
        fireC = cursorC;
      } else if (cursorR >= sr + h) {
        aligned = true;
        dirR = 1;
        dirC = 0;
        fireR = sr + h - 1;
        fireC = cursorC;
      }
    }

    if (aligned) {
      performPiercingTorpedo(fireR, fireC, dirR, dirC, PLAYER_HUMAN, active);
    } else {
      sharedBoard.display(
          cursorR, cursorC,
          "OUT OF RANGE - ALIGN WITH " + active->getName() + "!", "", active);
      usleep(800000);
      return; // Don't advance turn or record shot
    }
  }

  active->recordShot();
  if (active->sinksOnFire()) {
    active->takeHit(); // Missile Vessel self-destruction
    while (active->getHealth() > 0)
      active->takeHit();
  }
}

void GameManager::performPiercingTorpedo(int startR, int startC, int dr, int dc, int attackerID, Ship *activeShip) {
  int r = startR + dr;
  int c = startC + dc;

  while (sharedBoard.isValid(r, c)) {
    // Animation
    sharedBoard.display(r, c, "TORPEDO IN TRANSIT...", "", activeShip);
    usleep(30000);

    Cell &target = sharedBoard.getCell(r, c);

    if (target.getType() == CellType::ISLAND) {
      break;
    }

    if (target.getShip()) {
      target.getShip()->takeHit();
    }

    target.setHit(true);

    r += dr;
    c += dc;
  }
}

void GameManager::performArchShot(int r, int c, Ship *ship) {
  auto pattern = ship->getWeaponPattern();
  for (auto p : pattern) {
    int tr = r + p.first;
    int tc = c + p.second;
    if (sharedBoard.isValid(tr, tc)) {
      sharedBoard.getCell(tr, tc).setHit(true);
      if (sharedBoard.getCell(tr, tc).getShip()) {
        sharedBoard.getCell(tr, tc).getShip()->takeHit();
      }
    }
  }
  // Animation for arch: flash area
  usleep(200000);
}

void GameManager::handleBotTurn() {
  Ship *active = bot->getShips()[rand() % bot->getShips().size()];
  while (active->isSunk()) {
    active = bot->getShips()[rand() % bot->getShips().size()];
  }

  string weaponName = (active->getWeapon() == WeaponType::TORPEDO) ? "PIERCING TORPEDO" : "ARCH PROJECTILE";
  sharedBoard.display(cursorR, cursorC, "BOT TURN: SELECTING TARGET...", "", active);
  usleep(1500000);

  int tr, tc;
  int dr = 0, dc = 0;

  if (active->getWeapon() == WeaponType::ARCH) {
    bool validTarget = false;
    int attempts = 0;
    while (!validTarget && attempts < 100) {
      attempts++;
      tr = rand() % GRID_SIZE;
      tc = rand() % GRID_SIZE;
      validTarget = true;
      auto pattern = active->getWeaponPattern();
      for (auto p : pattern) {
        int nr = tr + p.first;
        int nc = tc + p.second;
        if (sharedBoard.isValid(nr, nc)) {
          if (sharedBoard.getCell(nr, nc).getOwner() == PLAYER_AI) {
            validTarget = false;
            break;
          }
        }
      }
    }
  } else {
    vector<int> choices = {0, 1, 2, 3};
    random_shuffle(choices.begin(), choices.end());
    bool foundDir = false;

    for (int choice : choices) {
      int tempDr = 0, tempDc = 0;
      if (choice == 0)
        tempDr = -1;
      else if (choice == 1)
        tempDr = 1;
      else if (choice == 2)
        tempDc = -1;
      else
        tempDc = 1;

      bool hitsSelf = false;
      int r = active->getStartR() + tempDr;
      int c = active->getStartC() + tempDc;
      while (sharedBoard.isValid(r, c)) {
        if (sharedBoard.getCell(r, c).getType() == CellType::ISLAND)
          break;
        if (sharedBoard.getCell(r, c).getOwner() == PLAYER_AI) {
          hitsSelf = true;
          break;
        }
        r += tempDr;
        c += tempDc;
      }

      if (!hitsSelf) {
        dr = tempDr;
        dc = tempDc;
        foundDir = true;
        break;
      }
    }

    if (!foundDir) {
      // Fallback
      dr = -1; // Just pick one
    }

    tr = active->getStartR() + dr * 5;
    tc = active->getStartC() + dc * 5;
  }

  // Move cursor to bot's target for visibility
  cursorR = max(0, min(GRID_SIZE - 1, tr));
  cursorC = max(0, min(GRID_SIZE - 1, tc));
  sharedBoard.display(
      cursorR, cursorC, "BOT TURN",
      "TARGETING WITH " + active->getName() + " [" + weaponName + "]!", active);
  usleep(2000000);

  if (active->getWeapon() == WeaponType::ARCH) {
    performArchShot(cursorR, cursorC, active);
  } else {
    performPiercingTorpedo(active->getStartR(), active->getStartC(), dr, dc,
                           PLAYER_AI, active);
  }

  active->recordShot();
  if (active->sinksOnFire()) {
    active->takeHit();
    while (active->getHealth() > 0)
      active->takeHit();
  }

  sharedBoard.display(cursorR, cursorC, "BOT TURN COMPLETE.", "", active);
  usleep(1500000);
}

// input/output, saves the game result to a history file.
void GameManager::saveGameHistory(const string &winner) {
  ofstream outFile("data/history.txt", ios::app);
  if (outFile.is_open()) {
    outFile << "Level: " << selectedLevel << " | Winner: " << winner << " | Turns: " << turnCounter << " | Player Fleet: " << human->getShips().size() << "\n";
    outFile.close();
  }
}

string GameManager::inputString(const string &prompt) {
  InputHandler::disableRawMode();
  cout << "\n  " << prompt;
  string input;
  getline(cin, input);
  InputHandler::enableRawMode();
  return input;
}

void GameManager::saveGame() {
  string filename = inputString("Enter save filename: ");
  if (filename.empty())
    return;

  ofstream outFile("data/" + filename + ".sav");
  if (!outFile.is_open())
    return;

  // Save global state
  outFile << selectedLevel << " " << turnCounter << " " << (int)currentMode
          << " " << movesLeft << " " << cursorR << " " << cursorC << " " << activeShipIndex << "\n";

  // Save Human ships
  auto &humanShips = human->getShips();
  outFile << humanShips.size() << "\n";
  for (Ship *s : humanShips) {
    outFile << (int)s->getType() << " " << s->getHealth() << " "
            << s->getStartR() << " " << s->getStartC() << " "
            << (int)s->isVertical() << " " << s->getShotsFired() << "\n";
  }

  // Save Bot ships
  auto &botShips = bot->getShips();
  outFile << botShips.size() << "\n";
  for (Ship *s : botShips) {
    outFile << (int)s->getType() << " " << s->getHealth() << " "
            << s->getStartR() << " " << s->getStartC() << " "
            << (int)s->isVertical() << " " << s->getShotsFired() << "\n";
  }

  // Save Board
  outFile << GRID_SIZE << "\n";
  for (int i = 0; i < GRID_SIZE; ++i) {
    for (int j = 0; j < GRID_SIZE; ++j) {
      Cell &c = sharedBoard.getCell(i, j);
      int shipIndex = -1;
      if (c.getShip()) {
        auto &fleet = (c.getOwner() == PLAYER_HUMAN) ? humanShips : botShips;
        for (size_t k = 0; k < fleet.size(); ++k) {
          if (fleet[k] == c.getShip()) {
            shipIndex = k;
            break;
          }
        }
      }
      outFile << (int)c.getType() << " " << (int)c.isHit() << " " << c.getOwner() << " " << shipIndex << "\n";
    }
  }

  outFile.close();
  cout << "\n  Game saved to data/" << filename << ".sav. Press any key...";
  InputHandler::getKey();
}

bool GameManager::loadGame(const string &filename) {
  ifstream inFile("data/" + filename + ".sav");
  if (!inFile.is_open())
    return false;

  // Cleanup existing
  if (human)
    human->clearShips();
  if (bot)
    bot->clearShips();
  delete human;
  delete bot;

  human = new Player("Commander Human", PLAYER_HUMAN);
  bot = new Player("Bot Nexus", PLAYER_AI);

  int modeInt;
  inFile >> selectedLevel >> turnCounter >> modeInt >> movesLeft >> cursorR >>
      cursorC >> activeShipIndex;
  currentMode = (GameMode)modeInt;

  // Load Human ships
  size_t humanShipCount;
  inFile >> humanShipCount;
  for (size_t i = 0; i < humanShipCount; ++i) {
    int type, health, r, c, vert, shots;
    inFile >> type >> health >> r >> c >> vert >> shots;
    Ship *s = createShip(type);
    s->setHealth(health);
    s->setPosition(r, c, (bool)vert);
    s->setShotsFired(shots);
    human->addShip(s);
  }

  // Load Bot ships
  size_t botShipCount;
  inFile >> botShipCount;
  for (size_t i = 0; i < botShipCount; ++i) {
    int type, health, r, c, vert, shots;
    inFile >> type >> health >> r >> c >> vert >> shots;
    Ship *s = createShip(type);
    s->setHealth(health);
    s->setPosition(r, c, (bool)vert);
    s->setShotsFired(shots);
    bot->addShip(s);
  }

  // Load Board
  int size;
  inFile >> size; // Assume it matches GRID_SIZE
  sharedBoard.initialize();
  for (int i = 0; i < GRID_SIZE; ++i) {
    for (int j = 0; j < GRID_SIZE; ++j) {
      int type, hit, owner, shipIdx;
      inFile >> type >> hit >> owner >> shipIdx;
      Cell &c = sharedBoard.getCell(i, j);
      c.setType((CellType)type);
      c.setHit((bool)hit);
      c.setOwner(owner);
      if (shipIdx != -1) {
        auto &fleet =
            (owner == PLAYER_HUMAN) ? human->getShips() : bot->getShips();
        c.setShip(fleet[shipIdx]);
      }
    }
  }

  inFile.close();
  return true;
}

Ship *GameManager::createShip(int typeID) {
  switch ((ShipType)typeID) {
  case ShipType::SUBMARINE:
    return new Submarine();
  case ShipType::BIGBOAT:
    return new BigBoat();
  case ShipType::MISSILEVESSEL:
    return new MissileVessel();
  case ShipType::CRUISER:
    return new Cruiser();
  case ShipType::LONGBOAT:
    return new LongBoat();
  default:
    return new Cruiser();
  }
}
