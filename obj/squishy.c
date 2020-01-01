#include "header.h"

#define INTERP_FACTOR 0.5
#define OFFSET_VALUE 20

void three_point_beizerfan(Vector2 start, Vector2 mid, Vector2 end, Vector2* arr);
void calc_offsets(struct squishy_square *square);

struct squishy_square init_squishy_square(struct kinematic_obj *parent, Color color){
    struct squishy_square sqr = {
        .parent = parent,
        .color = color,
        .topleft = (Vector2){0,0},
        .topright= (Vector2){0,0},
        .bottomleft = (Vector2){0,0},
        .bottomright = (Vector2){0,0},
        .top_handle = (Vector2){0,0},
        .bottom_handle = (Vector2){0,0},
        .left_handle = (Vector2){0,0},
        .right_handle = (Vector2){0,0},
        .top_offset = 0.0,
        .bottom_offset = 0.0,
        .left_offset = 0.0,
        .right_offset = 0.0
    };
    return sqr;
}

void update_squishy(struct squishy_square *square){

    calc_offsets(square);

    // Update to follow the player
    square->topleft.x = square->parent->rect.x;
    square->topleft.y = square->parent->rect.y;
    square->topright.x = square->parent->rect.x + square->parent->rect.width;
    square->topright.y = square->parent->rect.y;
    square->bottomleft.x = square->parent->rect.x;
    square->bottomleft.y = square->parent->rect.y + square->parent->rect.height;
    square->bottomright.x = square->parent->rect.x + square->parent->rect.width;
    square->bottomright.y = square->parent->rect.y + square->parent->rect.height;

    square->top_handle.x = square->parent->rect.x + square->parent->rect.width / 2;
    square->top_handle.y = square->parent->rect.y + square->top_offset; 
    square->bottom_handle.x = square->parent->rect.x + square->parent->rect.width / 2;
    square->bottom_handle.y = square->parent->rect.y + square->parent->rect.height - square->bottom_offset;
    square->left_handle.x = square->parent->rect.x + square->left_offset;
    square->left_handle.y = square->parent->rect.y + square->parent->rect.height / 2;
    square->right_handle.x = square->parent->rect.x + square->parent->rect.width - square->right_offset;
    square->right_handle.y = square->parent->rect.y + square->parent->rect.height / 2;

}

void calc_offsets(struct squishy_square *square){
    // TODO: Normalise the offsets
    double left_target_offset = 0;
    double right_target_offset = 0;
    double top_target_offset = 0;
    double bottom_target_offset = 0;
    
    if (IsKeyDown(KEY_A)){
        left_target_offset = OFFSET_VALUE;
        if (place_meeting(square->parent, (Vector2){1, 0})){
            top_target_offset = -OFFSET_VALUE / 2;
            bottom_target_offset = -OFFSET_VALUE / 2;
        }else{
            right_target_offset = -OFFSET_VALUE * 0.8;
        }
    }
    if (IsKeyDown(KEY_D)){
        right_target_offset = OFFSET_VALUE;
        if (place_meeting(square->parent, (Vector2){-1, 0})){
            top_target_offset = -OFFSET_VALUE / 2;
            bottom_target_offset = -OFFSET_VALUE / 2;
        }else{
            left_target_offset = -OFFSET_VALUE * 0.8;
        }
    }
    if (IsKeyDown(KEY_W)){
        top_target_offset = OFFSET_VALUE;
        if (place_meeting(square->parent, (Vector2){0, 1})){
            right_target_offset = -OFFSET_VALUE / 2;
            left_target_offset = -OFFSET_VALUE / 2;
        }else{
            bottom_target_offset = -OFFSET_VALUE * 0.8;
        }
    }
    if (IsKeyDown(KEY_S)){
        bottom_target_offset = OFFSET_VALUE;
        if (place_meeting(square->parent, (Vector2){0, -1})){
            right_target_offset = -OFFSET_VALUE / 2;
            left_target_offset = -OFFSET_VALUE / 2;
        }else{
            top_target_offset = -OFFSET_VALUE * 0.8;
        }
    }

    approach(&square->left_offset, left_target_offset, INTERP_FACTOR);
    approach(&square->right_offset, right_target_offset, INTERP_FACTOR);
    approach(&square->top_offset, top_target_offset, INTERP_FACTOR);
    approach(&square->bottom_offset, bottom_target_offset, INTERP_FACTOR);
}

void draw_squishy(struct squishy_square *square){
    Vector2 center = (Vector2){
        .x = (square->topleft.x + square->topright.x)/2,
        .y = (square->topleft.y + square->bottomleft.y)/2
    };
    Vector2 vertices[BEZIER_POINTS+1];
    int i;
    three_point_beizerfan(square->topleft, square->top_handle, square->topright, vertices);
    for(i=0;i<BEZIER_POINTS;++i)
        DrawTriangle(vertices[i], center,vertices[i+1], square->color);
    three_point_beizerfan(square->topright, square->right_handle, square->bottomright, vertices);
    for(i=0;i<BEZIER_POINTS;++i)
        DrawTriangle(vertices[i], center,vertices[i+1], square->color);
    three_point_beizerfan(square->bottomright, square->bottom_handle, square->bottomleft, vertices);
    for(i=0;i<BEZIER_POINTS;++i)
        DrawTriangle(vertices[i], center,vertices[i+1], square->color);
    three_point_beizerfan(square->bottomleft, square->left_handle, square->topleft, vertices);
    for(i=0;i<BEZIER_POINTS;++i)
        DrawTriangle(vertices[i], center,vertices[i+1],square->color);
}

void three_point_beizerfan(Vector2 start, Vector2 mid, Vector2 end, Vector2* arr){
    double t;
    double t_prime;
    for (int i=0;i<=BEZIER_POINTS;++i){
        t = i*1.0/BEZIER_POINTS;
        t_prime = 1-t;
        arr[i].x = t_prime*t_prime*start.x + 2*t_prime*t*mid.x + t*t*end.x;
        arr[i].y = t_prime*t_prime*start.y + 2*t_prime*t*mid.y + t*t*end.y;
    }
}
