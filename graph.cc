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

vector<vertex_descriptor_t> find_adj_vertices(Graph g, vertex_descriptor_t v) {
    typename boost::graph_traits<Graph>::adjacency_iterator ai;
    typename boost::graph_traits<Graph>::adjacency_iterator ai_end;

    for (tie(ai, ai_end) = boost::adjacent_vertices(v, g); ai != ai_end; ++ai)
        ;
}

typedef boost::adjacency_list<boost::setS,boost::setS,boost::undirectedS, uint32_t, float> AdjacencyList;
typedef boost::graph_traits<AdjacencyList>::adjacency_iterator AdjacencyIterator;

int main() {
    AdjacencyList adjacency_list;;

    boost::add_edge(
            boost::add_vertex(10, adjacency_list),
            boost::add_vertex(20, adjacency_list),
            1.5f,
            adjacency_list
        );

    boost::add_edge(
            boost::add_vertex(30, adjacency_list),
            boost::add_vertex(40, adjacency_list),
            2.5f,
            adjacency_list
        );

    AdjacencyList::vertex_iterator i, end;

    for (boost::tie(i, end) = boost::vertices(adjacency_list); i != end; i++) {
        AdjacencyIterator ai, a_end; 

        boost::tie(ai, a_end) = boost::adjacent_vertices(*i, adjacency_list);
        for (; ai != a_end; ai++) { 
            std::cout << adjacency_list[*ai] << "\t";
        }
    }
}

// int main() {

//     Graph g = graph_create();

//     // vertex_descriptor_t vd1 = add_vertex(&g, "0x18279e26", ADDR);
//     // vertex_descriptor_t vd2 = add_vertex(&g, "$rax", REG);
    
//     // pair<edge_descriptor_t, bool> ed_suc = add_edge(&g, vd1, vd2, "add");

//     // typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//     // IndexMap index = get(boost::vertex_index, g);

//     // std::cout << "vertices(g) = ";
//     // typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
//     // std::pair<vertex_iter, vertex_iter> vp;
//     // for (vp = vertices(g); vp.first != vp.second; ++vp.first)
//     //   std::cout << index[*vp.first] <<  " ";
//     // std::cout << std::endl;
//     // // ...

//     return 0;

// }