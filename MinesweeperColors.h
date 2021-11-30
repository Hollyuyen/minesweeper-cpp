// Outputing text with different colors in console

#include <map>

using namespace std;

class MinesweeperColors {
    private:

    map<string, int> colorMap;

    public:

    MinesweeperColors ();

    void addColor(string representative, int value);
    // add new color to the map

    int getColor(string representative);
    // get color value from the representative string

    void outputText(string representative);
    // ouput the representative text with its color
};

MinesweeperColors::MinesweeperColors () {
    addColor("|", 12);
    addColor("0", 13);
}

void MinesweeperColors::addColor (string representative, int value) {
    colorMap.insert(pair<string ,int>(representative, value));
}

int MinesweeperColors::getColor(string representative) {
    return colorMap.at(representative);
}

void MinesweeperColors::outputText(string representative) {
    int color = getColor(representative);
}