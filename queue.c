#include "queue.h"

queue* queue_init(queue *q){
    //call with queue_init(&queue);
    queue *p = malloc(sizeof(queue));
    p->front = NULL;
    p->back = NULL;
    return p;
    //*q = p;
}

int is_empty(queue *my_queue){
    fprintf(stderr, "in empty\n");
    if(my_queue->front == NULL && my_queue->front == NULL) return 1;
    return 0;
}

void* push_back(char* data, queue *my_queue){
    node *new_node = malloc(sizeof(node));
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->data = strdup(data);
    //lock
    if(is_empty(my_queue)){
        my_queue->front = new_node;
        my_queue->back = new_node;
    }else{
        my_queue->back->next = new_node;
        new_node->prev = my_queue->back;
        my_queue->back = new_node;
    }
    //unlock 
    fprintf(stderr, "at end of push_back\n");
}

node* pop_front(queue *my_queue){
    //lock
    //wait for it to be nonempty
    if(is_empty(my_queue)){
        return 0;
    }else{
        node *temp = my_queue->front;
        fprintf(stderr, "set temp\n");
        my_queue->front = temp->next;
        if(my_queue->front != NULL){
            fprintf(stderr, "set front to nex\n");
            my_queue->front->prev = NULL;
            fprintf(stderr, "set front prev to null\n");
        }
        return temp;
    }
    fprintf(stderr, "done with pop\n");
    //unlock
}

node* get_front(queue *my_queue){
    //lock
    node *temp = malloc(sizeof(node));
    temp = my_queue->front;
    return temp;
    //unlock
}

node* get_back(queue *my_queue){
    //lock
    node *temp = malloc(sizeof(node));
    temp = my_queue->back;
    return temp;
    //unlock
}

void* delete_queue(queue *my_queue){
    while(!is_empty(my_queue)){
       pop_front(my_queue); 
    }
    //need to free my_queue in main
}
void* check_queue(queue *my_queue){
    node *temp = my_queue->front;
    while(temp){
        fprintf(stderr, "Data %s\n", temp->data);
        
        temp = temp->next;
    }
    fprintf(stderr, "done with checkqueue\n");
}
