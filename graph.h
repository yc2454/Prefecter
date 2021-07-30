#include <iostream> // for std::cout
#include <utility> // for std::pair
#include <algorithm> // for std::for_each
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
using namespace std;

enum vertex_type {CONST, REG, ADDR, NONTERM};

// The properties of the vertex
// value stands for the op code when ty == NONTERM
// value stands for a constant/addr/reg otherwise
struct VertexProperty{

    vertex_type ty;
    uint64_t value;
    uint64_t source;

};

struct EdgeProperty {

};

typedef boost::adjacency_list<boost::vecS, boost::vecS,       
            boost::bidirectionalS, VertexProperty, EdgeProperty>
    Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor_t;

Graph graph_create();

vertex_descriptor_t add_vertex(Graph * g, uint64_t value, uint64_t source, vertex_type t);

pair<edge_descriptor_t, bool> add_edge(Graph * g, vertex_descriptor_t src, vertex_descriptor_t dst);

vector<VertexProperty> find_adj_vertices(Graph g, vertex_descriptor_t v);

void print_1();