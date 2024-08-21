#include <stdio.h>
#include <stdlib.h>

#define MALLOC_SIZE 16

typedef struct Node{
    struct Node* next;
    int start_indx;
    int block_size;     //in bytes
}Node;

typedef struct TAG_TCB_MALLOC{
    char mem_pool[MALLOC_SIZE];
    Node* alloc_blocks;
}TCB_MALLOC;

TCB_MALLOC g_tcb_malloc={{0},NULL};

int return_contigous_memory_index (TCB_MALLOC* tcb_malloc,size_t bytes_req){
    Node* node = tcb_malloc->alloc_blocks;
    int start_indx =0;
    while(node){
        int bytes_available = node->start_indx -start_indx;
        if(bytes_available >= bytes_req)
            return start_indx;
        start_indx = node->start_indx + node->block_size;
        node = node->next;
    }
    if(MALLOC_SIZE - start_indx >= bytes_req)
        return start_indx;
    return -1;
}

char* release_memory_from_pool(TCB_MALLOC* tcb_malloc,size_t bytes_req,int index){
    Node* node = malloc(sizeof(Node));
    node->start_indx = index;
    node->block_size = bytes_req;
    node->next = NULL;

    Node* prev_node = NULL;
    Node* curr_node = tcb_malloc->alloc_blocks;
    if(!curr_node){
        tcb_malloc->alloc_blocks = node;
    }else{
        while(curr_node && curr_node->start_indx < node->start_indx){
            prev_node = curr_node;
            curr_node = curr_node->next;
        }
        if(!prev_node){
            node->next = tcb_malloc->alloc_blocks;
            tcb_malloc->alloc_blocks = node;
        }else{
            prev_node->next = node;
            node->next = curr_node;
        }
    }

    return &(tcb_malloc->mem_pool[index]);
}
void* mem_malloc(size_t bytes_to_reserve){
    TCB_MALLOC* tcb_malloc = &g_tcb_malloc;
    int indx_to_allot = return_contigous_memory_index(tcb_malloc,bytes_to_reserve);
    if(indx_to_allot == -1)
        return NULL;
    return (void*)release_memory_from_pool(tcb_malloc,bytes_to_reserve,indx_to_allot);
}

void mem_free(void* addr_to_free){
    TCB_MALLOC* tcb_malloc = &g_tcb_malloc;
    int index_to_free = (char*)addr_to_free - tcb_malloc->mem_pool;
    Node* prev_node = NULL;
    Node* curr_node = tcb_malloc->alloc_blocks;
    while(curr_node && curr_node->start_indx != index_to_free){
        prev_node = curr_node;
        curr_node = curr_node->next;
    }
    if(!curr_node)
        return;
    if(!prev_node){
        Node* temp = curr_node->next;
        free(curr_node);
        tcb_malloc->alloc_blocks = temp;
        return;
    }else{
        Node* temp = curr_node;
        prev_node->next = curr_node->next;
        free(temp);
    }
}
int main(){
    char* ptr0 = mem_malloc(1);
    char* ptr1 = mem_malloc(8);
    char* ptr2 = mem_malloc(7);
    char* ptr3 = mem_malloc(1);
    return 0;
}