#ifndef GAME_H
#define GAME_H

#include <thread>
#include <string>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <math.h>

#include "map.h"
#include "unit.h"
#include "typeunit.h"
#include "display.h"
#include "vector_image.h"
#include "SDL.h"
#include "player.h"

#include "infantery.h"
#include "artillery.h"
#include "helicopter.h"

#define FRAMERATE 60
#define MOVE_SPEED_CURSOR 2

enum State {
    SELECTION, DESTINATION, MOVE, MOVE_ATTACK, ATTACK
};

using namespace std;

class Game{

public:

  Game();
  Game(string _mapFile);
  ~Game();

  void setMapFile(string _mapFile);

  bool StartGame(void);
  void initGame(void);
  bool StopGame(void);

  void cursorLeft(void);
  void cursorRight(void);
  void cursorUp(void);
  void cursorDown(void);

  void getAllowedMoves(Unit* unit, vector<Rect>* allowedMoves);
  void printAllowedMoves(vector<Rect>* allowedMoves);
  void getAllowedAttack(Unit* unit, vector<Rect>* allowedAttacks);
  void getAllowedAttack(Unit* unit, Rect position, vector<Rect>* allowedAttacks);

  void attack(Unit* aggressor, Unit* defender, bool counterattack);

protected:

  string mapFile;
  VectorImage libImages;
  thread* gameThread;
  Map gameMap;
  Display gameDisplay;
  bool gameRunning;

  State state;
  int currentPlayer;

  virtual void loop();
  int verifMoves(Rect src, int posTabX, int posTabY, int remainingMoves, Unit* unit, vector<vector<int>>* moves);

};

#endif
