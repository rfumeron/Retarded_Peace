#include <iostream>
#include <string>
#include <unistd.h>

#include "game.h"

int main(int argc, char* argv[])
{
    // GenericInfantry genericinfantery  = GenericInfantry("infanterie");
    // GenericArtillery genericartillery = GenericArtillery("artillery");

  Game game("map0");
  game.initGame();
  game.StartGame();

  // game.setMapFile("map1");
  // game.StartGame();

  return EXIT_SUCCESS;
}
