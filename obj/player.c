#include "header.h"


#define PLAYER_ACCEL 1500
#define RUN_INIT_SPD 200
#define JUMP_SPD 500
#define GRAV 1000

static bool allow_move = true;
static int jumps = 1;
static frame_counter = 0;
static int run_dir = 1;
static bool allow_friction = true;
const unsigned int run_start_frames = 8;
const unsigned int jump_squat_frames = 6;
const unsigned int land_lag_frames = 5;

// The player FSM
void player_input_check(struct player_obj *player){
    Vector2 accel = (Vector2){
        .x = 0,
        .y = 0
    };
    switch(player->state){
        case IDLE:
            if (IsKeyDown(LEFT) || IsKeyDown(RIGHT)){
                player->state = RUN_START;
                allow_friction = false;
                player->kinematic.velocity.x = (IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT)) * RUN_INIT_SPD;
            }
        break;
        case RUN_START:
            run_dir = sign(player->kinematic.velocity.x);
            // Run Opposite Direction
            if ((IsKeyPressed(LEFT) && run_dir == 1) || (IsKeyPressed(RIGHT) && run_dir == -1)){
                frame_counter = 0;
                player->kinematic.velocity.x *= -1;
            }else{
                // Complete the run startup
                if(frame_counter<run_start_frames)
                    ++frame_counter;
                else{
                    frame_counter = 0;
                    allow_friction = true;
                    player->state = RUNNING;
                }
            }
        break;
        case RUNNING:
            run_dir = sign(player->kinematic.velocity.x);
            if ((IsKeyPressed(LEFT) && run_dir == 1) || (IsKeyPressed(RIGHT) && run_dir == -1)){
                player->state = RUN_END;
            }else{
                if (!IsKeyDown(LEFT) && !IsKeyDown(RIGHT)){
                    player->state = RUN_END;
                }else{
                    accel.x = PLAYER_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
                }
            }
        break;
        case RUN_END:
            if(player->kinematic.velocity.x < 10 && player->kinematic.velocity.x > -10){
                if(IsKeyDown(LEFT) || IsKeyDown(RIGHT)){
                    player->state = RUNNING;
                }else{
                    player->state = IDLE;
                }
            }
        break;
        case TURN_AROUND:

        break;
        case JUMP_SQUAT:          
            if(frame_counter<jump_squat_frames)
                    ++frame_counter;
            else{
                frame_counter = 0;
                if (IsKeyDown(JUMP))
                    player->kinematic.velocity.y = -JUMP_SPD;
                else
                    player->kinematic.velocity.y = -JUMP_SPD/2;
                player->state = JUMPING;
            }
        break;
        case JUMPING:
            accel.x = PLAYER_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (player->kinematic.velocity.y > 0)
                player->state = FALLING;
        break;
        case FALLING:
            accel.x = PLAYER_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (place_meeting(&player->kinematic, (Vector2){0,1})){
                player->state = LANDING;
            }
        break;
        case LANDING:
            if(frame_counter<land_lag_frames)
                    ++frame_counter;
            else{
                jumps = 1;
                frame_counter = 0;
                if (IsKeyDown(LEFT) || IsKeyDown(RIGHT)){
                    player->state = RUNNING;
                }else{
                    player->state = IDLE;
                }
            }            
        break;
        case DASH_START:

        break;
        case DASHING:

        break;
        case DASH_END:

        break;
    }

    if (IsKeyPressed(JUMP) && jumps > 0){
        player->state = JUMP_SQUAT;
        allow_friction = true;
        --jumps;
    }

    if (allow_friction == true)
        accel.x -= player->kinematic.velocity.x * 8;

    if (!place_meeting(&player, (Vector2){0,1})){
        accel.y = GRAV;
    }

    move(&player->kinematic, accel);
}
