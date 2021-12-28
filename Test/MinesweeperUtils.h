#include <iostream>
#include <future>
#include <random>
#include <ctime>
#include <sstream>

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

    vector <string> splitString(string str, string splitter);
    // split the string with given splitter

    int stoi(string str);
    // convert string to int

    string toDateString(long epoch);
    // convert unix timestamp to human readable time

    string convertTime(long seconds);
    // convert seconds to AhBmCs
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
};

vector <string> MinesweeperUtils::splitString (string str, string splitter) {
    vector <string> result;
    size_t found;
    while ((found = str.find(splitter)) != string::npos) {
        result.push_back(str.substr(0, found));
        str = str.substr((int)found + splitter.length(), str.length());
    }
    if (str.length() != 0) result.push_back(str);
    return result;
}

int MinesweeperUtils::stoi(string str) {
    return atoi( str.c_str() );
}

//string MinesweeperUtils::toDateString (long epoch) {
//    return asctime(gmtime((time_t*)epoch));
//}

string MinesweeperUtils::convertTime (long seconds) {
    long s = seconds % 60;
    seconds /= 60;
    long m = seconds % 60;
    long h = seconds / 60;
    return to_string(h) + "h" + to_string(m) + "m" + to_string(s) + "s";
}