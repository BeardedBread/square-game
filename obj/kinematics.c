#include "header.h"
extern struct kinematic_obj_node *kinematic_HEAD;

struct kinematic_obj init_kinematic_obj(int width, int height){
    struct kinematic_obj obj = {
        .velocity = {0.0f,0.0f},
        .rect = {0,0,width,height}
    };

    return obj;
};

void set_position(struct kinematic_obj *obj, int x, int y){
    obj->rect.x = x;
    obj->rect.y = y;
};

void move(struct kinematic_obj *obj, Vector2 acceleration){
    // Use Euler method for moving
    double delta = 1.0/60.0;

    Rectangle collide_rect;
    struct kinematic_obj_node *current;
    //Simplistic Collision Handling for AABB, Could add coeff of restitution?
    obj->velocity.x += acceleration.x * delta;
    obj->rect.x += obj->velocity.x * delta;
    current = kinematic_HEAD;
    while(current != NULL){
        if(current->obj != obj){
            if (CheckCollisionRecs(obj->rect, current->obj->rect)){
                collide_rect = GetCollisionRec(obj->rect, current->obj->rect);                
                if(collide_rect.width < collide_rect.height){
                    obj->rect.x -= sign(obj->velocity.x) * collide_rect.width;
                    obj->velocity.x = 0;
                }else{
                    obj->rect.y -= sign(obj->velocity.y) * collide_rect.height;
                    obj->velocity.y = 0;
                }
            }
        }
        current = current->next;
    }

    // Repeat for y
    obj->velocity.y += acceleration.y * delta;
    obj->rect.y += obj->velocity.y * delta;
    current = kinematic_HEAD;
    while(current != NULL){
        if(current->obj != obj){
            if (CheckCollisionRecs(obj->rect, current->obj->rect)){
                collide_rect = GetCollisionRec(obj->rect, current->obj->rect);                
                if(collide_rect.width < collide_rect.height){
                    obj->rect.x -= sign(obj->velocity.x) * collide_rect.width;
                    obj->velocity.x = 0;
                }else{
                    obj->rect.y -= sign(obj->velocity.y) * collide_rect.height;
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
