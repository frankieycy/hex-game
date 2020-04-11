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
    Graph player1Graph; // graph representation of player 1's moves
    Graph player2Graph; // graph representation of player 2's moves
    vector<point> player1; // player 1's moves (marker X, goal N-S)
    vector<point> player2; // player 2's moves (marker O, goal E-W)
    vector<point> boardPoints; // all points in board
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
    void welcome();
    void result();
    void chooseMode();
    void chooseFirstPlayer();
    void nextPlayer();
    vector<point> legalMoves();
    bool gameFinished();
    void inputNextMove();
    point machineMove(vector<point> moves);
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

Hex::Hex(int size):player1Graph(size*size),player2Graph(size*size){
    // construct Hex class
    this->size = size;
    this->mode = 0;
    this->round = 0;
    this->player = 0;
    point p;
    marker = new int[size*size];
    // fill in board points
    for(int i=0; i<size; i++)
        for(int j=0; j<size; j++)
            boardPoints.push_back(point(i,j));
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

void Hex::welcome(){
    // instructions to player
    cout <<
    "Welcome to Hex!\n"
    "Instructions:\n"
    "  | Player |  1  |  2  |\n"
    "  | ------ | --- | --- |\n"
    "  | Goal   | N-S | E-W |\n"
    "  | Marker |  X  |  O  |\n"
    "  Players alternate placing markers on unoccupied spaces, and\n"
    "  who first links to his opposite side in an unbroken chain wins.\n"
    "Board coordinates:\n"
    "  top left     - 0 0\n"
    "  bottom left  - " << size-1 << " 0\n"
    "  top right    - 0 " << size-1 << "\n"
    "  bottom right - " << size-1 << " " << size-1 << "\n"
    "When prompted, enter the coordinates for your next move in format: x y\n"
    "Example: round 1 | now player 1's turn: 0 2" << endl;
}

void Hex::result(){
    // game result
    cout <<
    "Game result:\n"
    "  Player " << winner << " wins!\n"
    "  Player " << 1+winner%2 << " loses!" << endl;
}

void Hex::chooseMode(){
    // choose game mode: 1. human vs human, or 2. human vs machine
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
    // choose first player (who makes a move at round 1)
    while(player!=1 && player!=2){
        cout << "Please choose the first player (1 or 2): ";
        cin >> player;
    }
}

void Hex::nextPlayer(){
    // switch current player
    player = 1+player%2;
}

vector<point> Hex::legalMoves(){
    // return a vector of legal moves for current player (any unoccupied spaces on board)
    vector<point> moves;
    for(auto p:boardPoints)
        // is a legal move if p is not any of the players' previous moves, i.e., unoccupied
        if(find(player1.begin(),player1.end(),p)==player1.end() &&
        find(player2.begin(),player2.end(),p)==player2.end())
            moves.push_back(p);
    return moves;
}

bool Hex::gameFinished(){
    // check if game has finished, i.e., one player has won
    vector<point> start,end;
    vector<node>* paths;

    /* has player 1 won? */
    for(auto p:player1){
        if(p.x==0) start.push_back(p); // board points on top edge (North)
        if(p.x==size-1) end.push_back(p); // board points on bottom edge (South)
    }
    for(auto p:start){
        paths = player1Graph.shortestPaths(toNode(p));
        for(auto q:end) // check if there is a path linking to opposite edge
            if(paths[toNode(q)].size()!=0){ // player 1 wins, game ends
                winner = 1;
                return true;
            }
    }

    /* has player 2 won? */
    start.clear(); end.clear();
    for(auto p:player2){
        if(p.y==0) start.push_back(p); // board points on left edge (West)
        if(p.y==size-1) end.push_back(p); // board points on right edge (East)
    }
    for(auto p:start){
        paths = player2Graph.shortestPaths(toNode(p));
        for(auto q:end) // check if there is a path linking to opposite edge
            if(paths[toNode(q)].size()!=0){ // player 2 wins, game ends
                winner = 2;
                return true;
            }
    }
    return false;
}

void Hex::inputNextMove(){
    // call human or machine for next move
    point p;
    vector<point> moves = legalMoves(); // legal moves at current round
    round++;

    /* ask for human input */
    if(mode==1 || (mode==2 && player==1)){
        int i,j;
        bool isIllegal = true;
        while(isIllegal){
            cout << "round " << round << " | now player " << player << "'s turn: ";
            cin >> i >> j;
            p = point(i,j);
            if(find(moves.begin(),moves.end(),p)!=moves.end()) isIllegal = false; // a legal move
            else cout << "(previous move illegal) "; // an illegal move
        }
    }else
    /* call for machine input */
    if(mode==2 && player==2){
        p = machineMove(moves);
        cout << "(machine move) round " << round << " | now player 2's turn: " << p << endl;
    }

    /* log the inputted move */
    if(player==1){
        marker[toNode(p)] = 1;
        player1.push_back(p);
        for(auto v:neighborVec) // connect p to board points around p in player's graph
            if(find(player1.begin(),player1.end(),p+v)!=player1.end()){
                player1Graph.addEdge(toNode(p),toNode(p+v));
                player1Graph.setCost(toNode(p),toNode(p+v),1);
            }
    }else if(player==2){
        marker[toNode(p)] = 2;
        player2.push_back(p);
        for(auto v:neighborVec) // connect p to board points around p in player's graph
            if(find(player2.begin(),player2.end(),p+v)!=player2.end()){
                player2Graph.addEdge(toNode(p),toNode(p+v));
                player2Graph.setCost(toNode(p),toNode(p+v),1);
            }
    }
}

point Hex::machineMove(vector<point> moves){
    // ALGORITHMS HERE!
    // algorithmically generate next move (mode 2, player 2)
    point p;

    /* 1. dumb random move */
    // p = moves[rand()%moves.size()];

    /* 2. choose rightmost move */
    if(round==1) p = point(size/2,0); // first round: begins at left edge
    else{
        priorityQueue q;
        auto ptr=player2.end()-1; // "rightmost" end of path
        while(q.isEmpty()){
            for(auto v:neighborVec)
                if(find(moves.begin(),moves.end(),*ptr+v)!=moves.end())
                    // possible moves around the "rightmost" end
                    q.add(toNode(*ptr+v),(*ptr+v).y);
            ptr--;
        }
        p = toPoint(q.popHeadNode().label);
    }
    return p;
}

void Hex::runGame(){
    // run the game: human vs human, or human vs machine
    welcome(); // game instructions
    seperator(10);
    chooseMode(); // game mode
    chooseFirstPlayer(); // first player
    while(!gameFinished()){
        printBoard(); // print board before next move
        inputNextMove(); // (player 1,2 alternating) call for next move
        nextPlayer(); // next player
    }
    seperator(6*size-5);
    result(); // game result
}

#endif
