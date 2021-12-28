

class MineCell {
    public:

    MineCell (int xPos, int yPos, bool cellHasBomb) {
        x=xPos;
        y=yPos;
        hasBomb=(bool)cellHasBomb;
        revealed = false;
        neighborBombsCount = 0;
        flagged = false;
    };

    int x, y, neighborBombsCount;

    bool hasBomb, revealed, flagged;
};