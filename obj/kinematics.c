#include "header.h"
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
    // Simplistic Collision Handling for AABB, Could add coeff of restitution?
    // TODO: Implement the slightly better method of collision from:
    // https://hopefultoad.blogspot.com/2017/09/2d-aabb-collision-detection-and-response.html

    // Also think about what happens if the square is completely inside the shape
    // Then extend to multiple object
    // Might need to check distance
    obj->velocity.x += acceleration.x * delta;
    obj->pos.x += obj->velocity.x * delta;
    obj->rect.x = obj->pos.x + obj->dim_reduction[0];
    obj->rect.width = obj->ori_width - obj->dim_reduction[0]  - obj->dim_reduction[2];
    current = kinematic_HEAD;
    while(current != NULL){
        if(current->obj != obj){
            if (CheckCollisionRecs(obj->rect, current->obj->rect)){
                collide_rect = GetCollisionRec(obj->rect, current->obj->rect);                
                if(collide_rect.width < collide_rect.height){
                    if (!place_meeting(obj, (Vector2){-collide_rect.width,0})){
                        obj->rect.x -= collide_rect.width;
                        obj->pos.x -= collide_rect.width;
                    }else{
                        obj->rect.x += collide_rect.width;
                        obj->pos.x += collide_rect.width;
                    }
                    obj->velocity.x = 0;
                }else{
                    if (!place_meeting(obj, (Vector2){0,-collide_rect.height})){
                        obj->rect.y -= collide_rect.height;
                        obj->pos.y -= collide_rect.height;
                    }else{
                        obj->rect.y += collide_rect.height;
                        obj->pos.y += collide_rect.height;
                    }
                    obj->velocity.y = 0;
                }
            }
        }
        current = current->next;
    }

    // Repeat for y
    obj->velocity.y += acceleration.y * delta;
    obj->pos.y += obj->velocity.y * delta;
    obj->rect.y = obj->pos.y + obj->dim_reduction[1];
    obj->rect.height = obj->ori_height - obj->dim_reduction[1]  - obj->dim_reduction[3];

    current = kinematic_HEAD;
    while(current != NULL){
        if(current->obj != obj){
            if (CheckCollisionRecs(obj->rect, current->obj->rect)){
                collide_rect = GetCollisionRec(obj->rect, current->obj->rect);                
                if(collide_rect.width < collide_rect.height){
                    obj->rect.x -= sign(obj->velocity.x) * collide_rect.width;
                    obj->pos.x -= sign(obj->velocity.x) * collide_rect.width;
                    obj->velocity.x = 0;
                }else{
                    obj->rect.y -= sign(obj->velocity.y) * collide_rect.height;
                    obj->pos.y -= sign(obj->velocity.y) * collide_rect.height;
                    obj->velocity.y = 0;
                }
            }
        }
        current = current->next;
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
