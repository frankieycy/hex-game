/*
Hex game - 11/4/2020

instructions:
- display the game board
- allow an input and determine if the inputted move is legal
- determine who wins
- implement simple strategy for human-machine play

note:
- to grade, pls look into my hex class in hex.cpp (other files are simply auxiliary)
- my (naive) machine stratrgy implemented include: 1. random move, 2. rightmost move (see point Hex::machineMove(vector<point> moves))
- the whole folder contains 4 files: util.cpp, graph.cpp, hex.cpp, main.cpp
- hex.cpp contains the hex class (core part), main.cpp runs the game
- util.cpp, graph.cpp contain elements from previous homework (which are used in hex.cpp)
*/
#include "hex.cpp"

int main(){
    srand(clock());
    Hex hex(7); // instantiate Hex game
    hex.runGame(); // run the game (play on terminal)
}
