#include "header.h"
#include <raymath.h>
#include <math.h>
// Target is circular, thus a kinematic obj has w=h
struct target_obj init_target(){
    return (struct target_obj){
        .radius = 12,
        .kinematic = init_kinematic_obj(12, 12)
    };
}

bool collide_target(struct kinematic_obj *obj, struct target_obj *target){
    /* The method is based off SAT
    */
    Vector2 obj_center = center(obj->rect);
    Vector2 target_center = center(target->kinematic.rect);

    float dist = Vector2Distance(obj_center, target_center);

    if (dist < target->radius)
        return true;

    double max_dim = fmax(obj->rect.width, obj->rect.height);
    if (dist > max_dim + target->radius)
        return false;

    Vector2 n = Vector2Subtract(target_center, obj_center);
    n = Vector2Normalize(n);

    Vector2 pos_check = obj->pos;
    double obj_proj1 = Vector2DotProduct(pos_check, n);
    pos_check.x += obj->rect.width;
    double obj_proj2 = Vector2DotProduct(pos_check, n);
    pos_check.y += obj->rect.height;
    double obj_proj3 = Vector2DotProduct(pos_check, n);
    pos_check.x -= obj->rect.width;
    double obj_proj4 = Vector2DotProduct(pos_check, n);

    double min_proj = fmin(fmin(fmin(obj_proj1, obj_proj2), obj_proj3), obj_proj4);
    double max_proj = fmax(fmax(fmax(obj_proj1, obj_proj2), obj_proj3), obj_proj4);
   
    double target_proj = Vector2DotProduct(target_center, n);

    if (!(max_proj < target_proj - target->radius) && !(min_proj > target_proj + target->radius))
        return true;
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