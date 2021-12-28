#include <cmath>
#include <iostream>
#include <ctime>
#include <string>

#include "MineCell.h"
#include "MinesweeperUtils.h"

using namespace std;

class MineField {
    private:

    vector <MineCell*> flatMap; // flatMap data

    MinesweeperUtils Utils;

    vector <MineCell*> bombs; // bombs data

    public:

    bool valid; // if the field is valid or not
    
    int Size; // Size of the field

    int unrevealedCellsCount; // number of unrevealed cells

    int bombsCount; // Number of bombs

    int flagsCount; // number of flags

    bool firstTime; // first click or not

    long savedTimestamp; // saved timestamp

    long timesPlayed; // times played

    int openedTimestamp; // timestamp when you reopen the record

    vector <vector <MineCell*>> map; // Map data

    MineField (int FieldSize, int BombsCount);
    // constructor for creating new MineField

    MineField (long Timestamp, long TimesPlayed, int FieldSize, string data);
    // constructor for creating new MineField based on data

    void initMap();
    // setup the map

    bool reveal (int x, int y, bool passiveMode, bool flagged);
    // Reveal the cell at (x,y) <Array position>, returns `true` if the cell has bomb, `false` otherwise

    void createEmptyMap (int mapSize);
    // Create a blank Map from given Map Size

    string createHBorder(string start, string end);
    // create horizontal borders with specific starting and ending character sequences

    int safeIndex (int index);
    // return safe index based on field size

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

    string exportData();
    // save export the map to file-readable string

    void save();
    // save the timestamp

    void flag(int x, int y);
    // flag the specified cell

    void getAllFlags();
    // get all current flags in the field

    void getNeighborBombs (int x, int y);
    // get neighbor bombs at <x,y>
};

MineField::MineField (int FieldSize, int BombsCount) {
    savedTimestamp = time(0);
    timesPlayed = 0;
    Size = FieldSize;
    bombsCount = BombsCount < (pow(Size, 2) - 1) ? BombsCount : (pow(Size, 2) - 1);
    createEmptyMap(Size);
    flattenMap();
    for (int i = 0; i < bombsCount; ++i) assignRandomBomb();
    valid = true;
    firstTime = true;
    initMap();
};

MineField::MineField (long Timestamp, long TimesPlayed, int FieldSize, string data) {
    savedTimestamp = Timestamp;
    timesPlayed = max(TimesPlayed, 0l);
    Size = max(FieldSize, 3);
    createEmptyMap(Size);
    valid = true;
    firstTime = true;
    for (vector <MineCell*> row : map) {
        for (MineCell* cell : row) {
            if (data.length() == 0) goto nextProcess;
            int ut = Utils.stoi(data.substr(0,1)); 
            data = data.substr(1, data.length());
            cell->hasBomb = (bool)(ut % 2);
            ut /= 2;
            cell->revealed = (bool)(ut % 2);
            ut /= 2;
            cell->flagged = (bool)(ut % 2);
            if (cell->revealed) firstTime = false;
            if (cell->hasBomb && cell->revealed) {
                valid = false;
                return;
            }
        }
    }
    nextProcess: flattenMap();
    getAllBombs();
    bombsCount = bombs.size();
    if (bombsCount == 0) {
        valid = false;
        return;
    }
    for (vector <MineCell*> row : map) {
        for (MineCell* cell : row) {
            if (cell->revealed && !cell->hasBomb) getNeighborBombs(cell->x, cell->y);
        }
    }
    initMap();
}

void MineField::save() {
    long currentTime = time(0);
    timesPlayed += max(currentTime - openedTimestamp, 0l);
    openedTimestamp = currentTime;
    savedTimestamp = currentTime;
}

string MineField::exportData() {
    string exp = to_string(savedTimestamp) + "\n" + to_string(timesPlayed) + "\n" + to_string(Size) + "\n";
    for (vector<MineCell*> row : map) {
        for (MineCell* cell : row) exp += to_string(cell->flagged * 4 + cell->revealed * 2 + cell->hasBomb);
    }
    return exp;
}

void MineField::initMap () {
    openedTimestamp = time(0);
    unrevealedCellsCount = 0;
    getAllFlags();
    for (vector <MineCell*> row : map) {
        for (MineCell* cell : row) {
            unrevealedCellsCount += !cell->revealed;
        }
    }
}

int MineField::safeIndex (int index) {
    return min(max(index, 0), Size - 1);
};

bool MineField::reveal (int x, int y, bool passiveMode, bool flagged) {
    if ((x < 0 || x >= Size) || (y < 0 || y >= Size)) return false;
    if (flagged) {
        flag(x, y);
        return false;
    }
    MineCell* revealingCell = map[x][y];
    if (revealingCell->revealed || (!passiveMode && revealingCell->flagged)) return false;
    int firstReveal = firstTime;
    if (revealingCell->hasBomb) {
        if (passiveMode) return true;
        if (firstTime) {
            // first reveal can't be bombed, right?
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
    firstTime = false;
    if (firstReveal) {
        getAllBombs();
        save();
        timesPlayed = 0;
    }
    revealingCell->revealed = true;
    revealingCell->flagged = false;
    getAllFlags();
    --unrevealedCellsCount;
    if (revealingCell->hasBomb) return true;
    // check neigbor bombs
    getNeighborBombs(x, y);
    // if it doesn't have any neighbor bombs, continue expanding it
    if (revealingCell->neighborBombsCount == 0) {
        for (int i = safeIndex(x - 1); i <= safeIndex(x + 1); ++i) {
            for (int j = safeIndex(y - 1); j <= safeIndex(y + 1); ++j) {
                reveal(i, j, true, false);
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
                if (cell->hasBomb) {
                    if (cell->flagged )cout << "X";
                    else cout << "*";
                }
                else {
                    if (cell->neighborBombsCount == 0) cout << " ";
                    else cout << cell->neighborBombsCount;
                }
            }
            else {
                if (cell->flagged) cout << "F";
                else cout << "-";
            }
        }
        cout << "|" << endl;
    }
    cout << createHBorder("└", "┘") << "Field size: " << Size << " | " << "Bombs: " << bombsCount << " | " << "Flags: " << flagsCount << endl;
}

void MineField::flattenMap () {
    flatMap.clear();
    for (vector <MineCell*> row : map) {
        for (MineCell* cell: row) {
            if (!cell->hasBomb) flatMap.push_back(cell);
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

void MineField::flag (int x, int y) {
    if ((x < 0 || x >= Size) || (y < 0 || y >= Size)) return;
    MineCell* revealingCell = map[x][y];
    if (revealingCell->revealed) return;
    revealingCell->flagged = !revealingCell->flagged;
    getAllFlags();
}

void MineField::getAllFlags () {
    flagsCount = bombsCount;
    for (vector <MineCell*> row : map) {
        for (MineCell* cell: row) flagsCount -= cell->flagged;
    }
}

void MineField::getNeighborBombs (int x, int y) {
    MineCell* cell = map[x][y];
    cell->neighborBombsCount = 0;
    if ((x < 0 || x >= Size) || (y < 0 || y >= Size)) return;
    for (int i = safeIndex(x - 1); i <= safeIndex(x + 1); ++i) {
        for (int j = safeIndex(y - 1); j <= safeIndex(y + 1); ++j) {
            cell->neighborBombsCount += map[i][j]->hasBomb;
        }
    }
}