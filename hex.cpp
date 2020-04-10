#ifndef HEX
#define HEX
#include "graph.cpp"

class Hex{
private:
    int size; // dimension of board
    int mode; // game mode: 1. human vs human, 2. human vs machine (initialized at 0)
    int round; // current playing round (initialized at 0)
    int player; // player at current round, takes 1 or 2 (initialized at 0)
    int winner; // winner at final round, takes 1 or 2
    int* marker; // array of players occupying the nodes
    Graph board; // internal representation: graph for nodes (nodes have int labels)
    vector<point> player1; // player 1's moves (marker X, goal N->S)
    vector<point> player2; // player 2's moves (marker O, goal W->E)
    const vector<point> neighborVec{point(0,1),point(-1,1),point(-1,0),point(0,-1),point(1,-1),point(1,0)}; // vectors pointing to (potential) neighbors
public:
    /**** constructors ****/
    Hex();
    Hex(int size);
    /**** helper functions ****/
    int toNode(const point& p);
    point toPoint(int n);
    bool inBoard(const point& p);
    /**** accessors ****/
    void printBoard();
    /**** game ****/
    bool gameFinished();
    vector<point> legalMoves(int Player);
    void inputNextMove(int Player);
    void machineNextMove();
    void nextPlayer();
    void welcome();
    void result(int Winner);
    void chooseMode();
    void chooseFirstPlayer();
    void runGame();
};

/**** helper functions ****/

inline int Hex::toNode(const point& p){return size*p.x+p.y;} // convert board point to graph node
inline point Hex::toPoint(int n){return point(n/size,n%size);} // convert graph node to board point
inline bool Hex::inBoard(const point& p){
    // check if a point is inside board
    if(p.x>=0 && p.x<=size-1 && p.y>=0 && p.y<=size-1) return true;
    return false;
}

/**** constructors ****/

Hex::Hex(){}

Hex::Hex(int size):size(size),mode(0),round(0),player(0),board(size*size){
    // construct graph for board points
    point p;
    marker = new int[board.getSize()];
    for(int i=0; i<size; i++)
        for(int j=0; j<size; j++){
            p = point(i,j); // board point
            marker[toNode(p)] = 0; // unoccupied by any player
            for(auto v:neighborVec)
                if(inBoard(p+v)) board.addEdge(toNode(p),toNode(p+v));
        }
}

/**** accessors ****/

void Hex::printBoard(){
    // print game board
    // marker: player1 = X, player2 = O
    point p;
    seperator(6*size-5);
    cout << "round " << round << " board" << endl;
    for(int i=0; i<size; i++){
        /* print board points */
        // first (size-1) board points
        cout << string(2*i,' ');
        for(int j=0; j<size-1; j++){
            p = point(i,j);
            if(marker[toNode(p)]==1) cout << "X - "; // occupied by player 1
            else if(marker[toNode(p)]==2) cout << "O - "; // occupied by player 2
            else cout << ". - "; // unoccupied
        }
        // last board points
        p = point(i,size-1);
        if(marker[toNode(p)]==1) cout << "X" << endl; // occupied by player 1
        else if(marker[toNode(p)]==2) cout << "O" << endl; // occupied by player 2
        else cout << "." << endl; // unoccupied
        /* print connections between board points (edges) */
        if(i<size-1){
            cout << string(2*i+1,' ');
            for(int j=0; j<size-1; j++) cout << "\\ / ";
            cout << "\\" << endl;
        }
    }
    seperator(6*size-5);
}

/**** game ****/

bool Hex::gameFinished(){
    // check if game has finished (one player has won)
    if(player1.size()==0 || player2.size()==0) return false;
    else if((player1.end()-1)->x==size-1 || (player2.end()-1)->y==size-1) return true;
    return false;
}

vector<point> Hex::legalMoves(int Player){
    // return a vector of legal moves for Player (1 or 2)
    point p;
    vector<point> moves;
    if(Player==1){
        if(player1.size()==0){
            for(int j=0; j<size; j++) moves.push_back(point(0,j));
            return moves;
        }
        p = *(player1.end()-1);
    }
    else if(Player==2){
        if(player2.size()==0){
            for(int i=0; i<size; i++) moves.push_back(point(i,0));
            return moves;
        }
        p = *(player2.end()-1);
    }
    for(auto v:neighborVec)
        // check the 6 potential neighbors of p
        // is a legal move if p+v is not one of players' previous moves and in board
        if(find(player1.begin(),player1.end(),p+v)==player1.end() &&
        find(player2.begin(),player2.end(),p+v)==player2.end() &&
        inBoard(p+v))
            moves.push_back(p+v);
    return moves;
}

void Hex::inputNextMove(int Player){
    // ask Player to input next move
    // setting: (mode 1, either player) or (mode 2, player 1)
    int i,j;
    bool isIllegal = true;
    vector<point> moves = legalMoves(Player);
    round++;
    /* ask for input */
    cout << "next possible moves: " << moves << endl;
    if(moves.size()==0){ // check if there are any legal moves
        cout << "round " << round << " | player " << player << ", unfortunately you have run into a dead end!" << endl;
        return;
    }
    while(isIllegal){
        cout << "round " << round << " | now player " << player << "'s turn: ";
        cin >> i >> j;
        if(find(moves.begin(),moves.end(),point(i,j))!=moves.end()) isIllegal = false; // a legal move
        else cout << "(previous move illegal) "; // an illegal move
    }
    /* log the inputted move */
    point p(i,j);
    if(Player==1){
        marker[toNode(p)] = 1;
        player1.push_back(p);
    }else if(Player==2){
        marker[toNode(p)] = 2;
        player2.push_back(p);
    }
}

void Hex::machineNextMove(){
    // algorithmically generate next move
    // setting: (mode 2, player 2)
    vector<point> moves = legalMoves(player);
    round++;
    /* machine next move */
    cout << "next possible moves: " << moves << endl;
    if(moves.size()==0){ // check if there are any legal moves
        cout << "round " << round << " | player 2, unfortunately you have run into a dead end!" << endl;
        return;
    }
    // ALGORITHM HERE!
    /* 1. dumb random move */
    // point p = moves[rand()%moves.size()];
    /* 2. choose rightmost move */
    priorityQueue q;
    for(int i=0; i<moves.size(); i++) q.add(i,moves[i].y);
    point p = moves[q.popHeadNode().label];
    cout << "(machine move) round " << round << " | now player 2's turn: " << p << endl;
    /* log the machine move */
    marker[toNode(p)] = 2;
    player2.push_back(p);
}

void Hex::nextPlayer(){
    // switch current player
    player = 1+player%2;
}

void Hex::welcome(){
    // instructions to player
    cout <<
    "Welcome to Hex!\n"
    "Instructions:\n"
    "  Player | 1    | 2\n"
    "  Goal   | N->S | W->E\n"
    "  Marker | X    | O\n"
    "  Player who first links to his opposite side in an unbroken chain wins.\n"
    "Board coordinates:\n"
    "  top left     - 0 0\n"
    "  bottom left  - " << size-1 << " 0\n"
    "  top right    - 0 " << size-1 << "\n"
    "  bottom right - " << size-1 << " " << size-1 << "\n"
    "When prompted, enter the coordinates for your next move in format: x y\n"
    "Example: round 1 | now player 1's turn: 0 2" << endl;
}

void Hex::result(int Winner){
    // game result
    cout <<
    "Game result:\n"
    "  Player " << Winner << " wins!\n"
    "  Player " << 1+Winner%2 << " loses!" << endl;
}

void Hex::chooseMode(){
    // choose game setting: human vs human, or human vs machine
    cout <<
    "Game mode:\n"
    "  1. human vs human\n"
    "  2. human vs machine" << endl;
    while(mode!=1 && mode!=2){
        cout << "Please choose a mode (1 or 2): ";
        cin >> mode;
    }
    if(mode==2) cout << "  You are player 1, machine is player 2." << endl;
}

void Hex::chooseFirstPlayer(){
    // choose first player (at round 1)
    while(player!=1 && player!=2){
        cout << "Please choose the first player (1 or 2): ";
        cin >> player;
    }
}

void Hex::runGame(){
    // run the game: human vs human, or human vs machine
    welcome(); // game instructions
    seperator(10);
    chooseMode(); // game mode
    chooseFirstPlayer(); // first player
    /* 1. human vs human */
    if(mode==1)
        while(!gameFinished()){
            printBoard(); // print board before next move
            inputNextMove(player); // (player 1,2 alternating) ask for next move
            nextPlayer(); // next player
        }
    /* 2. human vs machine */
    if(mode==2)
        while(!gameFinished()){
            printBoard(); // print board before next move
            if(player==1) inputNextMove(player); // (player 1 human) ask for next move
            if(player==2) machineNextMove(); // (player 2 machine) algorithmically generate next move
            nextPlayer(); // next player
        }
    seperator(6*size-5);
    winner = 1+player%2;
    result(winner); // game result
}

#endif
