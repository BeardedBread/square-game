#include "header.h"
#include <rlgl.h>
#include <math.h>

#define INTERP_FACTOR 0.2
#define OFFSET_VALUE 20
#define SHEAR_NORMALISE 100
float shear_mat[16] =  {1.0, 0.0, 0, 0,
                        0.1, 1.0, 0, 0,
                        0, 0, 1.0, 0,
                        0, 0, 0, 1.0};

float translate_mat[16] =  {1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1};

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

void set_squish_target_offset(struct squishy_square *square, unsigned int dir, int val){
    /* 0 - left, 1 - top, 2 - right, 3 - bottom
     */
    square->target_offsets[0] = 0;
    square->target_offsets[1] = 0;
    square->target_offsets[2] = 0;
    square->target_offsets[3] = 0;
    switch(dir){
        case 0:
            square->target_offsets[0] += val;
            square->target_offsets[2] += -val * 0.5;
        break;
        case 1:
            square->target_offsets[1] += val;
            square->target_offsets[3] += -val * 0.5;
        break;
        case 2:
            square->target_offsets[2] += val;
            square->target_offsets[0] += -val * 0.5;
        break;
        case 3:
            square->target_offsets[3] += val;
            square->target_offsets[1] += -val * 0.5;
        break;
    }
}

void calc_offsets(struct squishy_square *square){
    // TODO: Normalise the offsets
    
    bool contacts[4];
    int n_contacts = 0;
    contacts[0] = place_meeting(square->parent, (Vector2){-1, 0});
    contacts[1] = place_meeting(square->parent, (Vector2){0, -1});
    contacts[2] = place_meeting(square->parent, (Vector2){1, 0});
    contacts[3] = place_meeting(square->parent, (Vector2){0, 1});

    // Redistribute the offset on contact
    for (int i=0; i < 4; ++i){
        if (contacts[i] == true && square->target_offsets[i] < 0){
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
                        square->target_offsets[ind] += square->target_offsets[i] / n;
                }
            }
            square->target_offsets[i] = 0;
        }
    }
    approach(&square->left_offset, square->target_offsets[0], INTERP_FACTOR);
    approach(&square->top_offset, square->target_offsets[1], INTERP_FACTOR);
    approach(&square->right_offset, square->target_offsets[2], INTERP_FACTOR);
    approach(&square->bottom_offset, square->target_offsets[3], INTERP_FACTOR);
}

void draw_squishy(struct squishy_square *square){
    rlPushMatrix();
        // TODO: Need a correction term to put the square in the box????
        shear_mat[4] = -square->parent->velocity.x / 600;
        rlMultMatrixf(shear_mat);
        translate_mat[12] = square->center.x;
        translate_mat[13] = square->center.y;
        rlMultMatrixf(translate_mat);

        int i;
        for(i=0;i<BEZIER_POINTS;++i){
            DrawTriangle(square->top_vertices[i], (Vector2){0,0}, square->top_vertices[i+1], square->color);
            DrawTriangle(square->bottom_vertices[i], (Vector2){0,0}, square->bottom_vertices[i+1], square->color);
            DrawTriangle(square->left_vertices[i], (Vector2){0,0}, square->left_vertices[i+1], square->color);
            DrawTriangle(square->right_vertices[i], (Vector2){0,0}, square->right_vertices[i+1],square->color);
        }
    rlPopMatrix();
}

void draw_afterimages(struct player_obj *player){
    struct afterImage *current = player->after_img_head;
    int i;
    while (current != NULL){
        rlPushMatrix();
            shear_mat[4] = -current->velocity.x / 600;
            rlMultMatrixf(shear_mat);
            translate_mat[12] = current->pos.x;
            translate_mat[13] = current->pos.y;
            rlMultMatrixf(translate_mat);
            current->color.a = 255 * current->opacity;
            for(i=0;i<BEZIER_POINTS;++i){
                DrawTriangle(current->top_vertices[i], (Vector2){0,0}, current->top_vertices[i+1], current->color);
                DrawTriangle(current->bottom_vertices[i], (Vector2){0,0}, current->bottom_vertices[i+1], current->color);
                DrawTriangle(current->left_vertices[i], (Vector2){0,0}, current->left_vertices[i+1], current->color);
                DrawTriangle(current->right_vertices[i], (Vector2){0,0}, current->right_vertices[i+1], current->color);
            }
            current->opacity -= 0.2;
        rlPopMatrix();
        current = current->next;
    }
    
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
