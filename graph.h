#include </Users/yalucai/Downloads/igraph-0.9.4/include/igraph.h>
#include <string>

enum vertex_types {CONSTANT, REG, ADDR};

using namespace std;

struct Vertex
{
    vertex_types ty;
    char * name;
    int id;
};

/* create a new graph with n vertices labeled 0..n-1 and no edges */
igraph_t graph_create(int n);

/* free all space used by graph */
void graph_destroy(igraph_t);

/* add an edge to an existing graph */
/* doing this more than once may have unpredictable results */
void graph_add_edge(igraph_t, int source, int sink);

/* return the number of vertices/edges in the graph */
int graph_vertex_count(igraph_t);
int graph_edge_count(igraph_t);

void graph_add_vertex(igraph_t *g, struct Vertex v);