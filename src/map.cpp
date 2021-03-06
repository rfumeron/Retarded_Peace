#include "headers/map.h"

vector<int> mapTiles = vector<int>();
vector<string> terrainName = vector<string>();

using namespace std;

Map::Map(){}

Map::Map(string mapName)
{
    std::vector<CaseHighlight> vectorHighlight = std::vector<CaseHighlight>();
    loadMapFromFile(mapName);
    clearVectorHighlight();
}

Map::Map(int _nmbTilesX, int _nmbTilesY, string _nameMap)
{
    nameMap = _nameMap;
    nmbTilesX = _nmbTilesX;
    nmbTilesY = _nmbTilesY;

    mapTiles.resize(_nmbTilesX*_nmbTilesY);
    vectorHighlight.resize(_nmbTilesX*_nmbTilesY);
    for (int i = 0; i<mapTiles.size(); i++)
    {
        mapTiles[i].setNameTerrain("mer");
    }
}

Map::~Map(){}

bool Map::loadMapFromFile(string mapName)
{
    char buf[128];
    int idTerrain;
    char strType[25];
    int idPlayer;
    int coord_X, coord_Y;
    char nameTerrain[256];
    ifstream mapFile;
    string mapPath = "data/maps/" + mapName + ".map";    //Be careful of mapPath (relative to Makefile)
    mapFile.open(mapPath);
    if(mapFile.bad() == true)
    {
        cout << "Erreur de chargement de la carte (fichier non ouvert)" << endl;
        return false;
    }
    mapFile.getline(buf, 128);
    while(sscanf(buf, "# %s\n", nameTerrain) > 0)
    {
      addNameTerrain(string(nameTerrain));
      mapFile.getline(buf, 128);
    }
    sscanf(buf, "$ %d %d\n", &nmbTilesX, &nmbTilesY);
    mapTiles.resize(nmbTilesX*nmbTilesY);
    for(int cmp = 0; cmp < nmbTilesY*nmbTilesX; cmp++)
    {
        coord_X = cmp%nmbTilesX;
        coord_Y = (cmp - coord_X)/nmbTilesX;
        if(mapFile.eof())
        {
            cout << "Erreur de chargement de la carte (fin de fichier atteint)" << endl;
            return false;
        }
        mapFile.getline(buf,128);
        sscanf(buf, "%d %s %d\n", &idTerrain, strType, &idPlayer);
        mapTiles[cmp].setNameTerrain(terrainName[idTerrain]);
        if(string(strType) != "none" && idPlayer != 0)
        {
        getTile(coord_X, coord_Y)->setUnit(Player::getPlayerFromId(idPlayer)->creatUnit(Rect(coord_X, coord_Y), strType));
        }
    }

    vectorHighlight.resize(nmbTilesX*nmbTilesY);
    clearVectorHighlight();

    return true;
}

int Map::getNmbTilesX(void)
{
    return nmbTilesX;
}
int Map::getNmbTilesY(void)
{
    return nmbTilesY;
}

void Map::printMap()
{
    cout << nmbTilesX << endl;
    cout << nmbTilesY << endl;
    for(int i = 0; i < nmbTilesY; i++)
    {
        for(int j = 0; j < nmbTilesX; j++)
        {
            cout << mapTiles[i*nmbTilesX + j].getNameTerrain() << " ";
        }
        cout << endl;
    }
}

Tile* Map::getTile(int _x, int _y)
{
  return &mapTiles[_y*nmbTilesX + _x];
}
Tile* Map::getTile(Rect _position)
{
    return getTile(_position.getX(), _position.getY());
}

Terrain* Map::getTerrainFromTiles(int _x, int _y)
{
    string nameTerrain = mapTiles[_y*nmbTilesX + _x].getNameTerrain();
    return Terrain::getTerrainFromName(nameTerrain);
}

void Map::addNameTerrain(string _nameTerrain)
{
    terrainName.push_back(_nameTerrain);
}

Unit* Map::getUnitFromTiles(int _x, int _y)
{
    return mapTiles[_y*nmbTilesX + _x].getUnit();
}

void Map::moveUnit(Rect _src, Rect _dst)
{
    Tile* tileSrc = getTile(_src.getX(), _src.getY());
    Tile* tileDst = getTile(_dst.getX(), _dst.getY());
    Unit* unit = tileSrc->getUnit();
    unit->setPosition(_dst);
    tileSrc->setUnit(NULL);
    tileDst->setUnit(unit);
}

vector<CaseHighlight> Map::getVectorHighlight()
{
    return vectorHighlight;
}

CaseHighlight Map::getHighlight(Rect _position)
{
    return vectorHighlight[_position.getY()*nmbTilesX + _position.getX()];
}

CaseHighlight Map::getHighlight(int _x, int _y)
{
    return vectorHighlight[_y*nmbTilesX + _x];
}

void Map::updateVectorHighlight(std::vector<Rect> _listPos, CaseHighlight _val)
{
    for(int i = 0; i<_listPos.size(); i++)
    {
        vectorHighlight[_listPos[i].getY()*nmbTilesX + _listPos[i].getX()] = _val;
    }
}

void Map::clearVectorHighlight()
{
    for(int i = 0; i<vectorHighlight.size(); i++)
    {
        vectorHighlight[i] = NONE;
    }
}

void Map::clearVectorHighlight(CaseHighlight highlight)
{
    for(int i = 0; i<vectorHighlight.size(); i++)
    {
        if(vectorHighlight[i] == highlight)
        {
            vectorHighlight[i] = NONE;
        }
    }
}
