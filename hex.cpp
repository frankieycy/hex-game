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
    /**** destructors ****/
    ~Hex();
    /**** helper functions ****/
    int toNode(const point& p);
    point toPoint(int n);
    bool inBoard(const point& p);
    /**** accessors ****/
    void printBoard(int* Marker);
    /**** game ****/
    void welcome();
    void result();
    void chooseMode();
    void chooseFirstPlayer();
    void nextPlayer();
    vector<point> legalMoves();
    bool hasWon(const vector<point>& start, const vector<point>& end, Graph& playerGraph);
    bool gameFinished();
    void inputNextMove();
    void logInputtedMove(vector<point>& Player, Graph& playerGraph, const point& p);
    point machineMove(const vector<point>& moves);
    // MACHINE ALGORITHMS HERE!
    point machineRandomMove(const vector<point>& moves);
    point machineRightmostMove(const vector<point>& moves);
    point machineMCMove(const vector<point>& moves);
    point machineAlphaBetaMove(const vector<point>& moves); // TO DO
    int getHeuristic();
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
    // this->player1Graph = Graph(size*size);
    // this->player2Graph = Graph(size*size);
    // use adjacency list for sparse graphs
    // player1Graph.makeAdjList();
    // player2Graph.makeAdjList();
    point p;
    marker = new int[size*size];
    // fill in board points
    for(int i=0; i<size; i++)
        for(int j=0; j<size; j++)
            boardPoints.push_back(point(i,j));
}

/**** destructors ****/

Hex::~Hex(){
    delete[] marker;
}

/**** accessors ****/

void Hex::printBoard(int* Marker){
    // print game board
    // marker: player1 = X, player2 = O
    point p;
    seperator(50);
    cout << "round " << round << " board" << endl;
    for(int i=0; i<size; i++){
        /* print board points */
        // first (size-1) board points
        cout << string(2*i,' ');
        for(int j=0; j<size-1; j++){
            p = point(i,j);
            if(Marker[toNode(p)]==1) cout << "X - "; // occupied by player 1
            else if(Marker[toNode(p)]==2) cout << "O - "; // occupied by player 2
            else cout << ". - "; // unoccupied
        }
        // last board points
        p = point(i,size-1);
        if(Marker[toNode(p)]==1) cout << "X" << endl; // occupied by player 1
        else if(Marker[toNode(p)]==2) cout << "O" << endl; // occupied by player 2
        else cout << "." << endl; // unoccupied
        /* print connections between board points (edges) */
        if(i<size-1){
            cout << string(2*i+1,' ');
            for(int j=0; j<size-1; j++) cout << "\\ / ";
            cout << "\\" << endl;
        }
    }
    seperator(50);
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
    // ADD: MODE 3 - MACHINE VS MACHINE?
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

bool Hex::hasWon(const vector<point>& start, const vector<point>& end, Graph& playerGraph){
    // check if there is a path linking to opposite edge
    // i.e., any paths that go from start points to end points
    // CORRECTION: inefficient! could make borders on one side and the opposite, then do one search
    int* pathNodes;
    if(start.size()==0 || end.size()==0) return false;
    for(auto p:start){
        pathNodes = playerGraph.shortestPathNodes(toNode(p));
        for(auto q:end)
            if(pathNodes[toNode(q)]!=-1)
                return true;
    }
    return false;
}

bool Hex::gameFinished(){
    // check if game has finished, i.e., one player has won
    vector<point> start,end;

    /* has player 1 won? */
    for(auto p:player1){
        if(p.x==0) start.push_back(p); // board points on top edge (North)
        if(p.x==size-1) end.push_back(p); // board points on bottom edge (South)
    }
    if(hasWon(start,end,player1Graph)){
        winner = 1;
        return true;
    }

    /* has player 2 won? */
    start.clear(); end.clear();
    for(auto p:player2){
        if(p.y==0) start.push_back(p); // board points on left edge (West)
        if(p.y==size-1) end.push_back(p); // board points on right edge (East)
    }
    if(hasWon(start,end,player2Graph)){
        winner = 2;
        return true;
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
        string inputi, inputj;
        bool isIllegal = true;
        while(isIllegal){
            cout << "round " << round << " | now player " << player << "'s turn: ";
            cin >> inputi >> inputj;
            if(!isInt(inputi) || !isInt(inputj)){ // ensure integer input
                cout << "(non-integer input) ";
                continue;
            }
            i = stoi(inputi);
            j = stoi(inputj);
            p = point(i,j);
            if(find(moves.begin(),moves.end(),p)!=moves.end()) isIllegal = false; // a legal move
            else cout << "(previous move illegal) "; // an illegal move
        }
    }else
    /* call for machine input */
    if(mode==2 && player==2){
        auto startTime = high_resolution_clock::now();
        p = machineMove(moves);
        auto endTime = high_resolution_clock::now();
        cout << "(machine move) round " << round << " | now player 2's turn: " << p << endl;
        cout << "               time taken: " << duration_cast<seconds>(endTime-startTime).count()  << " s" << endl;
    }

    /* log inputted move */
    if(player==1){
        marker[toNode(p)] = 1;
        logInputtedMove(player1,player1Graph,p);
    }else if(player==2){
        marker[toNode(p)] = 2;
        logInputtedMove(player2,player2Graph,p);
    }
}

void Hex::logInputtedMove(vector<point>& Player, Graph& playerGraph, const point& p){
    // log inputted move
    Player.push_back(p);
    for(auto v:neighborVec) // connect p to board points around p in player's graph
        if(find(Player.begin(),Player.end(),p+v)!=Player.end()){
            playerGraph.addEdge(toNode(p),toNode(p+v));
            playerGraph.setCost(toNode(p),toNode(p+v),1);
        }
}

point Hex::machineMove(const vector<point>& moves){
    // AI ALGORITHMS HERE!
    // algorithmically generate next move (mode 2, player 2)

    // return machineRandomMove();
    // return machineRightmostMove(moves);
    return machineMCMove(moves);
}

point Hex::machineRandomMove(const vector<point>& moves){
    // dumb random move
    return moves[rand()%moves.size()];
}

point Hex::machineRightmostMove(const vector<point>& moves){
    // choose rightmost move
    if(round==1) return point(size/2,0); // first round: begins at left edge
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
        return toPoint(q.popHeadNode().label);
    }
}

point Hex::machineMCMove(const vector<point>& moves){
    // choose next move that has highest prob to win
    int wins;
    const int trials=1000; // num of trials for each possible move
    double winProb;
    priorityQueue q; // moves that have higher win prob have higher prioirty

    for(int i=0; i<moves.size(); i++){
        // show progress bar
        cout << "  running: " << progressBars[i%sizeof(progressBars)] << ' ' << static_cast<int>(100.*i/moves.size()) << "%\r";
        cout.flush();

        wins = 0;
        vector<point> shuffledMoves = moves;
        shuffledMoves.erase(shuffledMoves.begin()+i);
        for(int trial=0; trial<trials; trial++){
            vector<point> start,end;
            vector<point> player2Sim = player2; // player 2's simulated moves
            Graph player2GraphSim = player2Graph; // graph representation of player 2's simulated moves
            logInputtedMove(player2Sim,player2GraphSim,moves[i]); // move[i] chosen as next move
            random_shuffle(shuffledMoves.begin(),shuffledMoves.end()); // first half as player 2's moves
            for(int j=0; j<shuffledMoves.size()/2; j++)
                logInputtedMove(player2Sim,player2GraphSim,shuffledMoves[j]);
            for(auto p:player2Sim){
                if(p.y==0) start.push_back(p); // board points on left edge (West)
                if(p.y==size-1) end.push_back(p); // board points on right edge (East)
            }
            if(hasWon(start,end,player2GraphSim)) wins++; // player 2 wins, player 1 loses (implied)
        }
        winProb = static_cast<double>(wins)/trials;
        q.add(i,winProb);
    }
    // q.print();
    return moves[q.popHeadNode().label];
}

// point machineAlphaBetaMove(const vector<point>& moves){}

// int Hex::getHeuristic(){
//     // machine as max agent
// }

void Hex::runGame(){
    // run the game: human vs human, or human vs machine
    welcome(); // game instructions
    seperator(50);
    chooseMode(); // game mode
    chooseFirstPlayer(); // first player
    while(!gameFinished()){
        printBoard(marker); // print board before next move
        inputNextMove(); // (player 1,2 alternating) call for next move
        nextPlayer(); // next player
    }
    seperator(50);
    result(); // game result
}

#endif
