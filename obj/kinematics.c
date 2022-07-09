#include "header.h"
#include <raymath.h>
extern struct kinematic_obj_node *kinematic_HEAD;

struct kinematic_obj init_kinematic_obj(int width, int height){
    struct kinematic_obj obj = {
        .velocity = {0.0f,0.0f},
        .pos = {0.0f,0.0f},
        .rect = {0,0,width,height},
        .ori_width = width,
        .ori_height = height,
        .dim_reduction = {0,0,0,0},
        .x_shear = 0.0
    };

    return obj;
};

void set_position(struct kinematic_obj *obj, int x, int y){
    obj->pos.x = x;
    obj->rect.x = x;
    obj->pos.y = y;
    obj->rect.y = y;
};

void set_velocity(struct kinematic_obj *obj, int velx, int vely){
    obj->velocity.x = velx;
    obj->velocity.y = vely;
};

void move(struct kinematic_obj *obj, Vector2 acceleration){
    // Use Euler method for moving
    double delta = 1.0/60.0;

    Rectangle collide_rect;
    struct kinematic_obj_node *current;
    // Self implement AABB SAT from Metanet

    // TODO: extend to multiple object collision, Might need to check distance

    // Move the object and apply hitbox reduction
    obj->velocity = Vector2Add(obj->velocity, Vector2Scale(acceleration, delta));
    obj->pos = Vector2Add(obj->pos, Vector2Scale(obj->velocity, delta));

    obj->rect.x = obj->pos.x + obj->dim_reduction[0];
    obj->rect.width = obj->ori_width - obj->dim_reduction[0]  - obj->dim_reduction[2];
    obj->rect.y = obj->pos.y + obj->dim_reduction[1];
    obj->rect.height = obj->ori_height - obj->dim_reduction[1]  - obj->dim_reduction[3];
    
    current = kinematic_HEAD;
    while(current != NULL){
        if(current->obj != obj){
            // SAT method - If any projected axis is non overlapping, exit
            if (obj->rect.x + obj->rect.width < current->obj->rect.x) goto iter;
            if (current->obj->rect.x + current->obj->rect.width < obj->rect.x) goto iter;
            if (obj->rect.y + obj->rect.height < current->obj->rect.y) goto iter;
            if (current->obj->rect.y + current->obj->rect.height < obj->rect.y) goto iter;
            
            // Move in the direction of the shorter movement
            double hmove, vmove;
            hmove = (current->obj->rect.width + obj->rect.width) / 2 - fabs(current->obj->rect.x - obj->rect.x + (current->obj->rect.width - obj->rect.width) / 2);
            vmove = (current->obj->rect.height + obj->rect.height) / 2 - fabs(current->obj->rect.y - obj->rect.y + (current->obj->rect.height - obj->rect.height) / 2);
            if (hmove < vmove){
                if (obj->rect.x + obj->rect.width / 2 < current->obj->rect.x + current->obj->rect.width / 2){
                    obj->rect.x -= hmove;
                    obj->pos.x -= hmove;
                    if (obj->velocity.x > 0)
                        obj->velocity.x = 0;
                }
                else{
                    obj->rect.x += hmove;
                    obj->pos.x += hmove;
                    if (obj->velocity.x < 0)
                        obj->velocity.x = 0;
                }
            }else{
                if (obj->rect.y + obj->rect.height / 2 < current->obj->rect.y + current->obj->rect.height / 2){
                    obj->rect.y -= vmove;
                    obj->pos.y -= vmove;
                    if (obj->velocity.y > 0)
                        obj->velocity.y = 0;
                }
                else{
                    obj->rect.y += vmove;
                    obj->pos.y += vmove;
                    if (obj->velocity.y < 0)
                        obj->velocity.y = 0;
                }
            }
        }
iter:        current = current->next;
    }
};

bool place_meeting(struct kinematic_obj *obj, Vector2 dir){
    struct kinematic_obj_node *current = kinematic_HEAD;
    Rectangle rect_check = obj->rect;
    rect_check.x += dir.x;
    rect_check.y += dir.y;
    Rectangle collide_rect;
    while(current != NULL){
        if(current->obj != obj){
            collide_rect = GetCollisionRec(rect_check, current->obj->rect);
            if (collide_rect.x > 0 || collide_rect.y > 0)
                return true;
        }
        current = current->next;
    }
    return false;
}

void adjust_hitbox(struct kinematic_obj *obj){
    approach(&obj->dim_reduction[0], obj->set_dim_reduction[0], 0.2);
    approach(&obj->dim_reduction[1], obj->set_dim_reduction[1], 0.2);
    approach(&obj->dim_reduction[2], obj->set_dim_reduction[2], 0.2);
    approach(&obj->dim_reduction[3], obj->set_dim_reduction[3], 0.2);
}

// Placeholder collision checking structure. Use linked list for now
// Need to implement some sort of tree structure for efficient collision checking

void add_kinematic_node(struct kinematic_obj *obj, struct kinematic_obj_node **HEAD){
    struct kinematic_obj_node *node = malloc(sizeof(struct kinematic_obj_node));
    if (node){
        node->obj = obj;
        node->next = NULL;
    }
    if (*HEAD != NULL)
        node->next = *HEAD;
    
    *HEAD = node;
}

/**struct kinematic_obj_node **get_list(){
    return &kinematic_HEAD;
}*/

void free_kinematic_list(struct kinematic_obj_node **HEAD){
    struct kinematic_obj_node *current = *HEAD;
    struct kinematic_obj_node *next;
    while(current){
        next = current->next;
        current->next = NULL;
        free(current);
        current = next;
    }
    *HEAD = NULL;
}