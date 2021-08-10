#include <stdio.h>
#include <stdlib.h>
#define INFTY 4294967295

typedef struct node_struct {
    int graph_id;
    unsigned int distance;
    struct node_struct* next;
} node;

typedef struct{
    unsigned int distance;
    int graph_id;
} heap_node;

typedef struct {
    int size;
    int curr_lenght;
    int* LUT;   // KEY : GRAPH_INDEX - VALUE : HEAP_INDEX
    heap_node* heap_nodes;
} heap;


// PARSING-RELATED METHODS
void compute_integer(char* split, int number_digits, int buffer_position, unsigned int* int_buffer);
void manage_stdin(int d, unsigned int* int_buffer);
unsigned int cheat_pow(int exp);

// ADJ MATRIX RELATED METHODS
void init_adj_matrix(unsigned int** adj_matrix, int d);
void free_adj_matrix(unsigned int** adj_matrix, int d);

// DIJKSTRA RELATED METHODS
long unsigned int dijkstra(heap* h, unsigned int** adj_matrix, int d);

// HEAP RELATED METHODS
void init_min_heap(heap* h, int d);
int left(heap* h, int heap_node_index);
int right(heap* h, int heap_node_index);
int parent(int heap_node_index);
void swap(heap* h, int heap_node_index_0, int heap_node_index_1);
void swap_max(heap* h, int heap_node_index_0, int heap_node_index_1);
void min_heapify(heap* h, int heap_node_index);
void max_heapify(heap* h, int heap_node_index);
void decrease_heap_node(heap* h, int heap_node_index, unsigned int new_value);
void add_node(heap* h, unsigned int node_value, int graph_index);
void heap_test(heap* h, int d);
void free_heap(heap* h);
void top_k_insert(heap* h_topk, unsigned int graph_cost, int graph_index);

int main(){

    int d, k, graph_index;
    unsigned long int graph_cost;
    char command[15];
    heap* h = (heap*)malloc(sizeof(heap));
    heap* h_topk = (heap*)malloc(sizeof(heap));
    unsigned int** adj_matrix;
    graph_index = 0;

    if(scanf("%d %d\n", &d, &k));

    adj_matrix = (unsigned int**)malloc(sizeof(unsigned int*) * d);
    h -> heap_nodes = (heap_node*)malloc(sizeof(heap_node) * d);
    h -> LUT = (int*)malloc(sizeof(int) * d);
    h_topk -> heap_nodes = (heap_node*)malloc(sizeof(heap_node) * k);
    h_topk -> size = k;
    h_topk -> curr_lenght = 0;

    for(int i = 0; i < d; i++){
        adj_matrix[i] = (unsigned int*)malloc(sizeof(unsigned int) * d);
    }

    while(fgets(command, 15, stdin) && !feof(stdin)){
        if(command[0] == 'A'){
            init_adj_matrix(adj_matrix, d);
            graph_cost = dijkstra(h, adj_matrix, d);
            top_k_insert(h_topk, graph_cost, graph_index);
            graph_index ++;
        }
        else if(command[0] == 'T'){
            if(h_topk->curr_lenght == 0){
                printf("\n");
            }
            else{
                for(int i = 0; i< h_topk->curr_lenght; i++){
                    if(i != h_topk->curr_lenght - 1)
                        printf("%d ", h_topk->heap_nodes[i].graph_id);
                     else
                        printf("%d\n", h_topk->heap_nodes[i].graph_id);
                }
            }
        }
    }

    free_adj_matrix(adj_matrix, d);
    free_heap(h);
    free_heap(h_topk);
    return 0;
}

// ---------------------- HEAP-RELATED METHODS ------------------------

void init_min_heap(heap* h, int d){
    h -> size = d;
    h -> heap_nodes[0].distance = 0;
    h -> heap_nodes[0].graph_id = 0;
    h -> LUT[0] = 0;
    for(int i = 1; i < d; i++){
        h -> heap_nodes[i].distance = INFTY;
        h -> heap_nodes[i].graph_id = i;
        h -> LUT[i] = i;
    }
}

void free_heap(heap* h){
    free(h -> heap_nodes);
    free(h -> LUT);
    free(h);
}

int left(heap* h, int heap_node_index){
    if(h -> size >= 2 * heap_node_index + 1)
        return 2* heap_node_index + 1;
    return heap_node_index;
}

int right(heap* h, int heap_node_index){
    if(h -> size >= 2 * heap_node_index + 2)
        return 2* heap_node_index + 2;
    return heap_node_index;
}

int parent(int heap_node_index){
    if((heap_node_index - 1) / 2 >= 0)
        return (heap_node_index - 1) / 2;
    return heap_node_index;
}

void swap(heap* h, int heap_node_index_0, int heap_node_index_1){
    unsigned int tmp_heap_distance = h -> heap_nodes[heap_node_index_0].distance;
    int tmp_graph_index = h-> heap_nodes[heap_node_index_0].graph_id;
    int tmp_LUT = h -> LUT[h -> heap_nodes[heap_node_index_0].graph_id];

    h->LUT[h -> heap_nodes[heap_node_index_0].graph_id] = h->LUT[h -> heap_nodes[heap_node_index_1].graph_id];
    h->LUT[h -> heap_nodes[heap_node_index_1].graph_id] = tmp_LUT;

    h -> heap_nodes[heap_node_index_0].distance = h -> heap_nodes[heap_node_index_1].distance;
    h -> heap_nodes[heap_node_index_0].graph_id = h -> heap_nodes[heap_node_index_1].graph_id;
    h -> heap_nodes[heap_node_index_1].distance = tmp_heap_distance;
    h -> heap_nodes[heap_node_index_1].graph_id = tmp_graph_index;
}

void swap_max(heap* h, int heap_node_index_0, int heap_node_index_1){
    unsigned int tmp_heap_distance = h -> heap_nodes[heap_node_index_0].distance;
    int tmp_graph_index = h-> heap_nodes[heap_node_index_0].graph_id;

    h -> heap_nodes[heap_node_index_0].distance = h -> heap_nodes[heap_node_index_1].distance;
    h -> heap_nodes[heap_node_index_0].graph_id = h -> heap_nodes[heap_node_index_1].graph_id;
    h -> heap_nodes[heap_node_index_1].distance = tmp_heap_distance;
    h -> heap_nodes[heap_node_index_1].graph_id = tmp_graph_index;
}

void min_heapify(heap* h, int heap_node_index){
    int left_index = left(h, heap_node_index);
    int right_index = right(h, heap_node_index);
    int next_index;
    if (left_index > heap_node_index && h -> heap_nodes[left_index].distance < h -> heap_nodes[heap_node_index].distance)
        next_index = left_index;
    else
        next_index = heap_node_index;
    if (right_index > heap_node_index && h -> heap_nodes[right_index].distance < h -> heap_nodes[next_index].distance)
        next_index = right_index;
    if(next_index != heap_node_index){
        swap(h, heap_node_index, next_index);
        min_heapify(h, next_index);
    }
}

void max_heapify(heap* h, int heap_node_index){
    int left_index = left(h, heap_node_index);
    int right_index = right(h, heap_node_index);
    int next_index;
    if (left_index > heap_node_index && h -> heap_nodes[left_index].distance > h -> heap_nodes[heap_node_index].distance)
        next_index = left_index;
    else
        next_index = heap_node_index;
    if (right_index > heap_node_index && h -> heap_nodes[right_index].distance > h -> heap_nodes[next_index].distance)
        next_index = right_index;
    if(next_index != heap_node_index){
        swap_max(h, heap_node_index, next_index);
        max_heapify(h, next_index);
    }
}

void decrease_heap_node(heap* h, int heap_node_index, unsigned int new_value){
    int parent_index = parent(heap_node_index);
    int child_index = heap_node_index;
    h->heap_nodes[child_index].distance = new_value;
    if(parent_index < heap_node_index){
        while(h -> heap_nodes[parent_index].distance > new_value && parent_index < child_index){
            swap(h, parent_index, child_index);
            child_index = parent_index;
            parent_index = parent(parent_index);
        }
    }
}

int heap_not_empty(heap* h){
    if(h -> size > 0)
        return 1;
    return 0;
}

void add_node(heap* h, unsigned int node_value, int graph_index){
    int curr_index = h -> curr_lenght;
    int next_index = parent(curr_index);
    unsigned int tmp_value;
    int tmp_id;
    h -> heap_nodes[h -> curr_lenght].distance = node_value;
    h -> heap_nodes[h -> curr_lenght].graph_id = graph_index;
    while(curr_index >= 0 && next_index != curr_index && h -> heap_nodes[curr_index].distance > h -> heap_nodes[next_index].distance){
        tmp_value = h -> heap_nodes[curr_index].distance;
        tmp_id = h -> heap_nodes[curr_index].graph_id;
        h -> heap_nodes[curr_index].distance = h -> heap_nodes[next_index].distance;
        h -> heap_nodes[curr_index].graph_id = h -> heap_nodes[next_index].graph_id;
        h -> heap_nodes[next_index].distance = tmp_value;
        h -> heap_nodes[next_index].graph_id = tmp_id;
        curr_index = next_index;
        next_index = parent(next_index);
    }
    h -> curr_lenght ++;
}

heap_node remove_min(heap* h){
    heap_node min_node = h -> heap_nodes[0];
    int min_graph_index = min_node.graph_id;
    h -> LUT[min_graph_index] = -1;
    h -> LUT[h -> heap_nodes[h -> size - 1].graph_id] = 0;
    h -> heap_nodes[0].distance = h -> heap_nodes[h -> size - 1].distance;
    h -> heap_nodes[0].graph_id = h -> heap_nodes[h -> size - 1].graph_id;
    h -> size --;
    min_heapify(h, 0);
    return min_node;
}

// ------------------------- DIJKSTRA ----------------------------------

long unsigned int dijkstra(heap* h, unsigned int** adj_matrix, int d){
    heap_node removed_node;
    unsigned int cost;
    unsigned long int cumulated_cost = 0;
    init_min_heap(h, d);
    while(heap_not_empty(h)){
        removed_node = remove_min(h);
        if(removed_node.distance != INFTY){
            cumulated_cost += removed_node.distance;
            for(int neighbour_index = 0; neighbour_index < d; neighbour_index ++){
                if(adj_matrix[removed_node.graph_id][neighbour_index] != 0){
                    cost = removed_node.distance + adj_matrix[removed_node.graph_id][neighbour_index];
                    if(h->LUT[neighbour_index] >= 0 && cost < h->heap_nodes[h->LUT[neighbour_index]].distance){
                        decrease_heap_node(h, h->LUT[neighbour_index], cost);
                    }
                }
            }
        }
    }
    return cumulated_cost;
}


// ------------------------- TOP_K METHODS ----------------------------------

void top_k_insert(heap* h_topk, unsigned int graph_cost, int graph_index){
    if(h_topk -> curr_lenght  < h_topk -> size){
        add_node(h_topk, graph_cost, graph_index);
    } else {
        if(graph_cost < h_topk -> heap_nodes[0].distance){
            h_topk -> heap_nodes[0].distance = graph_cost;
            h_topk -> heap_nodes[0].graph_id = graph_index;
            max_heapify(h_topk, 0);
        }
    }
}


// -------------------------------- ADJ MATRIX ---------------------------------

void init_adj_matrix(unsigned int** adj_matrix, int d){
    long int max_dim = 10 * d;
    char char_buffer[max_dim];;
    char split[10];
    int number_digits = 0;
    int buffer_position = 0;
    int j = 0;
    for(int i = 0; i < d; i++){
        if(fgets(char_buffer, max_dim, stdin));
        while(char_buffer[j] != '\0'){
            if(char_buffer[j] == ',' || char_buffer[j] == '\n'){
                compute_integer(split, number_digits, buffer_position, adj_matrix[i]);
                buffer_position++;
                number_digits = 0;
            }
            else if (char_buffer[j] != ' '){
                split[number_digits] = char_buffer[j];
                number_digits++;
            }
            j++;
        }
        buffer_position = 0;
        number_digits = 0;
        j = 0;
    }
}

void free_adj_matrix(unsigned int** adj_matrix, int d){
    for(int i = 0; i < d; i++){
        free(adj_matrix[i]);
    }
    free(adj_matrix);
}

// ---------------------------------- PARSING ----------------------------------

void manage_stdin(int d, unsigned int* int_buffer){
    long int max_dim = 10 * d;
    char char_buffer[max_dim];;
    char split[10];
    int number_digits = 0;
    int buffer_position = 0;
    int j = 0;
    if(fgets(char_buffer, max_dim, stdin));
    while(char_buffer[j] != '\0'){
        if(char_buffer[j] == ',' || char_buffer[j] == '\n'){
            compute_integer(split, number_digits, buffer_position, int_buffer);
            buffer_position++;
            number_digits = 0;
        }
        else if (char_buffer[j] != ' '){
            split[number_digits] = char_buffer[j];
            number_digits++;
        }
        j++;
    }
}

void compute_integer(char* split, int number_digits, int buffer_position, unsigned int* int_buffer){
    unsigned int result = 0;
    for(int i = 0; i < number_digits; i++){
        result += (split[number_digits - 1 - i] - '0') * cheat_pow(i);
    }
    int_buffer[buffer_position] = result;
}

unsigned int cheat_pow(int exp){
    switch (exp)
    {
    case 0:
        return 1;

    case 1:
        return 10;

    case 2:
        return 100;

    case 3:
        return 1000;

    case 4:
        return 10000;

    case 5:
        return 100000;

    case 6:
        return 1000000;

    case 7:
        return 10000000;

    case 8:
        return 100000000;
    default:
        return 1000000000;

    }
}
