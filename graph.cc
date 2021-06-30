#include "graph.h"
using namespace std;

EdgeProperty ep; 

Graph graph_create() {
    
    Graph graph;

    return graph;

}

vertex_descriptor_t add_vertex(Graph * g, uint64_t value, uint64_t source, vertex_type t) {
    
    VertexProperty vp; 

    vp.ty = t;

    vp.value = value;

    vp.source = source;
    
    vertex_descriptor_t vd = boost::add_vertex(vp, *g);

    return vd;
}

pair<edge_descriptor_t, bool> add_edge(Graph * g, vertex_descriptor_t src, vertex_descriptor_t dst){

    EdgeProperty ep;

    edge_descriptor_t ed;
    bool inserted;

    tie(ed, inserted) = boost::add_edge(src, dst, *g);
    pair<edge_descriptor_t, bool> ret (ed, inserted);

    return ret;

}

vector<VertexProperty> find_adj_vertices(Graph g, vertex_descriptor_t v) {
    
    typename boost::graph_traits<Graph>::adjacency_iterator ai;
    typename boost::graph_traits<Graph>::adjacency_iterator ai_end;

    vector<VertexProperty> adjs;

    for (tie(ai, ai_end) = boost::adjacent_vertices(v, g); ai != ai_end; ++ai) {
        adjs.push_back(g[*ai]);
    }

    return adjs;

}

int main() {

    Graph g = graph_create();

    vertex_descriptor_t vd1 = add_vertex(&g, 0, 0x18279e26, ADDR);
    vertex_descriptor_t vd2 = add_vertex(&g, 0, 0x18279e26, REG);
    
    pair<edge_descriptor_t, bool> ed_suc = add_edge(&g, vd1, vd2);

    vector<VertexProperty> adjs = find_adj_vertices(g, vd1);

    cout << adjs[0].source << endl; 

    return 0;

}