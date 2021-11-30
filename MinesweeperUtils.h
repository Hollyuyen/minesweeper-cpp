#include <iostream>
#include <future>
#include <random>

#include "MinesweeperColors.h"

using namespace std;

class MinesweeperUtils {
    public:

    MinesweeperColors Colors;

    void clearConsole (); // Clear the whole console screen

    template <class F, class... Args>
    atomic_bool setInterval(atomic_bool& cancelToken, F&& f, size_t interval, Args&&... args);
    // setInterval with a given controller variable;
    // Syntax: setInterval(atomic_bool controller, func, delay, ...args?)

    void clearInterval(atomic_bool& controller);
    // clear the specified Interval

    int randInt (int range);
    // random an integer from range 0 -> range

    int randIntInRange (int start, int end);
    // random an integer in range (start, end)

    void readInt(int& num);
    // Safe way to read an int to the console and assign it to a var, returns -1 if failed; 

    void pauseConsole(bool includeMessage);
    // pause console - with or without prompting message
};

void MinesweeperUtils::clearConsole () {        
    if (system("CLS")) system("clear");
};

template <class F, class... Args>
atomic_bool MinesweeperUtils::setInterval(atomic_bool& cancelToken, F&& f, size_t interval, Args&&... args){
    cancelToken.store(true);
    auto cb = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
    std::async(std::launch::async,[=,&cancelToken]()mutable{
        while (cancelToken.load()){
            cb();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    });
};

void MinesweeperUtils::clearInterval(atomic_bool& controller) {
    controller.store(false);
}

int MinesweeperUtils::randIntInRange (int start, int end) {
    // https://stackoverflow.com/questions/43432014/c-generate-random-number-every-time/43432296#43432296
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(start, end);
    return dis(gen);
}

int MinesweeperUtils::randInt (int range) {
    return randIntInRange(0, range);
}

void MinesweeperUtils::readInt(int& num) {
    // https://stackoverflow.com/questions/5864540/infinite-loop-with-cin-when-typing-string-while-a-number-is-expected/5864560#5864560
    cin >> num;
    if (cin.fail())
    {
        // get rid of failure state
        cin.clear(); 

        // discard 'bad' character(s) 
        cin.ignore(numeric_limits <streamsize>::max(), '\n');

        // assign -1 to the variable
        num = -1;
    }
}

void MinesweeperUtils::pauseConsole (bool includeMessage) {
    if (includeMessage) cout << endl << "Press any key to continue...";
    int prom = getchar();
}