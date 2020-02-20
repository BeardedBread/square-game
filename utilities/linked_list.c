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
        current->next = NULL;
        free(current);
        current = next;
    }
    kinematic_HEAD = NULL;
}

void create_afterimage(struct player_obj *player){
    struct afterImage *img = malloc(sizeof(struct afterImage));
    if (img){
       for (int i=0;i<=BEZIER_POINTS;++i){
            img->top_vertices[i] = player->image->top_vertices[i];
            img->bottom_vertices[i] = player->image->bottom_vertices[i];
            img->left_vertices[i] = player->image->left_vertices[i];
            img->right_vertices[i] = player->image->right_vertices[i];
        }
        img->opacity = 1.0;
        img->prev = NULL;
        if (player->after_img_head == NULL){
            img->next = NULL;
            player->after_img_head = img;
            player->after_img_tail = img;
        }else{
            img->next = player->after_img_head;
            player->after_img_head->prev = img;
            player->after_img_head = img;
        } 
    }
}

void remove_last_afterimage(struct player_obj *player){
    struct afterImage *last;
    if (player->after_img_tail != NULL){
        if (player->after_img_tail->opacity <= 0){
            last = player->after_img_tail;
            player->after_img_tail = player->after_img_tail->prev;
            last->prev = NULL;
            free(last);
            player->after_img_tail->next = NULL;
        }
        // This happens if the last tail is the head
        if (player->after_img_tail == NULL)
            player->after_img_head == NULL;
    }
}

void free_afterimages(struct player_obj *player){
    struct afterImage *current = player->after_img_head;
    struct afterImage *next;
    while(current != NULL){
        next = current->next;
        current->next = NULL;
        current->prev = NULL;
        free(current);
        current = next;
    }
    player->after_img_head = NULL;
    player->after_img_tail = NULL;
}