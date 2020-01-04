#include "header.h"


#define PLAYER_ACCEL 1600
#define AIR_ACCEL 400
#define RUN_INIT_SPD 230
#define JUMP_SPD 500
#define GRAV 1200

static bool allow_move = true;
static bool allow_friction = true;
static bool on_ground = true;
static bool short_hop = false;
static unsigned int jumps = 1;
static unsigned int frame_counter = 0;
static int run_dir = 1;
static enum PLAYER_STATE state_buffer = IDLE;

const unsigned int run_start_frames = 10;
const unsigned int jump_squat_frames = 4;
const unsigned int land_lag_frames = 6;

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
                //allow_friction = false;
                player->kinematic.velocity.x = (IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT)) * RUN_INIT_SPD;
            }
        break;
        case RUN_START:
            run_dir = sign(player->kinematic.velocity.x);
            // Run Opposite Direction
            if ((IsKeyPressed(LEFT) && run_dir == 1) || (IsKeyPressed(RIGHT) && run_dir == -1)){
                frame_counter = 0;
                player->kinematic.velocity.x = -run_dir * RUN_INIT_SPD;
            }else{
                // Complete the run startup
                if(frame_counter<run_start_frames){
                    ++frame_counter;
                    if (IsKeyDown(LEFT) || IsKeyDown(RIGHT)){
                        player->kinematic.velocity.x = run_dir * RUN_INIT_SPD;
                    }
                }else{
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
            if(frame_counter<jump_squat_frames){
                    ++frame_counter;
                    if (short_hop != true && !IsKeyDown(JUMP)){
                        short_hop = true;
                    }
            }
            else{
                frame_counter = 0;
                if (short_hop == true)
                    player->kinematic.velocity.y = -JUMP_SPD/2;
                else
                    player->kinematic.velocity.y = -JUMP_SPD;
                player->state = JUMPING;
                on_ground = false;
                short_hop = false;
            }
        break;
        case JUMPING:
            accel.x = AIR_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (player->kinematic.velocity.y >= 0)
                player->state = FALLING;
        break;
        case FALLING:
            accel.x = AIR_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (place_meeting(&player->kinematic, (Vector2){0,1})){
                player->state = LANDING;
                on_ground = true;
                state_buffer = IDLE;
            }
        break;
        case LANDING:
            if(frame_counter<land_lag_frames){
                    ++frame_counter;
                if (IsKeyDown(JUMP))
                    state_buffer = JUMP_SQUAT;
            }                              
            else{
                jumps = 1;
                frame_counter = 0;
                if (state_buffer == JUMP_SQUAT){
                    player->state = state_buffer;
                    --jumps;
                }
                else if (IsKeyDown(LEFT) || IsKeyDown(RIGHT))
                    player->state = RUNNING;
                else
                    player->state = IDLE;
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
        short_hop = false;
        --jumps;
    }

    if (on_ground == true && !place_meeting(&player->kinematic, (Vector2){0,1})){
        jumps = 0;
        on_ground = false;
        allow_friction = true;
        player->state = FALLING;
    }

    if (allow_friction == true)
        if (on_ground)
            accel.x -= player->kinematic.velocity.x * 7.0;
        else
            accel.x -= player->kinematic.velocity.x * 1.0;

    if (!place_meeting(&player->kinematic, (Vector2){0,1})){
        accel.y = GRAV;
    }

    move(&player->kinematic, accel);
}
