#include "header.h"
#include <stdlib.h> 

// Placeholder collision checking structure. Use linked list for now
// Need to implement some sort of tree structure for efficient collision checking
struct kinematic_obj_node *kinematic_HEAD = NULL;

void add_node(struct kinematic_obj *obj){
    struct kinematic_obj_node *node = malloc(sizeof(struct kinematic_obj_node));
    if (node){
        node->obj = obj;
        node->next = NULL;
    }
    if (kinematic_HEAD == NULL){
        kinematic_HEAD = node;
    }else{
        node->next = kinematic_HEAD;
        kinematic_HEAD = node;
    }
}

/**struct kinematic_obj_node **get_list(){
    return &kinematic_HEAD;
}*/

void free_list(){
    struct kinematic_obj_node *current = kinematic_HEAD;
    struct kinematic_obj_node *next;
    while(current){
        next = current->next;
        free(current);
        current = next;
    }
    kinematic_HEAD = NULL;
}