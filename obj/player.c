#include "header.h"
#include <raymath.h>

#define PLAYER_ACCEL 1600
#define AIR_ACCEL 800
#define RUN_INIT_SPD 230
#define JUMP_SPD 350
#define GRAV 1200
#define DASH_SPD 550
#define DEFAULT_JUMP_COUNT 1

static bool allow_move = true;
static bool allow_friction = true;
static bool on_ground = true;
static bool short_hop = false;

static unsigned int jumps = DEFAULT_JUMP_COUNT;
static unsigned int dash_count = 1;

static unsigned int frame_counter = 0;
static unsigned int afterimage_fcounter = 10;

static Color afterimage_c = (Color){0,0,0,255};

static int run_dir = 1;
static Vector2 dash_vec = (Vector2){0.0, 0.0};

const unsigned int run_start_frames = 10;
const unsigned int jump_squat_frames = 4;
const unsigned int land_lag_frames = 2;
const unsigned int dash_time_frames = 8;
const unsigned int afterimage_frames = 10;

static enum PLAYER_STATE state_buffer = IDLE;
unsigned int PLAYER_SIZE = 30;

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
                player->kinematic.velocity.x = (IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT)) * RUN_INIT_SPD;
            }
        break;
        case RUN_START:
            if (player->kinematic.velocity.x == 0){
                frame_counter = 0;
                player->state = IDLE;
            }else{run_dir = sign(player->kinematic.velocity.x);
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
                        player->state = RUNNING;
                    }
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
            set_squish_target_offset(player->image, 0, 0);
            set_squish_target_offset(player->image, 2, 0);
            if (player->kinematic.velocity.x == 0){
                if (place_meeting(&player->kinematic, (Vector2){1,0})){
                    player->kinematic.dim_reduction[0] = PLAYER_SIZE / 2;
                    //set_squish_target_offset(player->image, 2, 15);
                }else if (place_meeting(&player->kinematic, (Vector2){1,0})){
                    player->kinematic.dim_reduction[2] = PLAYER_SIZE / 2;
                    //set_squish_target_offset(player->image, 2, 15);
                }
                player->state = IDLE;
            }
        break;
        case RUN_END:
            if(player->kinematic.velocity.x < 10 && player->kinematic.velocity.x > -10){
                set_squish_target_offset(player->image, 2, 0);
                player->kinematic.dim_reduction[2] = 0;
                set_squish_target_offset(player->image, 0, 0);
                player->kinematic.dim_reduction[0] = 0;
                if(IsKeyDown(LEFT) || IsKeyDown(RIGHT)){
                    player->state = RUNNING;
                }else{
                    player->state = IDLE;
                }
            }else{
                //Skidding
                if (player->kinematic.velocity.x > 0){
                    if (!IsKeyDown(RIGHT)){
                        set_squish_target_offset(player->image, 0, 15);
                        player->kinematic.dim_reduction[0] = 10;
                    }else{
                        set_squish_target_offset(player->image, 0, 0);
                        player->kinematic.dim_reduction[0] = 0;
                    }
                }else{
                    if (!IsKeyDown(LEFT)){
                        set_squish_target_offset(player->image, 2, 15);
                        player->kinematic.dim_reduction[2] = 10;
                    }else{
                        set_squish_target_offset(player->image, 2, 0);
                        player->kinematic.dim_reduction[2] = 0;
                    }
                }
            }
        break;
        case JUMP_SQUAT:
            player->kinematic.set_dim_reduction[1] = 10;
            set_squish_target_offset(player->image, 1, 20);
            if(frame_counter<jump_squat_frames){
                    ++frame_counter;
                    if (short_hop != true && !IsKeyDown(JUMP)){
                        short_hop = true;
                    }
            }
            else{
                frame_counter = 0;
                if (short_hop == true)
                    player->kinematic.velocity.y = -JUMP_SPD * 0.6;
                else
                    player->kinematic.velocity.y = -JUMP_SPD;
                player->state = JUMPING;
                short_hop = false;
            }
        break;
        case JUMPING:
            accel.x = AIR_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (player->kinematic.velocity.y >= 0){
                player->state = FALLING;
            }
        break;
        case FALLING:
            accel.x = AIR_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT));
            if (on_ground){
                frame_counter = 0;
                player->state = LANDING;
                state_buffer = IDLE;
            }
        break;
        case LANDING:
            dash_count = 1;
            if (frame_counter == 0){
                player->kinematic.dim_reduction[3] = 0;
                player->kinematic.set_dim_reduction[3] = 0;
                player->kinematic.dim_reduction[1] = PLAYER_SIZE;
                player->kinematic.set_dim_reduction[1] = 0;
                set_squish_target_offset(player->image, 1, 0);
            }
            if(frame_counter<land_lag_frames){
                    ++frame_counter;
                if (IsKeyDown(JUMP))
                    state_buffer = JUMP_SQUAT;
            }                              
            else{
                frame_counter = 0;
                jumps = DEFAULT_JUMP_COUNT;
                if (state_buffer == JUMP_SQUAT){
                    player->state = state_buffer;
                    --jumps;
                }else if (IsKeyDown(LEFT) || IsKeyDown(RIGHT))
                    player->state = RUNNING;
                else
                    player->state = IDLE;
            }            
        break;
        case DASHING:
            player->kinematic.velocity.x = dash_vec.x;
            player->kinematic.velocity.y = dash_vec.y;
            ++frame_counter;
            if (frame_counter > dash_time_frames){
                player->kinematic.velocity.x *= 0.8;
                player->kinematic.velocity.y *= 0.8;
                if (!on_ground){
                    player->state = JUMPING;
                }
                else{
                    player->state = RUNNING;
                    dash_count = 1;
                    jumps = 1;
                }
            }
        break;
    }

    // This accounts for player leaving the ground without jumping (dashing or falling)
    // Must be done before updating on_ground status
    // Possible change: store the previous on_ground state
    if (on_ground == true && !place_meeting(&player->kinematic, (Vector2){0,1})  && player->state != JUMP_SQUAT){
        jumps = 0;
        if (player->state != DASHING)
            player->state = FALLING;
        else
            player->state = JUMPING;
    }

    on_ground = place_meeting(&player->kinematic, (Vector2){0,1});

    // This check is to add gravity, but maybe could be combined with above
    if (!on_ground){
        accel.y = GRAV;
        if(player->state != DASHING && player->kinematic.velocity.y > 0)
            player->state = FALLING;
    }

    // Air friction is less than ground friction
    if (on_ground == true)
        accel.x -= player->kinematic.velocity.x * 6.0;
    else
        accel.x -= player->kinematic.velocity.x * 3.0;

    // Handle wall jumping
    // TODO: define the wall jump values
    if (IsKeyPressed(JUMP)){
        if (on_ground == false){
            bool left_check = place_meeting(&player->kinematic, (Vector2){-8,0});
            bool right_check = place_meeting(&player->kinematic, (Vector2){8,0});
            if (left_check || right_check){
                player->kinematic.velocity.y = -350;
                player->kinematic.velocity.x = left_check? 400 : -400;
                player->state = JUMPING;
                afterimage_fcounter = 5;
                afterimage_c.r = 128;
                afterimage_c.g = 128;
                afterimage_c.b = 128;
            }
        }
    }

    // Handle dashing
    if  (IsKeyPressed(DASH) && dash_count > 0){
        // Determine the direction of dashing
        dash_vec.x = 0;
        if (IsKeyDown(RIGHT))
            ++dash_vec.x;
        if (IsKeyDown(LEFT))
            --dash_vec.x;
        
        dash_vec.y = 0;
        if (IsKeyDown(DOWN))
            ++dash_vec.y;
        if (IsKeyDown(UP))
            --dash_vec.y;

        // Default a direction
        if (dash_vec.x == 0 && dash_vec.y == 0){
            dash_vec.x = sign(player->kinematic.velocity.x);
            if (place_meeting(&player->kinematic, (Vector2){dash_vec.x,0}))
                dash_vec.y = -1;
        }
        
        // Apply the scalar value, normalised to the unit direction
        dash_vec = Vector2Scale(Vector2Normalize(dash_vec), DASH_SPD);
        --dash_count;
        frame_counter=0;
        afterimage_fcounter=0;
        afterimage_c.r = 0;
        afterimage_c.g = 0;
        afterimage_c.b = 255;
        player->state = DASHING;
    }

    // All velocity modification must be done before calling the move function
    move(&player->kinematic, accel);

    // Handle jumping
    if (IsKeyPressed(JUMP) && jumps > 0){
        player->state = JUMP_SQUAT;
        short_hop = false;
        --jumps;
    }
   
    // Deform player based on falling speed
    player->kinematic.set_dim_reduction[1] = 0;
    player->kinematic.set_dim_reduction[3] = 0;
    if (on_ground == false){
        if (player->kinematic.velocity.y < 0)
            player->kinematic.set_dim_reduction[3] = -player->kinematic.velocity.y / 40;
        if (player->kinematic.velocity.y > 0)
            player->kinematic.set_dim_reduction[1] = player->kinematic.velocity.y / 40;

        set_squish_target_offset(player->image, 1, player->kinematic.velocity.y / 30);
        set_squish_target_offset(player->image, 3, -player->kinematic.velocity.y / 30);
    }

    //Generate afterimages
    if (afterimage_fcounter < afterimage_frames){
        if(afterimage_fcounter%2 == 0)
            create_afterimage(player, afterimage_c);
        ++afterimage_fcounter;
    }

    // Set the hitbox reductions
    adjust_hitbox(&player->kinematic);

    if (dash_count == 0)
        player->image->color.b = 255;
    else
        player->image->color.b = 0;
}