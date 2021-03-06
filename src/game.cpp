#include "game.h"

using namespace std;

Game::Game()
{

}

Game::Game(string _mapFile)
{
    new Player();
    new Player();
    new Player();
    new Player();
    new Player();

    gameMap = Map(_mapFile);
    mapFile = _mapFile;
    libImages = VectorImage();
    gameDisplay = Display(&gameMap, &libImages, 20, 20, 32, 32);
    gameRunning = false;

    state = SELECTION;
    currentPlayer = 1;
}

Game::~Game()
{

}

void Game::setMapFile(string _mapFile)
{
    mapFile = _mapFile;
    gameMap = Map(mapFile);
    gameDisplay.resizeWindow();
}

void Game::initGame(void)
{
    gameDisplay.startDisplay();

    libImages.initVector();
    libImages.loadImage("data/textures/land.png");
    libImages.loadImage("data/textures/sea.png");
    libImages.loadImage("data/textures/highlight.png");
    libImages.loadImage("data/textures/cursor.png");
    libImages.loadImage("data/textures/grey.png");
    libImages.loadImage("data/textures/red.png");

    new Sprit("infantery");
    new Sprit("artillery");
    new Sprit("helicopter");

    new Terrain(0, "plaine");
    new Terrain(1, "mer");

    new Infantery("infantery");
    new Artillery("artillery");
    new Helicopter("helicopter");

    //gameMap.getTile(4, 4)->setUnit(playerOne->creatUnit(Rect(4, 4), "infantery"));
    //gameMap.getTile(2, 2)->setUnit(playerTwo->creatUnit(Rect(2, 2), "infantery"));
    //gameMap.getTile(2, 3)->setUnit(playerOne->creatUnit(Rect(2, 3), "artillery"));

}

bool Game::StartGame(void)
{
    gameRunning = true;
    //gameThread = new thread(&Game::loop, this);
    loop();
}

bool Game::StopGame(void)
{
    gameRunning = false;
    //gameThread->join();
    //delete gameThread;
}

void Game::getAllowedMoves(Unit* unit, vector<Rect>* allowedMoves)
{
    int i, j;
    int coord_X, coord_Y;
    int nmbMovePoints = TypeUnit::getTypeUnit(unit->getStrType())->getMovePoints();
    allowedMoves->resize(0);
    int tabSize = (nmbMovePoints*2 + 1);
    vector<vector<int>> moves = vector<vector<int>>(tabSize, vector<int>(tabSize));
    for(i = 0; i < tabSize; i++)
    {
        for(j = 0; j < tabSize; j++)
        {
            moves[i][j] = -1;
        }
    }

    moves[tabSize/2][tabSize/2] = nmbMovePoints;
    verifMoves(unit->getPosition(), tabSize/2, tabSize/2, nmbMovePoints, unit, &moves);

    for (i = 0; i < tabSize; i++)
    {
        for(j = 0; j < tabSize; j++)
        {
            coord_X = unit->getPosition().getX() - (tabSize/2 - i);
            coord_Y = unit->getPosition().getY() - (tabSize/2 - j);
            if((coord_X >= 0) && (coord_X < gameMap.getNmbTilesX()) && (coord_Y >= 0) && (coord_Y < gameMap.getNmbTilesY()))
            {
                if((moves[i][j] >= 0) && (moves[i][j] < nmbMovePoints) && (gameMap.getUnitFromTiles(coord_X, coord_Y) == nullptr))
                {
                    Rect addRect = Rect(coord_X, coord_Y);
                    allowedMoves->push_back(addRect);
                }
            }
        }
    }
}

void Game::printAllowedMoves(vector<Rect>* allowedMoves)
{
    for(int i = 0; i < allowedMoves->size(); i++)
    {
        cout << "Move " << i << " -> X : " << (*allowedMoves)[i].getX() << " Y : " << (*allowedMoves)[i].getY() << endl;
    }
}

void Game::getAllowedAttack(Unit* unit, vector<Rect>* allowedAttacks)
{
    int delta_X, delta_Y;
    int coord_X, coord_Y;
    int minRange = TypeUnit::getTypeUnit(unit->getStrType())->getMinRange();
    int maxRange = TypeUnit::getTypeUnit(unit->getStrType())->getMaxRange();
    Rect unitPosition = unit->getPosition();
    allowedAttacks->resize(0);
    int tabSize = (maxRange*2 + 1);
    for (int i = 0; i < tabSize; i++)
    {
        for(int j = 0; j < tabSize; j++)
        {
            delta_X = abs(tabSize/2 - i);
            delta_Y = abs(tabSize/2 - j);
            coord_X = unitPosition.getX() - (tabSize/2 - i);
            coord_Y = unitPosition.getY() - (tabSize/2 - j);

            if((coord_X >= 0)
                && (coord_X < gameMap.getNmbTilesX())
                && (coord_Y >= 0)
                && (coord_Y < gameMap.getNmbTilesY())
                && (delta_X + delta_Y <= maxRange)
                && (delta_X + delta_Y >= minRange)
                && (delta_X + delta_Y > 0))
            {
                allowedAttacks->push_back(Rect(coord_X, coord_Y));
            }
        }
    }
}

void Game::getAllowedAttack(Unit* unit, Rect position, vector<Rect>* allowedAttacks)
{
    int delta_X, delta_Y;
    int coord_X, coord_Y;
    int minRange = TypeUnit::getTypeUnit(unit->getStrType())->getMinRange();
    int maxRange = TypeUnit::getTypeUnit(unit->getStrType())->getMaxRange();
    Rect unitPosition = unit->getPosition();
    allowedAttacks->resize(0);
    int tabSize = (maxRange*2 + 1);
    for (int i = 0; i < tabSize; i++)
    {
        for(int j = 0; j < tabSize; j++)
        {
            delta_X = abs(tabSize/2 - i);
            delta_Y = abs(tabSize/2 - j);
            coord_X = position.getX() - (tabSize/2 - i);
            coord_Y = position.getY() - (tabSize/2 - j);

            if((coord_X >= 0)
                && (coord_X < gameMap.getNmbTilesX())
                && (coord_Y >= 0)
                && (coord_Y < gameMap.getNmbTilesY())
                && (delta_X + delta_Y <= maxRange)
                && (delta_X + delta_Y >= minRange)
                && (delta_X + delta_Y > 0))
            {
                allowedAttacks->push_back(Rect(coord_X, coord_Y));
            }
        }
    }
}

void Game::attack(Unit* aggressor, Unit* defender, bool counterattack)
{
    string aggressorType = aggressor->getStrType();
    string defenderType = defender->getStrType();
    TypeUnit* aggressorTypeUnit = TypeUnit::getTypeUnit(aggressorType);
    TypeUnit* defenderTypeUnit = TypeUnit::getTypeUnit(defenderType);
    int basicAttackValue = aggressorTypeUnit->getAttackValue(defenderType);
    int defenseValue = gameMap.getTerrainFromTiles(defender->getPosition().getX(), defender->getPosition().getY())->getDefenseValue();
    int attackValue = basicAttackValue*(10 - defenseValue)*(aggressor->getPV())/1000;
    cout << endl;
    cout << "INFORMATIONS SUR l'ATTAQUE" << endl;
    cout << "L'unite " << aggressorType << " aux coordonnees [" << aggressor->getPosition().getX() << "; " << aggressor->getPosition().getY() <<"] du Joueur " << aggressor->getOwner()
    << " attaque l'unite " << defenderType << " aux coordonnees [" << defender->getPosition().getX() << "; " << defender->getPosition().getY() << "]" << endl;
    cout << "Elle lui inflige " << attackValue << " points de degats" << endl;
    if(defender->getPV() - attackValue <= 0)
    {
        cout << "L'unite attaquee a ete detruite" << endl;
        gameMap.getTile(defender->getPosition().getX(), defender->getPosition().getY())->setUnit(nullptr);
        Player::getPlayerFromId(defender->getOwner())->deleteUnit(defender);
    } else {
        defender->setPV(defender->getPV() - attackValue);
        if(counterattack == true)
        {
            basicAttackValue = defenderTypeUnit->getAttackValue(aggressorType);
            defenseValue = gameMap.getTerrainFromTiles(aggressor->getPosition().getX(), aggressor->getPosition().getX())->getDefenseValue();
            attackValue = basicAttackValue*0.8*(10 - defenseValue)*(defender->getPV())/1000;
            cout << "L'unite attaquee contre-attaque !" << endl;
            cout << "Elle inflige " << attackValue << " points de degats a l'attaquant" << endl;
            if(aggressor->getPV() - attackValue <= 0)
            {
                cout << "L'unite attaquante a ete detruite" << endl;
                gameMap.getTile(aggressor->getPosition().getX(), aggressor->getPosition().getY())->setUnit(nullptr);
                Player::getPlayerFromId(aggressor->getOwner())->deleteUnit(aggressor);
            } else {
                aggressor->setPV(aggressor->getPV() - attackValue);
            }
        }
    }
}

void Game::cursorLeft(void)
{
    Rect position = gameDisplay.getCursorPosition();
    if (position.getX() > 0)
    {
        position.setX(position.getX() - 1);
        gameDisplay.setCursorPosition(position);

        if (gameDisplay.getScreenPosition().getX() == position.getX())
            gameDisplay.moveLeft();
    }
}
void Game::cursorRight(void)
{
    Rect position = gameDisplay.getCursorPosition();
    if (position.getX() < gameMap.getNmbTilesX() - 1)
    {
        position.setX(position.getX() + 1);
        gameDisplay.setCursorPosition(position);

        if (gameDisplay.getScreenPosition().getX() + gameDisplay.getTileScreenX() - 1 == position.getX())
            gameDisplay.moveRight();
    }
}
void Game::cursorUp(void)
{
    Rect position = gameDisplay.getCursorPosition();
    if (position.getY() > 0)
    {
        position.setY(position.getY() - 1);
        gameDisplay.setCursorPosition(position);

        if (gameDisplay.getScreenPosition().getY() == position.getY())
            gameDisplay.moveUp();
    }
}
void Game::cursorDown(void)
{
    Rect position = gameDisplay.getCursorPosition();
    if (position.getY() < gameMap.getNmbTilesY() - 1)
    {
        position.setY(position.getY() + 1);
        gameDisplay.setCursorPosition(position);

        if (gameDisplay.getScreenPosition().getY() + gameDisplay.getTileScreenY() - 1 == position.getY())
            gameDisplay.moveDown();
    }
}

void Game::loop()
{
    int movespeed = MOVE_SPEED_CURSOR;
    int delta_X, delta_Y;
    unsigned int waitingTime;
    bool counterattack = false;
    bool keydown = false;
    vector<Rect> moves = vector<Rect>();
    vector<Rect> attacks = vector<Rect>();

    Unit* currentUnit;
    Unit* targetUnit;
    Rect cursorPosition = Rect(0, 0);
    Rect oldPosition = Rect();

    SDL_Event event;
    clock_t t;

    gameDisplay.enableCursor();
    gameDisplay.updateDisplay();

    cout << "Game running !" << endl;
    cout << "Turn of player " << currentPlayer << ((currentPlayer == 1)?" (Red)":" (Blue)") << endl;

    while (gameRunning == true)
    {
        t = clock();

        cursorPosition = gameDisplay.getCursorPosition();

        while(SDL_PollEvent(&event))
        {
            cursorPosition = gameDisplay.getCursorPosition();

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    gameRunning = false;
                }
            }

            if (state == SELECTION)
            {

                currentUnit = gameMap.getTile(cursorPosition.getX(), cursorPosition.getY())->getUnit();

                if (event.type == SDL_KEYDOWN)
                {
                    if (movespeed == 0)
                    {
                        if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            cursorLeft();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            cursorRight();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_UP)
                        {
                            cursorUp();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            cursorDown();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if(event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            Player::getPlayerFromId(currentPlayer)->setCursorPosition(cursorPosition);
                            Player::getPlayerFromId(currentPlayer)->setScreenPosition(gameDisplay.getScreenPosition());
                            do {
                                Player::getPlayerFromId(currentPlayer)->refreshUnit();
                                if (currentPlayer == Player::getNmbPlayer()-1)
                                {
                                    currentPlayer = 1;
                                } else
                                {
                                    currentPlayer += 1;
                                }
                            } while(Player::getPlayerFromId(currentPlayer)->getNumberUnit() == 0);

                            gameDisplay.setCursorPosition(Player::getPlayerFromId(currentPlayer)->getCursorPosition());
                            gameDisplay.setScreenPosition(Player::getPlayerFromId(currentPlayer)->getScreenPosition());

                            cout << "Turn of player " << currentPlayer << ((currentPlayer == 1)?" (Red)":" (Blue)") << endl;
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if(keydown == true)
                        {
                            gameMap.clearVectorHighlight();
                            movespeed = MOVE_SPEED_CURSOR;
                            keydown = false;
                        }
                        if(event.key.keysym.sym == SDLK_a)
                        {
                            if(currentUnit != nullptr)
                            {
                                getAllowedAttack(currentUnit, &attacks);
                                gameMap.updateVectorHighlight(attacks, RED);
                                movespeed = MOVE_SPEED_CURSOR;
                                keydown = true;
                            }
                        }
                        if(event.key.keysym.sym == SDLK_i)
                        {
                            if(currentUnit != nullptr)
                            {
                                currentUnit->printUnit();
                                movespeed = MOVE_SPEED_CURSOR;
                            }
                        }
                    }

                    if (event.key.keysym.sym == SDLK_SPACE && movespeed == 0)
                    {
                        if (currentUnit != nullptr
                            && currentUnit->getOwner() == currentPlayer
                            && !currentUnit->hasMoved())
                        {
                            getAllowedMoves(currentUnit, &moves);
                            gameMap.updateVectorHighlight(moves, BLUE);

                            state = DESTINATION;
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                    }
                }
            } else if (state == DESTINATION)
            {
                if (event.type == SDL_KEYDOWN)
                {
                    if (movespeed == 0)
                    {
                        if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            cursorLeft();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            cursorRight();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_UP)
                        {
                            cursorUp();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            cursorDown();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if(keydown == true)
                        {
                            gameMap.clearVectorHighlight(RED);
                            gameMap.updateVectorHighlight(moves, BLUE);
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if(event.key.keysym.sym == SDLK_a)
                        {
                            if(currentUnit != nullptr)
                            {
                                getAllowedAttack(currentUnit, cursorPosition, &attacks);
                                gameMap.updateVectorHighlight(attacks, RED);
                                movespeed = MOVE_SPEED_CURSOR;
                                keydown = true;
                            }
                        }
                        if (event.key.keysym.sym == SDLK_SPACE)
                        {

                            movespeed = MOVE_SPEED_CURSOR;
                            if (cursorPosition == currentUnit->getPosition())
                            {
                                gameMap.clearVectorHighlight();
                                getAllowedAttack(currentUnit, &attacks);
                                if (TypeUnit::getTypeUnit(currentUnit->getStrType())->canAttack() && attacks.size())
                                {
                                    gameMap.updateVectorHighlight(attacks, RED);
                                    state = ATTACK;
                                } else
                                {
                                    state = SELECTION;
                                }
                            } else if(gameMap.getHighlight(cursorPosition) == BLUE)
                            {

                                gameMap.clearVectorHighlight();
                                oldPosition = currentUnit->getPosition();
                                gameMap.moveUnit(currentUnit->getPosition(), cursorPosition);

                                state = MOVE;
                            } else
                            {
                                gameMap.clearVectorHighlight();
                                state = SELECTION;
                            }
                        }
                    }
                }
            } else if (state == MOVE)
            {
                if (TypeUnit::getTypeUnit(currentUnit->getStrType())->canMoveAttack())
                {

                    getAllowedAttack(currentUnit, &attacks);
                    gameMap.updateVectorHighlight(attacks, RED);

                    if (attacks.size())
                    {
                        state = MOVE_ATTACK;
                    } else {
                        gameMap.clearVectorHighlight();
                        state = SELECTION;
                    }
                } else {
                    if (event.type == SDL_KEYDOWN && movespeed == 0)
                    {
                        if (event.key.keysym.sym == SDLK_SPACE)
                        {
                            currentUnit->setMoved(true);
                            movespeed = MOVE_SPEED_CURSOR;
                            state = SELECTION;
                        }
                        if (event.key.keysym.sym == SDLK_ESCAPE && movespeed == 0)
                        {
                            gameMap.moveUnit(currentUnit->getPosition(), oldPosition);
                            movespeed = MOVE_SPEED_CURSOR;
                            state = SELECTION;
                        }
                    }
                }
            } else if (state == ATTACK)
            {
                if (event.type == SDL_KEYDOWN)
                {
                    if (movespeed == 0)
                    {
                        if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            cursorLeft();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            cursorRight();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_UP)
                        {
                            cursorUp();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            cursorDown();
                            movespeed = MOVE_SPEED_CURSOR;
                        }

                    }

                    if (event.key.keysym.sym == SDLK_SPACE && movespeed == 0)
                    {
                        targetUnit = gameMap.getUnitFromTiles(cursorPosition.getX(), cursorPosition.getY());

                        if(targetUnit != nullptr)
                        {
                            delta_X = abs(currentUnit->getPosition().getX() - targetUnit->getPosition().getX());
                            delta_Y = abs(currentUnit->getPosition().getY() - targetUnit->getPosition().getY());
                            if(((delta_X + delta_Y) <= 1) && (TypeUnit::getTypeUnit(targetUnit->getStrType())->getMinRange() <= 1))
                            {
                                counterattack = true;
                            } else {
                                counterattack = false;
                            }

                            if (cursorPosition == currentUnit->getPosition())
                            {
                                gameMap.clearVectorHighlight();
                                movespeed = MOVE_SPEED_CURSOR;
                                state = SELECTION;
                            } else if ((gameMap.getHighlight(cursorPosition) == RED)
                                && (targetUnit != nullptr)
                                && (targetUnit->getOwner() != currentPlayer))
                            {
                                attack(currentUnit, targetUnit, counterattack);
                                gameMap.clearVectorHighlight();
                                state = SELECTION;
                                movespeed = MOVE_SPEED_CURSOR;
                                currentUnit->setMoved(true);
                                counterattack = false;
                            }
                        }
                    }
                }
            } else if (state == MOVE_ATTACK)
            {
                if (event.type == SDL_KEYDOWN)
                {
                    if (movespeed == 0)
                    {
                        if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            cursorLeft();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            cursorRight();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_UP)
                        {
                            cursorUp();
                            movespeed = MOVE_SPEED_CURSOR;
                        }
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            cursorDown();
                            movespeed = MOVE_SPEED_CURSOR;
                        }

                    }

                    if (event.key.keysym.sym == SDLK_SPACE && movespeed == 0)
                    {
                        targetUnit = gameMap.getUnitFromTiles(cursorPosition.getX(), cursorPosition.getY());

                        if(targetUnit != nullptr)
                        {
                            delta_X = abs(currentUnit->getPosition().getX() - targetUnit->getPosition().getX());
                            delta_Y = abs(currentUnit->getPosition().getY() - targetUnit->getPosition().getY());
                            if(((delta_X + delta_Y) <= 1) && (TypeUnit::getTypeUnit(targetUnit->getStrType())->getMinRange() <= 1))
                            {
                                counterattack = true;
                            } else {
                                counterattack = false;
                            }
                            if (cursorPosition == currentUnit->getPosition())
                            {
                                gameMap.clearVectorHighlight();
                                movespeed = MOVE_SPEED_CURSOR;
                                currentUnit->setMoved(true);
                                state = SELECTION;
                            } else if ((gameMap.getHighlight(cursorPosition) == RED)
                                && (targetUnit != nullptr)
                                && (targetUnit->getOwner() != currentPlayer))
                            {
                                attack(currentUnit, targetUnit, counterattack);
                                gameMap.clearVectorHighlight();
                                state = SELECTION;
                                movespeed = MOVE_SPEED_CURSOR;
                                currentUnit->setMoved(true);
                                counterattack = false;
                            }
                        }
                    }
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        gameMap.clearVectorHighlight();
                        gameMap.moveUnit(currentUnit->getPosition(), oldPosition);
                        movespeed = MOVE_SPEED_CURSOR;
                        state = SELECTION;
                    }
                }
            }
        }

        gameDisplay.updateDisplay();

        if (movespeed != 0) movespeed--;

        t = clock() - t;

        waitingTime = (unsigned int)((1.0/FRAMERATE-(double)t/CLOCKS_PER_SEC)*1000000);
        if (waitingTime > 1000000/FRAMERATE) waitingTime = 0;

        if (waitingTime > 0)
        {
            usleep(waitingTime);
        }
    }
}

int Game::verifMoves(Rect src, int posTabX, int posTabY, int remainingMoves, Unit* unit, vector<vector<int>>* moves)
{
    if(remainingMoves > 0)
    {
        int malusLeft, malusUp, malusRight, malusDown;
        bool testLeft, testUp, testRight, testDown;

        if (src.getX() - 1 >= 0
        && (gameMap.getUnitFromTiles(src.getX() - 1, src.getY()) == nullptr || gameMap.getUnitFromTiles(src.getX() - 1, src.getY())->getOwner() == unit->getOwner()))
        {
            malusLeft = unit->getMoveMalus(gameMap.getTile(src.getX() - 1, src.getY())->getNameTerrain());
            testLeft = ((*moves)[posTabX - 1][posTabY] < remainingMoves) && (malusLeft >= 0) && (remainingMoves - malusLeft >= 0);
        } else {
            testLeft = false;
        }
        if (src.getY() - 1 >= 0
        && (gameMap.getUnitFromTiles(src.getX(), src.getY() - 1) == nullptr || gameMap.getUnitFromTiles(src.getX(), src.getY() - 1)->getOwner() == unit->getOwner()))
        {
            malusUp = unit->getMoveMalus(gameMap.getTile(src.getX(), src.getY() - 1)->getNameTerrain());
            testUp = ((*moves)[posTabX][posTabY - 1] < remainingMoves) && (malusUp >= 0) && (remainingMoves - malusUp >= 0);
        } else {
            testUp = false;
        }
        if (src.getX() + 1 < gameMap.getNmbTilesX()
        && (gameMap.getUnitFromTiles(src.getX() + 1, src.getY()) == nullptr || gameMap.getUnitFromTiles(src.getX() + 1, src.getY())->getOwner() == unit->getOwner()))
        {
            malusRight = unit->getMoveMalus(gameMap.getTile(src.getX() + 1, src.getY())->getNameTerrain());
            testRight = ((*moves)[posTabX + 1][posTabY] < remainingMoves) && (malusRight >= 0) && (remainingMoves - malusRight >= 0);
        } else {
            testRight = false;
        }
        if (src.getY() + 1 < gameMap.getNmbTilesY()
        && (gameMap.getUnitFromTiles(src.getX(), src.getY() + 1) == nullptr || gameMap.getUnitFromTiles(src.getX(), src.getY() + 1)->getOwner() == unit->getOwner()))
        {
            malusDown = unit->getMoveMalus(gameMap.getTile(src.getX(), src.getY() + 1)->getNameTerrain());
            testDown = ((*moves)[posTabX][posTabY + 1] < remainingMoves) && (malusDown >= 0) && (remainingMoves - malusDown >= 0);
        } else {
            testDown = false;
        }

        if(testLeft)
        {
            (*moves)[posTabX - 1][posTabY] = remainingMoves - malusLeft;
        }
        if(testUp)
        {
            (*moves)[posTabX][posTabY - 1] = remainingMoves - malusUp;
        }
        if(testRight)
        {
            (*moves)[posTabX + 1][posTabY] = remainingMoves - malusRight;
        }
        if(testDown)
        {
            (*moves)[posTabX][posTabY + 1] = remainingMoves - malusDown;
        }

        if(testLeft)
        {
            verifMoves(Rect(src.getX() - 1, src.getY()), posTabX - 1, posTabY, remainingMoves - malusLeft, unit, moves);
        }
        if(testUp)
        {
            verifMoves(Rect(src.getX(), src.getY() - 1), posTabX, posTabY - 1, remainingMoves - malusUp, unit, moves);
        }
        if(testRight)
        {
            verifMoves(Rect(src.getX() + 1, src.getY()), posTabX + 1, posTabY, remainingMoves - malusRight, unit, moves);
        }
        if(testDown)
        {
            verifMoves(Rect(src.getX(), src.getY() + 1), posTabX, posTabY + 1, remainingMoves - malusDown, unit, moves);
        }
    } else { return 1;}
    return 0;
}
