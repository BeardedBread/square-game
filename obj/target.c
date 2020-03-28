#include "header.h"
#include <raymath.h>
#include <math.h>

#define GRID_SIZE 32
// Target is circular, thus a kinematic obj has w=h
struct target_obj init_target(){
    return (struct target_obj){
        .radius = 12,
        .kinematic = init_kinematic_obj(12, 12),
        .destroyed = false
    };
}

// SUGGESTION: Implement a grid collision system for target
// This is okay since target is always constant size
// Partition the screen into apropriate grid sizes
// Check which grid requires check (i.e. which grid is the player overlapping)
// For each overlapped grid, check for collision with target in each (if any)
// If the target moved, the grid must be updated
// Min grid size 2*Player size, due to the shearing

// The grid will be an N*N array of linked list
// Please remember to go through each element,
// and free the linked list

// Removal of target
// It is possible for a target to occupy more than a grid
// Thus, need to check for that
// Then, go through those grids to mark them destroyed
// Only free them if the game ends 
// This will allow easy restart, no need to recreate objects

// The construction of the grid will take in a linked list all targets.
// So no need to modify the existing linked list
// However, it might be possible to make it an array
bool collide_target(struct kinematic_obj *obj, struct target_obj *target){
    /* The method is based off SAT
    */
    if (target->destroyed)
        return false;
  
    Vector2 obj_center = center(obj->rect);
    Vector2 target_center = center(target->kinematic.rect);

    float dist = Vector2Distance(obj_center, target_center);

    if (dist < target->radius){
        target->destroyed = true;
        return true;
    }

    double max_dim = fmax(obj->rect.width, obj->rect.height);
    if (dist > max_dim + target->radius)
        return false;

    Vector2 n = Vector2Subtract(target_center, obj_center);
    n = Vector2Normalize(n);

    Rectangle hitbox = obj->rect;
    if (obj->x_shear > 0.5){
        if (obj->x_shear > 0){
            hitbox.x -= obj->x_shear * obj->ori_width / 2;
            hitbox.width += obj->x_shear * obj->ori_width;
        }else{
            hitbox.x -= -obj->x_shear * obj->ori_width / 2;
            hitbox.width += -obj->x_shear * obj->ori_width;
        }
    }
    Vector2 pos_check = (Vector2){hitbox.x, hitbox.y};
    double obj_proj1 = Vector2DotProduct(pos_check, n);
    pos_check.x += hitbox.width;
    double obj_proj2 = Vector2DotProduct(pos_check, n);
    pos_check.y += hitbox.height;
    double obj_proj3 = Vector2DotProduct(pos_check, n);
    pos_check.x -= hitbox.width;
    double obj_proj4 = Vector2DotProduct(pos_check, n);

    double min_proj = fmin(fmin(fmin(obj_proj1, obj_proj2), obj_proj3), obj_proj4);
    double max_proj = fmax(fmax(fmax(obj_proj1, obj_proj2), obj_proj3), obj_proj4);
   
    double target_proj = Vector2DotProduct(target_center, n);

    if (!(max_proj < target_proj - target->radius) && !(min_proj > target_proj + target->radius)){
        target->destroyed = true;
        return true;
    }
    return false;
}

void add_target_node(struct target_obj *obj, struct target_obj_node **HEAD){
    struct target_obj_node *node = malloc(sizeof(struct target_obj_node));
    if (node){
        node->obj = obj;
        node->next = NULL;
    }
    if (*HEAD != NULL)
        node->next = *HEAD;
    
    *HEAD = node;
}

void free_target_list(struct target_obj_node **HEAD){
    struct target_obj_node *current = *HEAD;
    struct target_obj_node *next;
    while(current){
        next = current->next;
        current->next = NULL;
        free(current);
        current = next;
    }
    *HEAD = NULL;
}