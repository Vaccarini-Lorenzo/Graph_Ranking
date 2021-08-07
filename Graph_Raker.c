
//  PROGETTO API 2020/2021
//  GRAPH-RANKER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // DEBUGGING
#define INFTY 4294967295;

typedef struct{
    unsigned int distance;
    int id;
} vertex;

typedef struct {
    vertex* vertices;
    unsigned int size;
    unsigned int lenght;
    // graph_node -> heap_node
    int* graph_LUT;
} binary_heap;

typedef struct node_struct {
    int id;
    unsigned int distance;
    struct node_struct* next;
} node;

node** adjacence_list_init(int d);
binary_heap quick_init_heap(int d); // inits a heap with min 0 and other -1 (infty);   O(N)
int parent(int node_index);
int left(binary_heap* heap, int vertex);
int right(binary_heap* heap, int vertex);
void swap(binary_heap* heap, int index_0, int index_1);
void min_heapify(binary_heap* heap, int node_index);
vertex remove_min(binary_heap* heap);
void update_heap(binary_heap* heap, int node_index);
void decrease_distance(binary_heap* heap, int id, unsigned int lower_distance);
int is_empty(binary_heap* heap);
void dijkstra(int d, node** adjacence_matrix);

int main(){
    unsigned int d, k;   // d: # nodi, k: Dimensione list TopK
    char* command = (char*)malloc(sizeof(char)*14);
    node** adjacence_list;
    // Receiving d and k
    if(scanf("%d,%d", &d, &k));

    while (!feof(stdin)) {
        if(scanf("%s", command));
        if(strcmp(command, "AggiungiGrafo") == 0){
            strcpy(command, "");
            // O(N^2)
            adjacence_list = adjacence_list_init(d);
            dijkstra(d, adjacence_list);
        }
    }
    return 0;
}

node** adjacence_list_init(int d){
    node** adjacence_list = (node**)malloc(sizeof(node*) * (d + 1));
    node** pointer = (node**)malloc(sizeof(node*));
    node* next_node = (node*)malloc(sizeof(node));
    node* node_maker;
    unsigned int buffer;
    for(int row = 0; row < d; row++){
        adjacence_list[row] = (node*)malloc(sizeof(node));
        pointer = &adjacence_list[row];
        (*pointer) -> id = -1;
        for(int column = 0; column < d; column++){
            if(column != d - 1){
                if(scanf("%d,", &buffer));
            }
            else
                if(scanf("%d", &buffer));
            if(buffer != 0 && column != row){
                if((*pointer) -> id != -1){
                    node_maker = (node*)malloc(sizeof(node));
                    (*pointer) -> next = node_maker;
                    next_node = node_maker;
                    pointer = &next_node;
                }
                (*pointer) -> id = column;
                (*pointer) -> distance = buffer;
                (*pointer) -> next = NULL;
            }
        }
    }
    return adjacence_list;
}

binary_heap quick_init_heap(int d){
    binary_heap heap;
    heap.vertices = (vertex*)malloc(sizeof(vertex) * (d + 1));
    heap.graph_LUT = (int*)malloc(sizeof(int) * (d + 1));

    heap.lenght = d;
    heap.size = d + 1;
    heap.vertices[0].distance = 0;
    heap.vertices[0].id = 0;
    heap.graph_LUT[0] = 0;

    for(unsigned int i = 1; i  < d; i++){
        heap.vertices[i].distance = INFTY;
        heap.vertices[i].id = i;
        heap.graph_LUT[i] = i;
    }
    return heap;
}

int parent(int node_index){
    if((node_index - 1) / 2 >= 0)
        return (node_index - 1) / 2;
    else
        return node_index;
}

// We take the node_index (array of distances, see vertex structure) and ask for the left child node_index
// e.g: node_index = 0 => The first element of the vertices list (vertices[node_index] -> node of the heap).
int left(binary_heap* heap, int node_index){
    if(heap->lenght >= 2 * node_index + 1)
        return (2 * node_index) + 1;
    else
        return node_index;
}

// We take the node_index (index from the array of distances, see vertex structure) and ask for the right child node_index
int right(binary_heap* heap, int node_index){
    if(heap->lenght >= 2 * node_index + 2)
        return (2 * node_index) + 2;
    else
        return node_index;
}

// index_0: a node_index (index from the array of distances, see vertex structure)
void swap(binary_heap* heap, int heap_index_0, int heap_index_1){
    vertex vertex_0 = heap->vertices[heap_index_0];
    vertex vertex_1 = heap->vertices[heap_index_1];
    int graph_index_0 = vertex_0.id;
    int graph_index_1 = vertex_1.id;
    unsigned int tmp_dist = vertex_0.distance;
    // LUT update
    heap-> graph_LUT[graph_index_0] = heap_index_1;
    heap-> graph_LUT[graph_index_1] = heap_index_0;
    // Vertex update
    heap->vertices[heap_index_0].distance = heap->vertices[heap_index_1].distance;
    heap->vertices[heap_index_0].id = heap->vertices[heap_index_1].id;
    heap->vertices[heap_index_1].distance = tmp_dist;
    heap->vertices[heap_index_1].id = graph_index_0;
}

// Min_heapify: from the node_index (index from the array of distances) recursive check of the tree property
void min_heapify(binary_heap* heap, int node_index){
    int l = left(heap, node_index);
    int r = right(heap, node_index);

    int next_check;
    if(l > node_index && heap->vertices[l].distance < heap->vertices[node_index].distance)
        next_check = l;
    else
        next_check = node_index;
    if (r > node_index && heap->vertices[r].distance < heap->vertices[next_check].distance)
        next_check = r;
    if (next_check != node_index){
        swap(heap, next_check, node_index);
        min_heapify(heap, next_check);
    }
}

vertex remove_min(binary_heap* heap){
    vertex min = heap->vertices[0];
    int graph_index = heap->vertices[0].id;
    heap->graph_LUT[graph_index] = -1;
    heap->lenght--;
    heap->size--;
    heap->vertices[0] = heap->vertices[heap->lenght];
    min_heapify(heap, 0);
    return min;
}

void update_heap(binary_heap* heap, int node_index){
    int check = parent(node_index);
    while(check < node_index && heap->vertices[node_index].distance < heap->vertices[check].distance){
        swap(heap, node_index, check);
        check = parent(check);
    }

}

void decrease_distance(binary_heap* heap, int id, unsigned int lower_distance){
    int heap_index = heap->graph_LUT[id];
    vertex* v = &heap->vertices[heap_index];
    // THIS HAS TO BE OPTIMIZED
    /*
    for(int i = 0; i < heap->lenght; i++){
        if(heap->vertices[i].id == id){
            v = &heap->vertices[i];
            v->distance = lower_distance;
            update_heap(heap, i);
        }
    }
    */
    v->distance = lower_distance;
    update_heap(heap, heap_index);
}

int is_empty(binary_heap* heap){
    if(heap->lenght == 0)
        return 1;
    return 0;
}

void free_heap(binary_heap* heap){
    free(heap->vertices);
    free(heap->graph_LUT);
}

// Maybe adjacence_matrix is not a good idea -- Need to fix this
void dijkstra(int d, node** adjacence_list){
    binary_heap heap = quick_init_heap(d);
    unsigned int* paths_cost = (unsigned int*)malloc(sizeof(unsigned int) * (d+1));
    for(int i = 0; i < d; i++){
        paths_cost[i] = -1;
    }
    int cost;
    int id;
    int neighbour_heap_index;
    node* neighbour;
    vertex* neighbour_heap_vertex;
    while(!is_empty(&heap)){
        vertex v = remove_min(&heap);
        id = v.id;
        paths_cost[id] = v.distance;
        neighbour = adjacence_list[id];
        while(neighbour != NULL){
            if(neighbour -> id != -1){
                cost = neighbour -> distance;
                neighbour_heap_index = heap.graph_LUT[neighbour -> id];
                neighbour_heap_vertex = &heap.vertices[neighbour_heap_index];
                if(neighbour_heap_index != -1){
                    if(neighbour_heap_vertex->distance > (cost + v.distance)){
                        decrease_distance(&heap, neighbour -> id, cost + v.distance);
                    }
                }
                neighbour = neighbour -> next;
            }
            else{
                neighbour = NULL;
            }
        }
    }
    for(int i = 0; i < d; i++){
        printf("[%d] = %d\n", i, paths_cost[i]);
    }
    free_heap(&heap);
}
