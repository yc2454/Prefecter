
#include <iostream> // for std::cout
#include <utility> // for std::pair
#include <algorithm> // for std::for_each
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
using namespace boost;

int main(int,char*[])
{
    struct VertexProperty{

        int id, size;

    };

    struct EdgeProperty {

        int id;
        int weight;

    };
    
    typedef boost::adjacency_list<boost::vecS, boost::vecS,       
             boost::bidirectionalS, VertexProperty, EdgeProperty>
        Graph;
    
    typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor_t;

    Graph graph; //声明一个图
    VertexProperty vp; // 声明一个顶点属性
    EdgeProperty ep; // 声明一个边属性

    vertex_descriptor_t src, dst; // 声明两个顶点的descriptor

    vp.id = 100;
    vp.size = 200;
    src = boost::add_vertex(vp, graph); // 给图里添加顶点
    dst = boost::add_vertex(vp, graph); // 给图里添加顶点
    boost::add_edge(src, dst, ep, graph); // 给src和dst之间添加边，属性是ep.

}