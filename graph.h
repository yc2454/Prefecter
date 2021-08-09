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

typedef boost::adjacency_list<boost::vecS, boost::listS,       
            boost::bidirectionalS, VertexProperty, EdgeProperty>
    Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_descriptor_t;

EdgeProperty ep; 

// Graph graph_create() {
    
//     Graph graph;

//     return graph;

// }

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

// vector<vertex_descriptor_t> find_adj_vertices(Graph g, vertex_descriptor_t v) {
    
//     typename boost::graph_traits<Graph>::adjacency_iterator ai;
//     typename boost::graph_traits<Graph>::adjacency_iterator ai_end;

//     vector<vertex_descriptor_t> adjs;

//     for (tie(ai, ai_end) = boost::adjacent_vertices(v, g); ai != ai_end; ++ai) {
//         adjs.push_back(*ai);
//     }

//     return adjs;

// }

// VertexProperty get_source_property(Graph g, vertex_descriptor_t target) {

//     // find in edges to target
//     boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
//     boost::tie(ei, ei_end) = boost::in_edges(target, g);
//     // find source
//     vertex_descriptor_t src = boost::source(*ei, g);
//     // get property
//     boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);
//     VertexProperty vp = boost::get(pmap, src);

//     return vp;

// }

// vertex_descriptor_t get_nonterm_source(Graph g, vertex_descriptor_t target) {
    
//     // find in edges to target
//     boost::graph_traits<Graph>::in_edge_iterator ei, ei_end;
//     boost::tie(ei, ei_end) = boost::in_edges(target, g);
//     // find source
//     vertex_descriptor_t src, nonterm_src;
//     VertexProperty vp;
//     boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, g);
    
//     for (; ei != ei_end; ++ei) {
//         src = boost::source(*ei, g);
//         vp = boost::get(pmap, src);
//         if (vp.ty == NONTERM) 
//             nonterm_src = src;
//         else 
//             nonterm_src = -100;
//     }

//     return nonterm_src;
    
// }

// void store_load_bypassing(Graph *g, vertex_descriptor_t root) {

//     int num_sources;

//     // The vector to keep track of the vertices along the path
//     deque<vertex_descriptor_t> circle;

//     // find the first non-terminal source
//     vertex_descriptor_t start = get_nonterm_source(*g, root);

//     // the current place on the path
//     vertex_descriptor_t cur;
//     // properties of the vertices
//     VertexProperty cur_property;
//     VertexProperty start_property;

//     cur = start;
//     circle.push_front(start);

//     boost::property_map<Graph, boost::vertex_bundle_t>::type pmap = boost::get(boost::vertex_bundle, *g);

//     // search up along the path
//     while (1)
//     {
//         num_sources = find_adj_vertices(*g, cur).size();
//         // when there are 2 sources
//         if (num_sources == 2) {
//             // check whether cur complete the circle
//             cur_property = boost::get(pmap, cur);
//             start_property = boost::get(pmap, cur);
//             // if a circle is completed, remove all vertices in the circle
//             if (cur_property.source == start_property.source) {
//                 for (int i = 0; circle[i] != cur; i++) {
//                     boost::remove_vertex(circle[i], *g);
//                 }
//             }
//             // change the start of the circle
//             start = cur;
//         }
//         else if (num_sources == 0) {
//             break;
//         }
//         else if (num_sources) {
//             // check whether cur complete the circle
//             cur_property = boost::get(pmap, cur);
//             start_property = boost::get(pmap, cur);
//             // if a circle is completed, remove all vertices in the circle
//             if (cur_property.source == start_property.source) {
//                 for (int i = 0; circle[i] != cur; i++) {
//                     boost::remove_vertex(circle[i], *g);
//                 }
//             }
//             start = cur;
//         }
//     }
    
// }

// // void print_1();