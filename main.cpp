/*
Hex game and AI - 12/4/2020
*/
#include "hex.cpp"

int main(){
    srand(clock());
    int size;
    cout << "Choose a board (e.g. enter 7 for a 7*7 game board): ";
    cin >> size;
    seperator(50);
    Hex hex(size); // instantiate Hex game
    hex.runGame(); // run the game (play on terminal)
}
