#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int* LUT;   // KEY : GRAPH_INDEX - VALUE : HEAP_INDEX
    heap_node* heap_nodes;
} heap;

typedef struct{
    unsigned int cost;
    int graph_index;
} graph_info;

typedef struct {
    int occupied_space;
    int max_lenght;
    graph_info* array;
} ranking;

// PARSING-RELATED METHODS
void compute_integer(char* split, int number_digits, int buffer_position, unsigned int* int_buffer);
void manage_stdin(int d, unsigned int* int_buffer);
unsigned int cheat_pow(int exp);

// ADJACENY_LIST RELATED METHODS
void adjacency_list_init(node** adjacency_list, int d);
void free_adjacency_list(node** adjacency_list, int d);
void test_adj_list(node** adjacency_list, int d);

// ADJ MATRIX RELATED METHODS
void init_adj_matrix(unsigned int** adj_matrix, int d);
void free_adj_matrix(unsigned int** adj_matrix, int d);

// DIJKSTRA RELATED METHODS
long unsigned int dijkstra(heap* h, unsigned int** adj_matrix, int d);

// HEAP RELATED METHODS
void init_heap(heap* h, int d);
int left(heap* h, int heap_node_index);
int right(heap* h, int heap_node_index);
int parent(heap* h, int heap_node_index);
void swap(heap* h, int heap_node_index_0, int heap_node_index_1);
void min_heapify(heap* h, int heap_node_index);
void decrease_heap_node(heap* h, int heap_node_index, unsigned int new_value);
void heap_test(heap* h, int d);
void free_heap(heap* h);

// TOP-K RELATED METHODS
void sort_swap(graph_info* array, int index0, int index1);
int partition(graph_info* array, int start, int end);
void quick_sort(graph_info* array, int start, int end);
void top_k_insert(ranking* top_k, unsigned int graph_cost, int graph_index);
void free_topk(ranking* top_k);

int main(){

    int d, k, graph_index;
    unsigned long int graph_cost;
    //node** adjacency_list;
    char* command = (char*)malloc(sizeof(char) * 15);
    heap* h;
    ranking* top_k = (ranking*)malloc(sizeof(ranking));
    unsigned int** adj_matrix;
    graph_index = 0;

    if(scanf("%d %d\n", &d, &k));

    top_k->array = (graph_info*)malloc(sizeof(graph_info) * k);
    top_k->max_lenght = k;
    top_k->occupied_space = 0;
    while(!feof(stdin)){

        if(scanf("%s\n", command));

        if(strcmp(command, "AggiungiGrafo") == 0){
            strcpy(command, "");

             h = (heap*)malloc(sizeof(heap));
/*            adjacency_list = (node**)malloc(sizeof(node*) * d);

            adjacency_list_init(adjacency_list, d); */
            adj_matrix = (unsigned int**)malloc(sizeof(unsigned int*) * d);
            init_adj_matrix(adj_matrix, d);
            graph_cost = dijkstra(h, adj_matrix, d);
            top_k_insert(top_k, graph_cost, graph_index);
            graph_index ++;

            free_heap(h);
            free_adj_matrix(adj_matrix, d);
/*             free_adjacency_list(adjacency_list, d); */
        }

        else if(strcmp(command, "TopK") == 0){
            strcpy(command, "");
            if(top_k->occupied_space == 0){
                printf("\n");
            }
            else{
                for(int i = 0; i< top_k->occupied_space; i++){
                    if(i != top_k->occupied_space - 1)
                        printf("%d ", top_k->array[i].graph_index);
                     else
                        printf("%d\n", top_k->array[i].graph_index);
                }
            }
        }
    }
    free_topk(top_k);
    free(command);
    return 0;
}


// ------------ ADJACENCY-RELATED METHODS -----------------

void adjacency_list_init(node** adjacency_list, int d){
    node* last_node;
    node* next_node;
    unsigned int* buffer = (unsigned int*)malloc(sizeof(unsigned int) * d);
    for(int row = 0; row < d; row++){
        last_node = (node*)malloc(sizeof(node));
        last_node->graph_id = -1;
        last_node->distance = -1;
        last_node->next = NULL;
        adjacency_list[row] = last_node;
        manage_stdin(d, buffer);
        for(int column = 0; column < d; column++){
            if(buffer[column] != 0 && column != row){
                if(last_node->graph_id == -1){
                    last_node->graph_id = column;
                    last_node->distance = buffer[column];
                    last_node->next = NULL;
                }
                else{
                    next_node = (node*)malloc(sizeof(node));
                    next_node->graph_id = column;
                    next_node->distance = buffer[column];
                    next_node->next = NULL;
                    last_node->next = next_node;
                    last_node = next_node;
                }
            }
        }
    }
    free(buffer);
}

void free_adjacency_list(node** adjacency_list, int d){
    node* curr_node;
    node* deallocated;
    for(int i = 0; i<d; i++){
        curr_node = adjacency_list[i];
        while (curr_node != NULL)
        {
            deallocated = curr_node;
            curr_node = curr_node -> next;
            free(deallocated);
        }
    }
    free(adjacency_list);
}

void test_adj_list(node** adjacency_list, int d){
    node* curr;
    for(int i = 0; i<5; i++){
        if(adjacency_list[i] -> graph_id == -1){
            printf("No neighbours\n");
        }
        else{
            curr = adjacency_list[i];
            while (curr != NULL){
                printf("[%d] -> ID : %d - DISTANCE : %u\n", i, curr -> graph_id, curr -> distance);
                curr = curr -> next;
            }
        }
    }
}


// ---------------------- HEAP-RELATED METHODS ------------------------

void init_heap(heap* h, int d){
    h -> size = d;
    h -> heap_nodes = (heap_node*)malloc(sizeof(heap_node) * d);
    h -> LUT = (int*)malloc(sizeof(int) * d);
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

int parent(heap* h, int heap_node_index){
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

void decrease_heap_node(heap* h, int heap_node_index, unsigned int new_value){
    int parent_index = parent(h, heap_node_index);
    int child_index = heap_node_index;
    h->heap_nodes[child_index].distance = new_value;
    if(parent_index < heap_node_index){
        while(h -> heap_nodes[parent_index].distance > new_value && parent_index < child_index){
            swap(h, parent_index, child_index);
            child_index = parent_index;
            parent_index = parent(h, parent_index);
        }
    }
}

int heap_not_empty(heap* h){
    if(h -> size > 0)
        return 1;
    return 0;
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

//------------------------- HEAP TEST_METHOD ----------------------------------

void heap_test(heap* h, int d){
    init_heap(h, d);
    printf("\nHEAP_INIT...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    decrease_heap_node(h, 4, 10);
    printf("\nDECREASING VALUE OF HEAP_NODE 4...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    decrease_heap_node(h, 5, 20);
    printf("\nDECREASING VALUE OF HEAP_NODE 5...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    decrease_heap_node(h, 3, 30);
    printf("\nDECREASING VALUE OF HEAP_NODE 3...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    remove_min(h);
    printf("\nREMOVING MIN...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    decrease_heap_node(h, 4, 0);
    printf("\nDECREASING VALUE OF HEAP_NODE 4...\n");
    for(int i = 0; i < d; i++){
        printf("GRAPH_INDEX [%d] -> HEAP_INDEX[%d]\n", i, h->LUT[i]);
    }

    free_heap(h);
}

// ------------------------- DIJKSTRA ----------------------------------

long unsigned int dijkstra(heap* h, unsigned int** adj_matrix, int d){
    heap_node removed_node;
//    node* neighbour;
    unsigned int cost;
    unsigned long int cumulated_cost = 0;
    init_heap(h, d);
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


void sort_swap(graph_info* array, int index0, int index1){
    unsigned int tmp_cost;
    int tmp_index;
    tmp_cost = array[index0].cost;
    tmp_index = array[index0].graph_index;
    array[index0].cost = array[index1].cost;
    array[index0].graph_index = array[index1].graph_index;
    array[index1].cost = tmp_cost;
    array[index1].graph_index = tmp_index;
}

int partition(graph_info* array, int start, int end){
    unsigned int end_value = array[end].cost;
    int swap_index = start - 1;
    for(int i = start; i < end; i ++){
        if(array[i].cost < end_value){
            swap_index ++;
            sort_swap(array, i, swap_index);
        }
    }
    sort_swap(array, swap_index + 1, end);
    return swap_index + 1;
}

void quick_sort(graph_info* array, int start, int end){
    int partition_index;
    if(start < end){
        partition_index = partition(array, start, end);
        quick_sort(array, start, partition_index - 1);
        quick_sort(array, partition_index + 1, end);
    }
}

void top_k_insert(ranking* top_k, unsigned int graph_cost, int graph_index){
    if(top_k -> occupied_space < top_k -> max_lenght){
        top_k -> array[top_k -> occupied_space].graph_index = graph_index;
        top_k -> array[top_k -> occupied_space].cost = graph_cost;
        top_k -> occupied_space = top_k -> occupied_space + 1;
        if(top_k -> occupied_space == top_k -> max_lenght)
            quick_sort(top_k -> array, 0, top_k -> max_lenght - 1);
    } else {
        if(top_k -> array[top_k -> max_lenght - 1].cost > graph_cost){
            top_k -> array[top_k -> max_lenght - 1].graph_index = graph_index;
            top_k -> array[top_k -> max_lenght - 1].cost = graph_cost;
            quick_sort(top_k -> array, 0, top_k -> max_lenght - 1);
        }
    }

}

void free_topk(ranking* top_k){
    free(top_k->array);
    free(top_k);
}

// -------------------------------- ADJ MATRIX ---------------------------------

void init_adj_matrix(unsigned int** adj_matrix, int d){
    for(int i = 0; i < d; i++){
        adj_matrix[i] = (unsigned int*)malloc(sizeof(unsigned int) * d);
        manage_stdin(d, adj_matrix[i]);
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
    char* char_buffer = malloc(sizeof(char) * max_dim);
    char split[10];
    int number_digits = 0;
    int buffer_position = 0;
    int i = 0;
    if(fgets(char_buffer, max_dim, stdin));
    while(char_buffer[i] != '\0'){
        if(char_buffer[i] == ',' || char_buffer[i] == '\n'){
            compute_integer(split, number_digits, buffer_position, int_buffer);
            buffer_position++;
            number_digits = 0;
        }
        else if (char_buffer[i] != ' '){
            split[number_digits] = char_buffer[i];
            number_digits++;
        }
        i++;
    }
    free(char_buffer);
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
