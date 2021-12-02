#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "MineField.h"

using namespace std;


class MinesweeperGameManager {
    public:

    vector <MineField*> records;

    MineField* currentData;

    vector <string> startMenuOptions {"Start a new game", "Load an existing game", "Quit"};

    MinesweeperUtils Utils;

    void load (MineField* data);
    // load game from given data

    void create();
    // Create new game

    void fetchRecords();
    // get Records from file

    void start();
    // start the game

    void gameOver ();
    // Gameover the player

    void render ();
    // render the minefield to the console

    void startProcess ();
    // start the user input reccursive process

    void save ();
    // save the current record

    void chooseRecord ();
    // userInput choose record

    void exportRecords();
    // export records to file

    void removeRecord (MineField* data);
    // clear the specified record from the records list

    void quit(bool needSave);
    // quit the game

    void win ();
    // user wins the game

    void endGameSelection (string text);
    // display endgame with text
};

void MinesweeperGameManager::load (MineField* data) {
    records.push_back(data);
    currentData = data;
    startProcess();
};

void MinesweeperGameManager::create () {
    Utils.clearConsole();
    int mapSize, bombs;
    cout << "Please Enter field size (minimum 2, -1 to back to menu): ";
    Utils.readInt(mapSize);
    if (mapSize < 0) {
        start();
        return;
    }
    mapSize = max(mapSize, 2);
    cout << "Please Enter field size (minimum 1, maximum (size^2 - 1), -1 to back to menu): ";
    Utils.readInt(bombs);
    if (bombs < 0) {
        start();
        return;
    }
    bombs = max(1, min(bombs, (int)pow(mapSize, 2) - 1));
    load(new MineField(mapSize, bombs));
};

void MinesweeperGameManager::fetchRecords () {
    records.clear();
    ifstream recordsFile ("MinesweeperRecords.txt");
    string content( (istreambuf_iterator<char>(recordsFile) ), (istreambuf_iterator<char>()    ) );
    vector <string> rawRecords = Utils.splitString(content, "\n\n");
    for (string rawRecord : rawRecords) {
        vector <string> recordInfo = Utils.splitString(rawRecord, "\n");
        while (recordInfo.size() < 4) recordInfo.push_back("0");
        for (string &info : recordInfo) {
            if (info == "") info = "0";
        }
        MineField* MF = new MineField((long)Utils.stoi(recordInfo[0]), (long)Utils.stoi(recordInfo[1]), Utils.stoi(recordInfo[2]), recordInfo[3]);
        if (MF->valid) records.push_back(MF);
    }
    recordsFile.close();
    exportRecords();
};

void MinesweeperGameManager::exportRecords () {
    ofstream recordsFile ("MinesweeperRecords.txt");
    for (MineField* MF : records) {
        recordsFile << MF->exportData() << endl << endl;
    }
    recordsFile.close();
}

void MinesweeperGameManager::chooseRecord () {
    Utils.clearConsole();
    int i = 0, selection;
    for (MineField* fieldData : records) {
        cout << i++ << ". " << fieldData->savedTimestamp << " | Field size: " << fieldData->Size << ", Bombs: " << fieldData->bombsCount << endl; 
    }
    if (records.size() == 0) cout << "NO RECORDS SAVED" << endl;
    cout << endl << "Choose a game from your saved records, -1 to go back: ";
    Utils.readInt(selection);
    if (selection < 0) start();
    else if (selection >= 0 && selection < records.size()) load(records[selection]);
    else chooseRecord();
};

void MinesweeperGameManager::removeRecord (MineField* data) {
    auto it = find(records.begin(), records.end(), data);
    if (it != records.end()) records.erase(it);
}

void MinesweeperGameManager::save () {
    currentData->save();
};

void MinesweeperGameManager::start () {
    Utils.clearConsole();
    fetchRecords();
    cout << "Welcome to Minesweeper!" << endl;
    cout << "Please choose a number below to start:" << endl;
    int menuSize = startMenuOptions.size();
    for (int i = 0; i < menuSize; ++i) cout << i << ". " << startMenuOptions[i] << endl;
    int option;
    cout << "Your option: ";
    Utils.readInt(option);
    if (option < 0 || option >= menuSize) start();
    else switch (option) {
        case 0:
            create();
            break;
        case 1:
            chooseRecord();
            break;
        case 2:
            quit(false);
    }
};

void MinesweeperGameManager::endGameSelection (string str) {
    currentData->save();
    render();
    removeRecord(currentData);
    cout << str << endl;
    cout << "Times played: " << Utils.convertTime(currentData->timesPlayed) << endl;
    cout << "Type anything to back to menu, 0 to quit: ";
    int selection;
    Utils.readInt(selection);
    if (selection == 0) quit(false);
    else start();
}

void MinesweeperGameManager::gameOver () {
    currentData->revealAllBombs();
    endGameSelection("Oops! You digged deeper and caught a bomb! Too bad!");
}

void MinesweeperGameManager::win () {
    endGameSelection("You win!");
}

void MinesweeperGameManager::quit (bool needSave) {
    if (needSave) {
        cout << "Save this game? (-1: Cancel, 0: No, 1: Yes): ";
        int prom;
        Utils.readInt(prom);
        if (prom < 0) {
            startProcess();
            return;
        }
        if (prom == 0) removeRecord(currentData);
        else save();
        exportRecords();
    }
    cout << "Quitting game" << endl;
    cout << "Thanks for playing!";
}

void MinesweeperGameManager::render () {
    Utils.clearConsole();
    currentData->render();
}

void MinesweeperGameManager::startProcess () {
    render();
    cout << "Input row, column position of a block (from 0 to " << currentData->Size - 1 << ") and a flagged number (0 to open, 1 to flag), -1 to exit: ";
    int row, column, flagged;
    Utils.readInt(row);
    if (row < 0) {
        quit(true);
        return;
    }
    Utils.readInt(column);
    if (column < 0) {
        quit(true);
        return;
    }
    Utils.readInt(flagged);
    if (flagged < 0) {
        quit(true);
        return;
    }
    bool hasBomb = currentData->reveal(row, column, false, flagged);
    if (hasBomb) gameOver();
    else {
        if (currentData->unrevealedCellsCount == currentData->bombsCount) win();
        else startProcess();
    }
}