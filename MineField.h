#include <cmath>
#include <iostream>

#include "MineCell.h"
#include "MinesweeperUtils.h"

using namespace std;

class MineField {
    private:

    vector <MineCell*> flatMap; // flatMap data

    MinesweeperUtils Utils;

    public:

    vector <MineCell*> bombs; // bombs data
    
    int Size; // Size of the field

    int unrevealedCellsCount; // number of unrevealed cells

    int bombsCount; // Number of bombs

    bool firstTime; // first click or not

    vector <vector <MineCell*>> map; // Map data

    MineField (int FieldSize, int BombsCount);
    // Default constructor<FieldSize, BombsCount>

    void initMap();
    // setup the map

    bool reveal (int x, int y, bool passiveMode);
    // Reveal the cell at (x,y) <Array position>, returns `true` if the cell has bomb, `false` otherwise

    void createEmptyMap (int mapSize);
    // Create a blank Map from given Map Size

    string createHBorder(string start, string end);
    // create horizontal borders with specific starting and ending character sequences

    int safeIndex (int index);
    // return safe index based on field size

    void loadFromData (string data);
    // Load map from data

    void flattenMap();
    // flatten 2D vector to 1D vector assigned in this->flatMap

    bool assignRandomBomb();
    // assign a bomb to random position, return false if failed

    void render ();
    // Render the map to the console

    void getAllBombs();
    // get All Bombs in current map

    void revealAllBombs();
    // reveal all bombs in the map
};

MineField::MineField (int FieldSize, int BombsCount) {
    Size = FieldSize;
    bombsCount = BombsCount < (pow(Size, 2) - 1) ? BombsCount : (pow(Size, 2) - 1);
    createEmptyMap(Size);
    flattenMap();
    for (int i = 0; i < bombsCount; ++i) assignRandomBomb();
    initMap();
};

void MineField::initMap () {
    firstTime = false;
    unrevealedCellsCount = (int)pow(Size, 2);
}

int MineField::safeIndex (int index) {
    return min(max(index, 0), Size - 1);
};

bool MineField::reveal (int x, int y, bool passiveMode) {
    if ((x < 0 || x >= Size) || (y < 0 || y >= Size)) return false;
    MineCell* revealingCell = map[x][y];
    if (revealingCell->revealed) return false;
    int firstReveal = !firstTime;
    if (revealingCell->hasBomb) {
        if (passiveMode) return true;
        if (!firstTime) {
            // first reveal can't be bombed, right?
            firstTime = true;
            revealingCell->hasBomb = false;
            // eject this position from free slot <flatMap> data
            for (int i = 0; i < flatMap.size(); ++i) {
                MineCell* cell = flatMap[i];
                if (cell->x == x && cell->y == y) {
                    flatMap.erase(flatMap.begin() + i);
                    break;
                }
            }
            // assign bombs to another location
            assignRandomBomb();
        }
    }
    if (firstReveal) getAllBombs();
    revealingCell->revealed = true;
    --unrevealedCellsCount;
    if (revealingCell->hasBomb) return true;
    revealingCell->neighborBombsCount = 0;
    // check neigbor bombs
    for (int i = safeIndex(x - 1); i <= safeIndex(x + 1); ++i) {
        for (int j = safeIndex(y - 1); j <= safeIndex(y + 1); ++j) {
            revealingCell->neighborBombsCount += map[i][j]->hasBomb;
        }
    }
    // if it doesn't have any neighbor bombs, continue expanding it
    if (revealingCell->neighborBombsCount == 0) {
        for (int i = safeIndex(x - 1); i <= safeIndex(x + 1); ++i) {
            for (int j = safeIndex(y - 1); j <= safeIndex(y + 1); ++j) {
                reveal(i, j, true);
            }
        }
    }
    return false;
};

void MineField::createEmptyMap (int mapSize) {
    map.clear();
    for (int i = 0; i < mapSize; ++i) {
        map.push_back(vector<MineCell*>());
        for (int j = 0; j < mapSize; ++j) map[i].push_back(new MineCell(i,j,false));   
    }
};


void MineField::loadFromData (string data) {

};

string MineField::createHBorder(string start, string end) {
    string hBorder = "";
    for (int i = 0; i <= (Size-1)*2; ++i) hBorder += "-";
    hBorder = start + hBorder + end + "\n";
    return hBorder;
}

void MineField::render () {
    string hBorder = createHBorder("|", "|");
    cout << createHBorder("┌", "┐");
    for (int i = 0; i < Size; ++i) {
        vector <MineCell*> row = map[i];
        if (i > 0) cout << hBorder;
        for (MineCell* cell : row) {
            cout << "|";
            if (cell->revealed) {
                if (cell->hasBomb) cout << "*";
                else {
                    if (cell->neighborBombsCount == 0) cout << " ";
                    else cout << cell->neighborBombsCount;
                }
            }
            else cout << "?";
        }
        cout << "|" << endl;
    }
    cout << createHBorder("└", "┘") << "Field size: " << Size << " | " << "Bombs: " << bombsCount << endl;
}

void MineField::flattenMap () {
    flatMap.clear();
    for (vector <MineCell*> row : map) {
        for (MineCell* cell: row) {
            flatMap.push_back(cell);
        }
    }
}

bool MineField::assignRandomBomb () {
    if (flatMap.size() == 0) return false;
    int index = Utils.randInt(flatMap.size() - 1);
    MineCell* randomCell = flatMap[index];
    flatMap.erase(flatMap.begin() + index);
    return randomCell->hasBomb = true;
}

void MineField::getAllBombs() {
    bombs.clear();
    for (vector <MineCell*> row : map) {
        for (MineCell* cell: row) {
            if (cell->hasBomb) bombs.push_back(cell);
        }
    }
}

void MineField::revealAllBombs () {
    for (MineCell* cell : bombs) {
        cell->revealed = true;
    }
}