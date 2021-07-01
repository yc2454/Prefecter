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

VertexProperty get_source_property(Graph g, vertex_descriptor_t target) {

    // find in edges to target
    boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = boost::in_edges(target, g);
    // find source
    vertex_descriptor_t src = boost::source(*ei, g);
    // get property
    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);
    VertexProperty vp = boost::get(pmap, src);

    return vp;

}

void store_load_bypassing(Graph *g, vertex_descriptor_t root) {

    vector<VertexProperty> adj_vertices;

    adj_vertices = find_adj_vertices(*g, root);

    uint64_t cur_start;

    for (VertexProperty vp : adj_vertices)
    {
        if (vp.ty != NONTERM) {
            continue;
        }
        else {
            cur_start = vp.source;
        }    
    }

    

}

int main() {

    Graph g = graph_create();

    vertex_descriptor_t vd1 = add_vertex(&g, 0x18279e26, 0, ADDR);
    vertex_descriptor_t vd2 = add_vertex(&g, 20, 0, REG);
    
    pair<edge_descriptor_t, bool> ed_suc = add_edge(&g, vd1, vd2);

    VertexProperty vp = get_source_property(g, vd2);

    cout << vp.value << endl;

    return 0;

}