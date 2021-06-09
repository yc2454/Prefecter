#include <iostream>
#include <vector>
#include <string>
using namespace std;

enum vertex_types {CONSTANT, REG, ADDR};

struct Vertex {
    vertex_types ty;
    string name;
    int id;
} Vertex;
 
// Data structure to store a graph edge
struct Edge {
    struct Vertex src, dest;
    string operation;
} Edge;
 
// A class to represent a graph object
class Graph
{
public:
    // a vector of vectors to represent an adjacency list
    vector<vector<struct Vertex>> adjList;
 
    // Graph Constructor
    Graph(vector<struct Edge> const &edges, int N)
    {
        // resize the vector to hold `N` elements of type `vector<int>`
        adjList.resize(N);
 
        // add edges to the directed graph
        for (auto &edge: edges)
        {
            // insert at the end
            adjList[edge.src.id].push_back(edge.dest);
 
            // uncomment the following code for undirected graph
            // adjList[edge.dest].push_back(edge.src);
        }
    }


};