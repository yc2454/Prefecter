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

vector<vertex_descriptor_t> find_source_vertices(Graph g, vertex_descriptor_t v) {
    
    // find in edges to target
    boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
    boost::tie(ei, ei_end) = boost::in_edges(v, g);

    vector<vertex_descriptor_t> adjs;
    vertex_descriptor_t src;

    for (; ei != ei_end; ++ei) {
        src = boost::source(*ei, g);
        adjs.push_back(src);
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

// Find the source vertex of the input target in g
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

void print_vertex(Graph g, vertex_descriptor_t v) {

    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);

    VertexProperty property = boost::get(pmap, v);

    cout << property.value << endl;

}

void print_graph(Graph g, vertex_descriptor_t root) {
    
    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);

    vector<vertex_descriptor_t> sources = find_source_vertices(g, root);

    for (vector<vertex_descriptor_t>::iterator i = sources.begin(); i != sources.end(); i++) {
        print_vertex(g, *i);
        print_graph(g, *i);
    }

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
    while (1) {
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
        else if (num_sources == 1) {
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

    vertex_descriptor_t root = add_vertex(&g, 11111, 0, NONTERM);
    vertex_descriptor_t const1 = add_vertex(&g, 28, 0, CONST);
    vertex_descriptor_t ld1 = add_vertex(&g, 22222, 0x12345678, NONTERM);
    vertex_descriptor_t add1 = add_vertex(&g, 11111, 0, NONTERM);
    vertex_descriptor_t const2 = add_vertex(&g, 8, 0, CONST);
    vertex_descriptor_t reg = add_vertex(&g, 26, 0, REG);

    add_edge(&g, const1, root);
    add_edge(&g, ld1, root);
    add_edge(&g, add1, ld1);
    add_edge(&g, const2, add1);
    add_edge(&g, reg, add1);

    print_graph(g, root);

    return 0;

}