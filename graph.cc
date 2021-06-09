
#include <iostream> // for std::cout
#include <utility> // for std::pair
#include <algorithm> // for std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
using namespace boost;
 
int main(int,char*[])
{
    typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
    // Make convenient labels for the vertices
    enum { A, B, C, D, E, N }; //代表 0 ，1，2，3，4 顶点,其中N为顶点数
    const int num_vertices = N;//N的值是5
    const char* name = "ABCDE";
    //图中的边
    typedef std::pair<int, int> Edge;
    Edge edge_array[] = { Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C),
                            Edge(C,E), Edge(B,D), Edge(D,E) };
    const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);
    // 创建一个拥有5个顶点的图对象
    Graph g(num_vertices);
    // 给图对象添加边
    for (int i = 0; i < num_edges; ++i)
      add_edge(edge_array[i].first, edge_array[i].second, g);//其中first表示第一个顶点，second表示第二个顶点，两个顶点连接
   return 0;
}