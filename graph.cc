#include "graph.h"

EdgeProperty ep; 

Graph graph_create() {
    
    Graph graph;

    return graph;

}

vertex_descriptor_t add_vertex(Graph * g, string name, vertex_type t) {
    
    VertexProperty vp; 

    vp.name = name;
    vp.ty = t;

    vertex_descriptor_t vd = boost::add_vertex(vp, *g);

    return vd;
}

pair<edge_descriptor_t, bool> add_edge(Graph * g, vertex_descriptor_t src, vertex_descriptor_t dst, string op){

    EdgeProperty ep;
    ep.operation = op;

    edge_descriptor_t ed;
    bool inserted;

    tie(ed, inserted) = boost::add_edge(src, dst, *g);
    pair<edge_descriptor_t, bool> ret (ed, inserted);

    return ret;

}


int main() {

    Graph g = graph_create();

    vertex_descriptor_t vd1 = add_vertex(&g, "0x18279e26", ADDR);
    vertex_descriptor_t vd2 = add_vertex(&g, "$rax", REG);
    
    pair<edge_descriptor_t, bool> ed_suc = add_edge(&g, vd1, vd2, "add");
    
}