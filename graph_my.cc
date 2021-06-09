#include "graph.h"

using namespace std;

int print_attributes(const igraph_t *g) {

    igraph_vector_t gtypes, vtypes, etypes;
    igraph_strvector_t gnames, vnames, enames;
    long int i;

    igraph_vector_t vec;
    igraph_strvector_t svec;
    long int j;

    igraph_vector_init(&gtypes, 0);
    igraph_vector_init(&vtypes, 0);
    igraph_vector_init(&etypes, 0);
    igraph_strvector_init(&gnames, 0);
    igraph_strvector_init(&vnames, 0);
    igraph_strvector_init(&enames, 0);

    igraph_cattribute_list(g, &gnames, &gtypes, &vnames, &vtypes,
                           &enames, &etypes);

    /* Graph attributes */
    for (i = 0; i < igraph_strvector_size(&gnames); i++) {
        printf("%s=", STR(gnames, i));
        if (VECTOR(gtypes)[i] == IGRAPH_ATTRIBUTE_NUMERIC) {
            igraph_real_printf(GAN(g, STR(gnames, i)));
            putchar(' ');
        } else {
            printf("\"%s\" ", GAS(g, STR(gnames, i)));
        }
    }
    printf("\n");

    for (i = 0; i < igraph_vcount(g); i++) {
        long int j;
        printf("Vertex %li: ", i);
        for (j = 0; j < igraph_strvector_size(&vnames); j++) {
            printf("%s=", STR(vnames, j));
            if (VECTOR(vtypes)[j] == IGRAPH_ATTRIBUTE_NUMERIC) {
                igraph_real_printf(VAN(g, STR(vnames, j), i));
                putchar(' ');
            } else {
                printf("\"%s\" ", VAS(g, STR(vnames, j), i));
            }
        }
        printf("\n");
    }

    for (i = 0; i < igraph_ecount(g); i++) {
        long int j;
        printf("Edge %li (%i-%i): ", i, (int)IGRAPH_FROM(g, i), (int)IGRAPH_TO(g, i));
        for (j = 0; j < igraph_strvector_size(&enames); j++) {
            printf("%s=", STR(enames, j));
            if (VECTOR(etypes)[j] == IGRAPH_ATTRIBUTE_NUMERIC) {
                igraph_real_printf(EAN(g, STR(enames, j), i));
                putchar(' ');
            } else {
                printf("\"%s\" ", EAS(g, STR(enames, j), i));
            }
        }
        printf("\n");
    }

    /* Check vector-based query functions */
    igraph_vector_init(&vec, 0);
    igraph_strvector_init(&svec, 0);

    for (j = 0; j < igraph_strvector_size(&vnames); j++) {
        if (VECTOR(vtypes)[j] == IGRAPH_ATTRIBUTE_NUMERIC) {
            igraph_cattribute_VANV(g, STR(vnames, j), igraph_vss_all(), &vec);
            for (i = 0; i < igraph_vcount(g); i++) {
                igraph_real_t num = VAN(g, STR(vnames, j), i);
                if (num != VECTOR(vec)[i] &&
                    (!isnan(num) || !isnan(VECTOR(vec)[i]))) {
                    exit(51);
                }
            }
        } else {
            igraph_cattribute_VASV(g, STR(vnames, j), igraph_vss_all(), &svec);
            for (i = 0; i < igraph_vcount(g); i++) {
                const char *str = VAS(g, STR(vnames, j), i);
                if (strcmp(str, STR(svec, i))) {
                    exit(52);
                }
            }
        }
    }

    for (j = 0; j < igraph_strvector_size(&enames); j++) {
        if (VECTOR(etypes)[j] == IGRAPH_ATTRIBUTE_NUMERIC) {
            igraph_cattribute_EANV(g, STR(enames, j),
                                   igraph_ess_all(IGRAPH_EDGEORDER_ID), &vec);
            for (i = 0; i < igraph_ecount(g); i++) {
                igraph_real_t num = EAN(g, STR(enames, j), i);
                if (num != VECTOR(vec)[i] &&
                    (!isnan(num) || !isnan(VECTOR(vec)[i]))) {
                    exit(53);
                }
            }
        } else {
            igraph_cattribute_EASV(g, STR(enames, j),
                                   igraph_ess_all(IGRAPH_EDGEORDER_ID), &svec);
            for (i = 0; i < igraph_ecount(g); i++) {
                const char *str = EAS(g, STR(enames, j), i);
                if (strcmp(str, STR(svec, i))) {
                    exit(54);
                }
            }
        }
    }

    igraph_strvector_destroy(&svec);
    igraph_vector_destroy(&vec);

    igraph_strvector_destroy(&enames);
    igraph_strvector_destroy(&vnames);
    igraph_strvector_destroy(&gnames);
    igraph_vector_destroy(&etypes);
    igraph_vector_destroy(&vtypes);
    igraph_vector_destroy(&gtypes);

    return 0;
}

igraph_t graph_create() {
    
    igraph_t g;
    igraph_vector_t y;

    igraph_empty_attrs(&g, 0, 1, 0);

    // set the vertex attributes for the graph
    
    return g;

}

/* free all space used by graph */
void graph_destroy(igraph_t *graph) {
    igraph_destroy(graph);
}

/* add an edge to an existing graph */
/* doing this more than once may have unpredictable results */
void graph_add_edge(igraph_t *g, struct Vertex source, struct Vertex sink, const char* operation){

    igraph_add_edge(g, source.id, sink.id);

    igraph_integer_t eid;

    igraph_get_eid(g, &eid, source.id, sink.id, 1, 0);
    
    // add attributes to the edge
    igraph_cattribute_EAS_set(g, "operation", eid, operation);

}

/* return the number of vertices/edges in the graph */
int graph_vertex_count(const igraph_t *graph) {
    return igraph_vcount(graph);
}
int graph_edge_count(const igraph_t *graph) {
    return igraph_ecount(graph);
}

void graph_add_vertex(igraph_t *g, struct Vertex v) {

    // add the vertex
    igraph_add_vertices(g, 1, 0);

    igraph_integer_t vid = 1;
    SETVAN(g, 1, vid, 1);
    // igraph_cattribute_VAS_set(g, "type", vid, v.name.c_str());

}

// /* return the out-degree of a vertex */
// int graph_out_degree(igraph_t, int source);

// /* return 1 if edge (source, sink) exists), 0 otherwise */
// int graph_has_edge(igraph_t, int source, int sink);

// /* invoke f on all edges (u,v) with source u */
// /* supplying data as final parameter to f */
// /* no particular order is guaranteed */
// void graph_foreach(igraph_t g, int source,
//         void (*f)(igraph_t g, int source, int sink, void *data),
//         void *data);

int main() {
    
    igraph_t mygraph = graph_create();

    struct Vertex v;
    v.id = 1;
    // v.name = "x0";
    v.ty = REG;

    graph_add_vertex(&mygraph, v);

    // print_attributes(&mygraph);

}