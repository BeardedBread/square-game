#include "header.h"
#include <rlgl.h>

#define INTERP_FACTOR 0.2
#define OFFSET_VALUE 20

void three_point_beizer(Vector2 start, Vector2 mid, Vector2 end, Vector2* arr);
void calc_offsets(struct squishy_square *square);

struct squishy_square init_squishy_square(struct kinematic_obj *parent, Color color){
    struct squishy_square sqr = {
        .parent = parent,
        .color = color,
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
    // This is assuming the shape remains AABB
    square->center.x = square->parent->rect.x + square->parent->rect.width/2;
    square->center.y = square->parent->rect.y + square->parent->rect.height/2;

    // Vertices are centered at zero, then transformed during drawing
    Vector2 topleft = (Vector2){-square->parent->rect.width/2, -square->parent->rect.height/2};
    Vector2 topright = (Vector2){square->parent->rect.width/2, -square->parent->rect.height/2};
    Vector2 bottomleft = (Vector2){-square->parent->rect.width/2, square->parent->rect.height/2};
    Vector2 bottomright = (Vector2){square->parent->rect.width/2, square->parent->rect.height/2};

    Vector2 top_handle = (Vector2){0, topleft.y + square->top_offset};
    Vector2 bottom_handle = (Vector2){0, bottomright.y - square->bottom_offset};
    Vector2 left_handle = (Vector2){topleft.x + square->left_offset, 0};
    Vector2 right_handle = (Vector2){topright.x - square->right_offset, 0};

    three_point_beizer(topleft, top_handle, topright, square->top_vertices);
    three_point_beizer(topright, right_handle, bottomright, square->right_vertices);
    three_point_beizer(bottomright, bottom_handle, bottomleft, square->bottom_vertices);
    three_point_beizer(bottomleft, left_handle, topleft, square->left_vertices);

}

void calc_offsets(struct squishy_square *square){
    /* 0 - left, 1 - top, 2 - right, 3 - bottom
     */
    // TODO: Normalise the offsets
    double target_offsets[4] = {0,0,0,0};
    
    if (IsKeyDown(KEY_A)){
        target_offsets[0] += OFFSET_VALUE;
        target_offsets[2] += -OFFSET_VALUE * 0.5;
    }
    if (IsKeyDown(KEY_D)){
        target_offsets[2] += OFFSET_VALUE;
        target_offsets[0] += -OFFSET_VALUE * 0.5;
    }
    if (IsKeyDown(KEY_W)){
        target_offsets[1] += OFFSET_VALUE;
        target_offsets[3] += -OFFSET_VALUE * 0.5;
    }
    if (IsKeyDown(KEY_S)){
        target_offsets[3] += OFFSET_VALUE;
        target_offsets[1] += -OFFSET_VALUE * 0.5;
    }
    bool contacts[4];
    int n_contacts = 0;
    contacts[0] = place_meeting(square->parent, (Vector2){-1, 0});
    contacts[1] = place_meeting(square->parent, (Vector2){0, -1});
    contacts[2] = place_meeting(square->parent, (Vector2){1, 0});
    contacts[3] = place_meeting(square->parent, (Vector2){0, 1});

    // Redistribute the offset on contact
    for (int i=0; i < 4; ++i){
        if (contacts[i] == true && target_offsets[i] < 0){
            unsigned int n = 0;
            unsigned int j;
            unsigned int ind;
            for (j=0; j < 3; ++j){
                ind = (i+1+j) % 4;
                if (contacts[ind] == false)
                    ++n;
            }
            if (n > 0){
                for (j=0; j < 3; ++j){
                    ind = (i+1+j) % 4;
                    if (contacts[ind] == false)
                        target_offsets[ind] += target_offsets[i] / n;
                }
            }
            target_offsets[i] = 0;
        }
    }
    approach(&square->left_offset, target_offsets[0], INTERP_FACTOR);
    approach(&square->top_offset, target_offsets[1], INTERP_FACTOR);
    approach(&square->right_offset, target_offsets[2], INTERP_FACTOR);
    approach(&square->bottom_offset, target_offsets[3], INTERP_FACTOR);
}

void draw_squishy(struct squishy_square *square){
    rlPushMatrix();
        rlTranslatef(square->center.x, square->center.y, 0.0f);
        //rlTranslatef(0.0f, square->parent->rect.height/2, 0.0f);
        //rlScalef(0.5, 0.5, 1.0);
        //rlTranslatef(0.0f, -square->parent->rect.height/2, 0.0f);
        int i;
        for(i=0;i<BEZIER_POINTS;++i){
            DrawTriangle(square->top_vertices[i], (Vector2){0,0}, square->top_vertices[i+1], square->color);
            DrawTriangle(square->bottom_vertices[i], (Vector2){0,0}, square->bottom_vertices[i+1], square->color);
            DrawTriangle(square->left_vertices[i], (Vector2){0,0}, square->left_vertices[i+1], square->color);
            DrawTriangle(square->right_vertices[i], (Vector2){0,0}, square->right_vertices[i+1],square->color);
        }
    rlPopMatrix();
}

void three_point_beizer(Vector2 start, Vector2 mid, Vector2 end, Vector2* arr){
    /* Generate the vertices for a beizer curve
     */
    double t;
    double t_prime;
    for (int i=0;i<=BEZIER_POINTS;++i){
        t = i*1.0/BEZIER_POINTS;
        t_prime = 1-t;
        arr[i].x = t_prime*t_prime*start.x + 2*t_prime*t*mid.x + t*t*end.x;
        arr[i].y = t_prime*t_prime*start.y + 2*t_prime*t*mid.y + t*t*end.y;
    }
}
