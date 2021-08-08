#include "graph.h"
using namespace std;
#define NO_NONTERM -100
#define ADD 11111
#define LOAD 22222

// Problem Aug 3, the reordering of vertices after removing

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

    // cout << "the number of source here is " << adjs.size() << endl;
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
            return src;
    }

    return NULL;
    
}

vertex_descriptor_t get_target(Graph g, vertex_descriptor_t v) {

    // find in edges to target
    cout << "got here -1" << endl;
    boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;
    cout << "got here 0" << endl;
    boost::tie(ei, ei_end) = boost::out_edges(v, g);
    // find source
    vertex_descriptor_t target;
    
    cout << "got here 1" << endl;
    target = boost::target(*ei, g);
    cout << "got here 2" << endl;
    return target;
    
        
}

// print out the graph in matrix from
void print_vertices(Graph *g) {
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(*g);

    boost::graph_traits<Graph>::vertex_iterator ui, ui_end;

    edge_descriptor_t e;
    bool to, from;

    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, *g);
    VertexProperty vp;

    cout << "  ";
    for (; vi != vi_end; vi++) {
        vp = boost::get(pmap, *vi);
        cout << vp.source << " ";
    }
    cout << endl;

    boost::tie(vi, vi_end) = boost::vertices(*g);
    for (; vi != vi_end; vi++) {
        vp = boost::get(pmap, *vi);
        cout << vp.source << " ";
        boost::tie(ui, ui_end) = boost::vertices(*g);
        for (; ui != ui_end; ui++) {
            boost::tie(e, to) = boost::edge(*ui, *vi, *g);
            boost::tie(e, from) = boost::edge(*vi, *ui, *g);
            if (to || from)
                cout << 1 << " ";
            else 
                cout << 0 << " ";  
        }
        
        cout << endl;
    }

    cout << endl;
}

// void print_vertex(Graph g, vertex_descriptor_t v) {

//     boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);

//     VertexProperty property = boost::get(pmap, v);

//     cout << property.value << endl;

// }

// void print_graph(Graph g, vertex_descriptor_t root) {

//     print_vertex(g, root);
//     cout << "current vertex is: " << root << " and its neighbors are: " << endl;
    
//     boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);

//     vector<vertex_descriptor_t> sources = find_source_vertices(g, root);

//     for (vector<vertex_descriptor_t>::iterator i = sources.begin(); i != sources.end(); i++) {
//         cout << *i << " ";
//     }
//     cout << endl;

//     for (vector<vertex_descriptor_t>::iterator i = sources.begin(); i != sources.end(); i++) {
//         print_graph(g, *i);
//     }

// }

void remove_self_edge(Graph * g) {
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
    boost::tie(vi, vi_end) = boost::vertices(*g);

    // self-edge. Don't understand why it exists yet
    edge_descriptor_t start_self;
    bool start_self_exists;

    for (; vi != vi_end; vi++) {
        boost::tie(start_self, start_self_exists) = boost::edge(*vi, *vi, *g);
        if (start_self_exists) {
            cout << "removed self edge for " << *vi << endl;
            boost::remove_edge(start_self, *g);
        }
    }
}

void store_load_bypassing(Graph * g, vertex_descriptor_t root) {

    int num_sources;

    // The vector to keep track of the vertices along the path
    deque<vertex_descriptor_t> circle;

    // find the first non-terminal source
    vertex_descriptor_t start;

    // the current place on the path
    vertex_descriptor_t cur = root;

    // the next place in the path
    vertex_descriptor_t next;

    // the target of the start point
    vertex_descriptor_t target_of_start;

    // properties of the vertices
    VertexProperty cur_property;
    VertexProperty start_property;
    VertexProperty next_property;

    // properties for testing
    VertexProperty p;

    // self-edge. Don't understand why it exists yet
    edge_descriptor_t start_self;
    bool start_self_exists;

    boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, *g);

    // search up along the path
    while (1) {
        
        num_sources = find_source_vertices(*g, cur).size();
        
        // when we are at an ADD node
        if (num_sources == 2) {
            cout << "two source!" << endl;
            // check if the ADD node has a child who is a nonterm
            cur = get_nonterm_source(*g, cur);
            cur_property = boost::get(pmap, cur);
            if (cur == NULL) 
                break;
            else if (cur_property.value == LOAD) {
                start = cur;
                circle.push_back(start);
            }
                

        }

        // when we reach a leaf node
        else if (num_sources == 0) {
            cout << "no more source!" << endl;
            cout << "BEFORE EXITING, the graph contains " << boost::num_vertices(*g) << " vertices" << endl;
            break;
        }

        // when we reach a LOAD node
        else if (num_sources == 1) {

            cout << "only one source!" << endl;

            // circle.push_back(cur);

            next = get_nonterm_source(*g, cur);
            if (next == NULL)
                break;
            
            // check whether cur complete the circle
            cur_property = boost::get(pmap, cur);
            next_property = boost::get(pmap, next);
            start_property = boost::get(pmap, start);

            // if a circle is completed, remove all vertices in the circle
            if (next_property.source == start_property.source) {
                
                circle.push_back(next);

                // cout << "start removing:" << endl;
                // cout << "the size of the circle is: " << circle.size() << endl;

                for (int i = 1; i < circle.size(); i++) {
                    // cout << i  << " " << circle[i] << " ";
                    boost::remove_vertex(circle[i], *g);
                }
                cout << endl;

                // reconnect the graph
                // print_vertices(g);
                // cout << start << endl;
                cout << "got here -1" << endl;
                boost::graph_traits<Graph>::out_edge_iterator ei, ei_end;
                cout << "got here 0" << endl;
                boost::tie(ei, ei_end) = boost::out_edges(start, *g);
                // find source
                vertex_descriptor_t target;
                
                cout << "got here 1" << endl;
                target = boost::target(*ei, *g);
                cout << "got here 2" << endl;

                target_of_start = target;

                add_edge(g, next, target_of_start);
                // cout << "reconnect target " << target_of_start << " of start " << start << " to the next vertex " << next;
                
                // clear the circle
                circle.clear();
                // the new start of the circle is the next vertex
                start = next;
            }
            else {
                circle.push_back(next);
            }

            cur = next;
            
        }
    }
    
}

void remove_vertex_in_func(Graph *g, vertex_descriptor_t v) {
    boost::remove_vertex(v, *g);
}

int main() {

    Graph g = graph_create();

    vertex_descriptor_t root = add_vertex(&g, ADD, 1, NONTERM);
    vertex_descriptor_t const1 = add_vertex(&g, 28, 2, CONST);
    vertex_descriptor_t ld1 = add_vertex(&g, LOAD, 3, NONTERM);
    vertex_descriptor_t ld2 = add_vertex(&g, LOAD, 4, NONTERM);
    vertex_descriptor_t ld3 = add_vertex(&g, LOAD, 5, NONTERM);
    vertex_descriptor_t ld4 = add_vertex(&g, LOAD, 6, NONTERM);
    vertex_descriptor_t ld5 = add_vertex(&g, LOAD, 3, NONTERM);
    // vertex_descriptor_t add1 = add_vertex(&g, ADD, 0, NONTERM);
    // vertex_descriptor_t const2 = add_vertex(&g, 8, 0, CONST);
    // vertex_descriptor_t ld2 = add_vertex(&g, LOAD, 0x12345678, NONTERM);
    // vertex_descriptor_t ld3 = add_vertex(&g, LOAD, 12, NONTERM);

    add_edge(&g, const1, root);
    add_edge(&g, ld1, root);
    add_edge(&g, ld2, ld1);
    add_edge(&g, ld3, ld2);
    add_edge(&g, ld4, ld3);
    add_edge(&g, ld5, ld4);
    // add_edge(&g, const2, add1);
    // add_edge(&g, ld2, add1);
    // add_edge(&g, ld3, ld2);

    cout << "before pruning" << endl;
    cout << "the graph contains " << boost::num_vertices(g) << " vertices" << endl;
    print_vertices(&g);
    // print_graph(g, root);
    store_load_bypassing(&g, root);
    cout << "finished store load bypassing" << endl;
    remove_self_edge(&g);
    // remove_vertex_in_func(&g, root);
    // remove_vertex_in_func(&g, ld1);
    cout << "after pruning" << endl;
    cout << "the graph contains " << boost::num_vertices(g) << " vertices" << endl;
    print_vertices(&g);
    // print_graph(g, root);

    return 0;

}
