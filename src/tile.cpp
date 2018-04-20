#include "headers/tile.h"

using namespace std;

Tile::Tile()
{
    idTerrain = 0;
}

Tile::Tile(int _idTerrain)
{
    idTerrain = _idTerrain;
}

Tile::~Tile(){}

int Tile::getIdTerrain(void)
{
    return idTerrain;
}

void Tile::setIdTerrain(int _idTerrain)
{
    idTerrain = _idTerrain;
}

void Tile::printTile()
{
    cout << "Tile : idTerrain : " << idTerrain << endl;
}

