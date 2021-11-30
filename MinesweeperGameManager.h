#include <vector>
#include <iostream>

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

    void GameOver (bool isBombed);
    // Gameover the player (bombed or not)

    void render ();
    // render the minefield to the console

    void startProcess ();
    // start the user input reccursive process
};

void MinesweeperGameManager::load (MineField* data) {
    records.push_back(data);
    currentData = data;
};

void MinesweeperGameManager::create () {
    Utils.clearConsole();
    int mapSize, bombs;
    cout << "Please Enter field size (minimum 2) and number of bombs (minimum 1), -1 to back to menu: ";
    cin >> mapSize;
    if (mapSize < 0) {
        start();
        return;
    }
    mapSize = max(mapSize, 2);
    cin >> bombs;
    if (bombs < 0) {
        start();
        return;
    }
    bombs = max(1, min(bombs, (int)pow(mapSize, 2) - 1));
    load(new MineField(mapSize, bombs));
    startProcess();
};

void MinesweeperGameManager::fetchRecords () {

};

void MinesweeperGameManager::start () {
    Utils.clearConsole();
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
            cout << "Load game";
            // load(data);
            break;
        case 2:
            GameOver(false);
    }
};

void MinesweeperGameManager::GameOver (bool isBombed) {
    render();
    if (!isBombed) {
        cout << "Quitting game" << endl;
        cout << "Thanks for playing!";
    }
    else {
        cout << "Oops! You digged deeper and caught a bomb! Too bad";
        Utils.pauseConsole(true);
    }
}

void MinesweeperGameManager::render () {
    Utils.clearConsole();
    currentData->render();
}

void MinesweeperGameManager::startProcess () {
    render();
    cout << "Input row and column position of a block to open it (from 0 to " << currentData->Size - 1 << "), -1 to exit: ";
    int row, column;
    Utils.readInt(row);
    if (row < 0) {
        GameOver(false);
        return;
    }
    Utils.readInt(column);
    if (column < 0) {
        GameOver(false);
        return;
    }
    bool hasBomb = currentData->reveal(row, column, false);
    if (hasBomb) {
        currentData->revealAllBombs();
        GameOver(true);
    }
    else {
        if (currentData->unrevealedCellsCount == currentData->bombs.size()) {
            render();
            cout << "You win!";
        }
        else startProcess();
    }
}