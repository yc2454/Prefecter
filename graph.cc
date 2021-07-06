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

vertex_descriptor_t get_nonterm_source(Graph g, vertex_descriptor_t target) {
    
    // find in edges to target
    boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = boost::in_edges(target, g);
    // find source
    vertex_descriptor_t src, nonterm_src;
    VertexProperty vp;
    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);
    
    for (; ei != ei_end; ++ei) {
        src = boost::source(*ei, g);
        vp = boost::get(pmap, src);
        if (vp.ty == NONTERM) 
            nonterm_src = src;
        else 
            nonterm_src = -100;
    }

    return nonterm_src;
    
}

void store_load_bypassing(Graph *g, vertex_descriptor_t root) {

    int num_sources;

    // The vector to keep track of the vertices along the path
    deque<vertex_descriptor_t> circle;

    // find the first non-terminal source
    vertex_descriptor_t start = get_nonterm_source(*g, root);

    // the current place on the path
    vertex_descriptor_t cur;
    // properties of the vertices
    VertexProperty cur_property;
    VertexProperty start_property;

    cur = start;
    circle.push_front(start);

    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, *g);

    // search up along the path
    while (1)
    {
        num_sources = find_adj_vertices(*g, cur).size();
        // when there are 2 sources
        if (num_sources == 2) {
            // check whether cur complete the circle
            cur_property = boost::get(pmap, cur);
            start_property = boost::get(pmap, cur);
            // if a circle is completed, remove all vertices in the circle
            if (cur_property.source == start_property.source) {
                for (int i = 0; circle[i] != cur; i++) {
                    boost::remove_vertex(circle[i], *g);
                }
            }
            // change the start of the circle
            start = cur;
        }
        else if (num_sources == 0) {
            break;
        }
        else if (num_sources) {
            // check whether cur complete the circle
            cur_property = boost::get(pmap, cur);
            start_property = boost::get(pmap, cur);
            // if a circle is completed, remove all vertices in the circle
            if (cur_property.source == start_property.source) {
                for (int i = 0; circle[i] != cur; i++) {
                    boost::remove_vertex(circle[i], *g);
                }
            }
            start = cur;
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