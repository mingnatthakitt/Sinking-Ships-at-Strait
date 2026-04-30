#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"
#include "InputHandler.h"
#include "Player.h"
#include <string>
using namespace std;

enum class GameMode { SHOOT, MOVE };

class GameManager {
public:
  GameManager();
  ~GameManager();

  void start();
  void setupGame();
  void gameLoop();
  void handlePlayerAction();
  void handleBotTurn();
  void saveGame();
  bool loadGame(const string &filename);
  Ship *createShip(int typeID);

private:
  Board sharedBoard;
  Player *human;
  Player *bot;
  bool gameOver;
  int turnCounter;
  int cursorR, cursorC;
  int activeShipIndex;
  GameMode currentMode;
  int movesLeft;
  int selectedLevel;
  vector<int> extraShipIndices;

  void showLevelMenu();
  void showShipSelection();
  void placeShipsRandomly(Player *p, const vector<int> &extras);
  void performPiercingTorpedo(int startR, int startC, int dr, int dc, int attackerID, Ship* activeShip = nullptr);
  void performArchShot(int r, int c, Ship *ship);
  void saveGameHistory(const string &winner);
  string inputString(const string &prompt);
};

#endif
