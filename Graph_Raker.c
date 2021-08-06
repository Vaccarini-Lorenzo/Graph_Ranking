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

unsigned int* matrix_computation(int d);
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
void dijkstra(int d, unsigned int* adjacence_matrix);

int main(){
    unsigned int d, k;   // d: # nodi, k: Dimensione list TopK
    char* command = (char*)malloc(sizeof(char)*14);
    unsigned int* adjacence_matrix;
    // Receiving d and k
    scanf("%d,%d", &d, &k);

    while (!feof(stdin)) {
        scanf("%s", command);
        if(strcmp(command, "AggiungiGrafo") == 0){
            // O(N^2)
            adjacence_matrix = matrix_computation(d);
            dijkstra(d, adjacence_matrix);
        }
    }
    return 0;
}

// FIX THIS -> LINKED LIST
unsigned int* matrix_computation(int d){
    unsigned int* adjacence_matrix = (unsigned int*)malloc(sizeof(unsigned int) * d * d );
    int buffer;
    for(int row = 0; row < d; row++){
        for(int column = 0; column < d; column++){
            if(column != d - 1)
                scanf("%d,", &buffer);
            else
                scanf("%d", &buffer);
            if(column != row)
                adjacence_matrix[row * d + column] = buffer;
            else
                adjacence_matrix[row * d + column] = 0;
        }
    }
    return adjacence_matrix;
}

binary_heap quick_init_heap(int d){
    binary_heap heap;
    heap.vertices = (vertex*)malloc(sizeof(vertex) * heap.size);
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
void swap(binary_heap* heap, int index_0, int index_1){
    printf("index_0 = %d, index_1 = %d\n", index_0, index_1);
    printf("LUT status BEFORE SWAP:\n");
    for(int i = 0; i < 4; i++){
        printf("[%d] = %d\n", i, heap->graph_LUT[i]);
    }
    unsigned int tmp_dist = heap->vertices[index_0].distance;
    int tmp_id = heap->vertices[index_0].id;
    int heap_index = heap->graph_LUT[index_1];
    vertex v = heap->vertices[heap_index];
    // LUT update
    heap->graph_LUT[index_0] =  v.id;
    heap->graph_LUT[index_1] = tmp_id;
    printf("LUT status AFTER swap:\n");
    for(int i = 0; i < 4; i++){
        printf("[%d] = %d\n", i, heap->graph_LUT[i]);
    }
     // Vertex update
    heap->vertices[index_0].distance = heap->vertices[index_1].distance;
    heap->vertices[index_0].id = heap->vertices[index_1].id;
    heap->vertices[index_1].distance = tmp_dist;
    heap->vertices[index_1].id = tmp_id;
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
    int tmp_index = heap->graph_LUT[heap->lenght - 1];
    heap->lenght--;
    heap->size--;
    heap->graph_LUT[heap->lenght] = heap->graph_LUT[0];
    heap->graph_LUT[0] = tmp_index;
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

// Maybe adjacence_matrix is not a good idea -- Need to fix this
void dijkstra(int d, unsigned int* adjacence_matrix){
    binary_heap heap = quick_init_heap(d);
    int* paths_cost = (int*)malloc(sizeof(int) * (d+1));
    int cost;
    int node;
    int neighbour_index;
    vertex* neighbour_v;
    while(!is_empty(&heap)){
        vertex v = remove_min(&heap);
        node = v.id;
        paths_cost[node] = v.distance;
        for(int i = 0; i < d; i++){
            if(adjacence_matrix[node * d + i] != 0){
                cost = adjacence_matrix[node * d + i];
                neighbour_index = heap.graph_LUT[i];
                // What happens here?
                neighbour_v = &heap.vertices[neighbour_index];
                if(neighbour_v->id == i && neighbour_v->distance > (cost + v.distance)){
                    decrease_distance(&heap, i, cost + v.distance);
                }

            }
        }
    }
    for(int i = 0; i < d; i++){
        printf("[%d] = %d\n", i, paths_cost[i]);
    }

}
