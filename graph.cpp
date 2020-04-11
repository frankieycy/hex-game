#ifndef GRAPH
#define GRAPH
#include "util.cpp"
using namespace std;

struct edge{
    // a tuple of int to represent graph edge
    int node1;
    int node2;
    edge(int n, int m):node1(n),node2(m){}
};

inline ostream& operator<<(ostream& out, const edge& e){
    out << '(' << e.node1 << ',' << e.node2 << ')';
    return out;
}

/******************************************************************************/

class Graph{
private:
    int size;
    double minCost,maxCost;
    double** cost; // edge cost matrix
    bool** graph; // graph edge matrix (aka. connectivity/adjacency matrix)
public:
    /**** constructors ****/
    Graph();
    Graph(int size);
    Graph(int size, double density, double minCost, double maxCost);
    Graph(ifstream& file);
    /**** destructor ****/
    // ~Graph();
    /**** accessors ****/
    int getSize();
    int getEdges();
    void printGraph(bool matrix);
    bool isAdjacent(int n, int m);
    vector<int> getNeighbors(int n);
    double getCost(int n, int m);
    void printShortestPaths(int n);
    double getAvgShortestPathCost(int n);
    void printMinSpanningTree(int n);
    /**** mutators ****/
    void addEdge(int n, int m);
    void deleteEdge(int n, int m);
    void setCost(int n, int m, double c);
    void setRandCost(int n, int m);
    /**** algorithms ****/
    bool isConnected();
    vector<node>* shortestPaths(int n);
    vector<edge> minSpanningTree(int n);
};

/**** constructors ****/

Graph::Graph(){}

Graph::Graph(int size){
    // construct empty graph
    this->size = size;
    this->minCost = 1;
    this->maxCost = 1;
    graph = new bool*[size];
    for(int i=0; i<size; i++)
        graph[i] = new bool[size];
    cost = new double*[size];
    for(int i=0; i<size; i++)
        cost[i] = new double[size];
}

Graph::Graph(int size, double density, double minCost, double maxCost){
    // construct random undirected graph
    this->size = size;
    this->minCost = minCost;
    this->maxCost = maxCost;
    /**** graph edge matrix ****/
    graph = new bool*[size];
    for(int i=0; i<size; i++)
        graph[i] = new bool[size];
    // symmetric matrix
    for(int i=0; i<size; i++)
        for(int j=i; j<size; j++)
            if(i==j) graph[i][j] = false; // no self-cycle
            else graph[i][j] = graph[j][i] = (prob()<density);
    /**** edge cost matrix ****/
    cost = new double*[size];
    for(int i=0; i<size; i++)
        cost[i] = new double[size];
    // symmetric matrix
    for(int i=0; i<size; i++)
        for(int j=i; j<size; j++)
            if(graph[i][j]) cost[i][j] = cost[j][i] = uniformRand(minCost,maxCost);
}

Graph::Graph(ifstream& file){
    // construct graph from file
    // format: 1st line - size, subsequent lines - i j cost
    int i,j;
    double c;
    /**** read size from file ****/
    file >> size;
    graph = new bool*[size];
    for(int i=0; i<size; i++)
        graph[i] = new bool[size];
    cost = new double*[size];
    for(int i=0; i<size; i++)
        cost[i] = new double[size];
    /**** read connections & costs from file ****/
    file >> i >> j >> c;
    minCost = maxCost = c;
    do{
        graph[i][j] = 1; // graph edge matrix
        cost[i][j] = c; // edge cost matrix
        if(c<minCost) minCost = c;
        if(c>maxCost) maxCost = c;
    }while(file >> i >> j >> c);
}

/**** accessors ****/

int Graph::getSize(){
    // number of nodes (aka vertices)
    return size;
}

int Graph::getEdges(){
    // number of edges
    int edges = 0;
    for(int i=0; i<size; i++)
        for(int j=i+1; j<size; j++)
            edges += graph[i][j];
    return edges;
}

void Graph::printGraph(bool matrix=true){
    if(matrix){
        // print adjacency matrix
        seperator(2*size-1);
        for(int i=0; i<size; i++){
            for(int j=0; j<size; j++)
                cout << graph[i][j] << ' ';
            cout << endl;
        }
        seperator(2*size-1);
    }else{
        // print neighbor lists
        seperator(2*size-1);
        for(int i=0; i<size; i++){
            cout << i << ": ";
            for(int j=0; j<size; j++)
                if(graph[i][j]==1) cout << j << ' ';
            cout << endl;
        }
        seperator(2*size-1);
    }
}

bool Graph::isAdjacent(int n, int m){
    // check if node n and m are connected
    return graph[n][m];
}

vector<int> Graph::getNeighbors(int n){
    // neighbors of node n
    vector<int> nbors;
    for(int i=0; i<size; i++)
        if(graph[n][i]==1) nbors.push_back(i);
    return nbors;
}

double Graph::getCost(int n, int m){
    // cost from node n to m
    return cost[n][m];
}

void Graph::printShortestPaths(int n){
    // print shortest paths from source node n to other nodes
    vector<node>* paths = shortestPaths(n); // array of shortest paths
    for(int i=0; i<size; i++){
        if(i==n) cout << "(source node) ";
        else if(paths[i].size()==0) cout << "(disconnected) ";
        cout << "shortest path from node " << n << " to node " << i << ":\n  ";
        cout << paths[i] << endl;
    }
}

double Graph::getAvgShortestPathCost(int n){
    // compute average shortest path cost from source node n
    vector<node>* paths = shortestPaths(n);
    double totalCost = 0;
    int connectedNodes = 0;
    for(int i=0; i<size; i++){
        if(paths[i].size()>0){ // there is a path that leads from node n to i
            connectedNodes++;
            totalCost += paths[i][paths[i].size()-1].value;
        }
    }
    return totalCost/connectedNodes;
}

void Graph::printMinSpanningTree(int n){
    // print edges & cost of minimum spanning tree
    vector<edge> mst = minSpanningTree(0);
    double treeCost = 0;
    for(int i=0; i<mst.size(); i++)
        treeCost += cost[mst[i].node1][mst[i].node2];
    cout << "minimum spanning tree:" << endl;
    cout << mst << endl;
    cout << "tree cost: " << treeCost << endl;
}

/**** mutators ****/

void Graph::addEdge(int n, int m){
    // connect node n and m
    graph[n][m] = graph[m][n] = 1;
}

void Graph::deleteEdge(int n, int m){
    // disconnect node n and m
    graph[n][m] = graph[m][n] = 0;
    cost[n][m] = cost[m][n] = 0;
}

void Graph::setCost(int n, int m, double c){
    // set cost from node n to m as c
    cost[n][m] = cost[m][n] = c;
}

void Graph::setRandCost(int n, int m){
    // randomly set cost from node n to m
    cost[n][m] = cost[m][n] = uniformRand(minCost,maxCost);
}

/**** algorithms ****/

bool Graph::isConnected(){
    // check if graph is connected
    int oldSize, closedSize=0;
    bool* closed = new bool[size]; // expanded nodes
    bool* open = new bool[size]; // nodes to be expanded
    for(int i=0; i<size; i++)
        closed[i] = open[i] = false;
    open[0] = true; // initialization: start from node 0
    while(closedSize<size){
        oldSize = closedSize;
        for(int i=0; i<size; i++){
            if(open[i] && !closed[i]){ // new nodes to expand
                closed[i] = true;
                closedSize++;
                for(int j=0; j<size; j++)
                    open[j] = open[j]||graph[i][j];
            }
            // for(int i=0; i<size; i++) cout << closed[i] << ' ';
            // cout << "\n";
            // for(int i=0; i<size; i++) cout << open[i] << ' ';
            // cout << "\n\n";
        }
        if(oldSize==closedSize) return false;
    }
    return true;
}

vector<node>* Graph::shortestPaths(int n){
    // Dijkstra's shortest path from source node n to other nodes
    // return an array of shortest path vectors (of nodes)
    const int inf = size*maxCost;
    vector<node>* paths = new vector<node>[size]; // shortest paths
    int current; // node currently being explored
    int* prev = new int[size];
    bool* closed = new bool[size]; // expanded nodes
    double* nodeCost = new double[size]; // tentative cost from node n
    double c; // tentative cost
    priorityQueue q; // priority queue for uniform cost search

    /**** initialization ****/
    nodeCost[n] = 0; // start from node n
    for(int i=0; i<size; i++){
        closed[i] = false;
        prev[i] = -1; // flag value
        if(i!=n) nodeCost[i] = inf;
        // prioirty: -cost
        node tmpNode(i,-nodeCost[i]);
        q.add(tmpNode);
    }
    // q.print();

    /**** uniform cost search ****/
    // loop until queue is empty or remaining nodes are disconnected
    while(!q.isEmpty() && q.peekHeadNode().value!=-inf){
        current = q.popHeadNode().label; // pop node with lowest cost
        for(int i=0; i<size; i++)
            // unvisited neighbors of current node
            if(!closed[i] && graph[current][i]){
                c = nodeCost[current]+cost[current][i];
                // update tentative cost
                if(c<nodeCost[i]){
                    nodeCost[i] = c;
                    prev[i] = current;
                    q.chgPriority(i,-c); // update priority queue
                }
            }
        closed[current] = true;
        // q.print(); // show the search
    }

    // for(int i=0; i<size; i++) cout << prev[i] << ' ';
    // cout << endl;

    /**** path from node n to other nodes ****/
    for(int m=0; m<size; m++){
        if(prev[m]!=-1){ // node m connected to n
            int i = m; // iterator from node m to n
            while(i!=n){ // continue if have not traced back to node n
                node tmpNode(i,nodeCost[i]);
                paths[m].insert(paths[m].begin(),tmpNode);
                i = prev[i];
            }
            node tmpNode(n,nodeCost[n]);
            paths[m].insert(paths[m].begin(),tmpNode); // prepend source node n
        }
    }
    return paths;
}

vector<edge> Graph::minSpanningTree(int n){
    // Prim's minimum spanning tree for a connected graph
    // return a vector of edges
    const int inf = size*maxCost;
    vector<edge> mst; // minimum spanning tree
    int current; // node currently being explored
    int* prev = new int[size];
    double* nodeCost = new double[size]; // tentative cheapest edge cost
    priorityQueue q; // priority queue for uniform cost search

    /**** initialization ****/
    nodeCost[n] = 0; // start from node n
    for(int i=0; i<size; i++){
        prev[i] = -1; // flag value
        if(i!=n) nodeCost[i] = inf;
        // prioirty: -cost
        node tmpNode(i,-nodeCost[i]);
        q.add(tmpNode);
    }
    // q.print();

    /**** uniform cost search ****/
    // loop until queue is empty
    while(!q.isEmpty()){
        current = q.popHeadNode().label; // pop node with lowest cost
        for(int i=0; i<size; i++)
            if(graph[current][i]) // neighbors of current node
                if(q.contains(i) && cost[current][i]<nodeCost[i]){
                    nodeCost[i] = cost[current][i];
                    prev[i] = current;
                    q.chgPriority(i,-cost[current][i]);
                }
        // q.print(); // show the search
    }

    /**** collect edges into mst ****/
    for(int i=0; i<size; i++)
        if(i!=n) mst.push_back(edge(prev[i],i));
    return mst;
}

#endif
