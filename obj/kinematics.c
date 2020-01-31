#include "header.h"
#include <math.h>
extern struct kinematic_obj_node *kinematic_HEAD;

struct kinematic_obj init_kinematic_obj(int width, int height){
    struct kinematic_obj obj = {
        .velocity = {0.0f,0.0f},
        .pos = {0.0f,0.0f},
        .rect = {0,0,width,height},
        .ori_width = width,
        .ori_height = height,
        .dim_reduction = {0,0,0,0}
    };

    return obj;
};

void set_position(struct kinematic_obj *obj, int x, int y){
    obj->pos.x = x;
    obj->rect.x = x;
    obj->pos.y = y;
    obj->rect.y = y;
};

void move(struct kinematic_obj *obj, Vector2 acceleration){
    // Use Euler method for moving
    double delta = 1.0/60.0;

    Rectangle collide_rect;
    struct kinematic_obj_node *current;
    // Self implement AABB SAT from Metanet

    // TODO: extend to multiple object collision, Might need to check distance

    // Move the object and apply hitbox reduction
    obj->velocity.x += acceleration.x * delta;
    obj->pos.x += obj->velocity.x * delta;
    obj->rect.x = obj->pos.x + obj->dim_reduction[0];
    obj->rect.width = obj->ori_width - obj->dim_reduction[0]  - obj->dim_reduction[2];
    obj->velocity.y += acceleration.y * delta;
    obj->pos.y += obj->velocity.y * delta;
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
                }
                else{
                    obj->rect.x += hmove;
                    obj->pos.x += hmove;
                }
                obj->velocity.x = 0;
            }else{
                if (obj->rect.y + obj->rect.height / 2 < current->obj->rect.y + current->obj->rect.height / 2){
                    obj->rect.y -= vmove;
                    obj->pos.y -= vmove;
                }
                else{
                    obj->rect.y += vmove;
                    obj->pos.y += vmove;
                }
                obj->velocity.y = 0;
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

Vector2 center(Rectangle rect){
    return (Vector2){
        .x = rect.x + rect.width/2, 
        .y = rect.y + rect.height/2
    };
}

void adjust_hitbox(struct kinematic_obj *obj){
    approach(&obj->dim_reduction[0], obj->set_dim_reduction[0], 0.2);
    approach(&obj->dim_reduction[1], obj->set_dim_reduction[1], 0.2);
    approach(&obj->dim_reduction[2], obj->set_dim_reduction[2], 0.2);
    approach(&obj->dim_reduction[3], obj->set_dim_reduction[3], 0.2);
}
