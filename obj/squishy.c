#include "header.h"

void three_point_beizerfan(Vector2 start, Vector2 mid, Vector2 end, Vector2* arr);

struct squishy_square init_squishy_square(Rectangle *rect, Color color){
    struct squishy_square sqr = {
        .rect = rect,
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
    square->topleft.x = square->rect->x;
    square->topleft.y = square->rect->y;
    square->topright.x = square->rect->x + square->rect->width;
    square->topright.y = square->rect->y;
    square->bottomleft.x = square->rect->x;
    square->bottomleft.y = square->rect->y + square->rect->height;
    square->bottomright.x = square->rect->x + square->rect->width;
    square->bottomright.y = square->rect->y + square->rect->height;

    square->top_handle.x = square->rect->x + square->rect->width / 2;
    square->top_handle.y = square->rect->y;
    square->bottom_handle.x = square->rect->x + square->rect->width / 2;
    square->bottom_handle.y = square->rect->y + square->rect->height;
    square->left_handle.x = square->rect->x;
    square->left_handle.y = square->rect->y + square->rect->height / 2;
    square->right_handle.x = square->rect->x + square->rect->width;
    square->right_handle.y = square->rect->y+ square->rect->height / 2;
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
