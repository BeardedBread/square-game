/*******************************************************************************************
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2019 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

/* Issues:
    Playing with a USB keyboard: Cannot handle simultaneous key input. I think this is more
    of a GLFW issue.
*/

#include "header.h"
struct kinematic_obj_node *kinematic_HEAD = NULL;
struct target_obj_node *target_HEAD = NULL;
int PLAYER_ACCEL = 1500;
int JUMP_ACCEL = 15000;
int JUMP_SPD = 350;
int GRAV = 1000;

const int screenWidth = 800;
const int screenHeight = 450;

void main_menu(Camera2D camera);
void play_test_level(Camera2D camera);

int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    //char dir[7]; 

    InitWindow(screenWidth, screenHeight, "raylib");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Camera2D camera = { 0 };
    camera.offset = (Vector2){0,0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    while (!WindowShouldClose())
        //play_test_level(camera);
        main_menu(camera);
    CloseWindow();        // Close window and OpenGL context
}


void main_menu(Camera2D camera){
    camera.target = (Vector2){0,0};


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        main_menu_check();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            draw_menu();
        EndDrawing();

    }   
}

void play_test_level(Camera2D camera){

    char current_state[20];
    char current_spd[50];

    struct player_obj player = {
        .kinematic = init_kinematic_obj(PLAYER_SIZE, PLAYER_SIZE),
        .state = IDLE,
        .after_img_head = NULL,
        .after_img_tail = NULL
    };

    struct kinematic_obj tile = init_kinematic_obj(900, 100);
    struct kinematic_obj tile2 = init_kinematic_obj(100, 40);
    struct kinematic_obj tile3 = init_kinematic_obj(100, 40);
    struct kinematic_obj tile4 = init_kinematic_obj(40, 200);
    struct kinematic_obj tile5 = init_kinematic_obj(40, 200);
    struct kinematic_obj tile6 = init_kinematic_obj(50, 300);
    struct kinematic_obj tile7 = init_kinematic_obj(50, 300);

    set_position(&player.kinematic, 400, 100);
    set_position(&tile, -50, 380);
    set_position(&tile2, 100, 280);
    set_position(&tile3, 500, 280);
    set_position(&tile4, 250, 140);
    set_position(&tile5, 350, 100);
    set_position(&tile6, 0, 50);
    set_position(&tile7, 700, 80);
    struct squishy_square sqr = init_squishy_square(&player.kinematic, RED);
    player.image = &sqr;
    //camera.target = player.kinematic.pos;

    add_kinematic_node(&tile, &kinematic_HEAD);
    add_kinematic_node(&tile2, &kinematic_HEAD);
    add_kinematic_node(&tile3, &kinematic_HEAD);
    add_kinematic_node(&tile4, &kinematic_HEAD);
    add_kinematic_node(&tile5, &kinematic_HEAD);
    add_kinematic_node(&tile6, &kinematic_HEAD);
    add_kinematic_node(&tile7, &kinematic_HEAD);
    add_kinematic_node(&player.kinematic, &kinematic_HEAD);


    struct target_obj target = init_target(50, 300);
    set_position(&target.kinematic, 300, 100);
    add_target_node(&target, &target_HEAD);
    
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose() || IsKeyPressed(KEY_Q))    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        player_input_check(&player);

        update_squishy(&sqr);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            struct kinematic_obj_node *current;
            struct target_obj_node *target_current;
            // Camera target follows player
            camera.target = (Vector2){ player.kinematic.pos.x -screenWidth/2, player.kinematic.pos.y -screenHeight/2, };
            BeginMode2D(camera);
                draw_afterimages(&player);
                draw_squishy(&sqr);
                current = kinematic_HEAD;
                while(current){
                    if (current->obj != &player.kinematic)
                        DrawRectangleLinesEx(current->obj->rect, 1, BLACK);
                    current = current->next;
                }
                target_current = target_HEAD;
                while(target_current){
                    if (!target_current->obj->destroyed)
                        DrawCircle(target_current->obj->kinematic.pos.x, target_current->obj->kinematic.pos.y, 
                        target_current->obj->kinematic.ori_width, BLACK);
                    target_current = target_current->next;
                }
                DrawFPS(0,0);
                state_string(current_state, player.state);
                DrawText(current_state, 250, 0, 12, BLACK);
                //display_input(dir);
                //DrawText(dir, 0, 50, 12, BLACK);
                sprintf(current_spd, "Velocity: {%.2f,%.2f}", player.kinematic.velocity.x,player.kinematic.velocity.y);
                DrawText(current_spd, 350, 0, 12, BLACK);
            EndMode2D();

        EndDrawing();
        remove_last_afterimage(&player);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free_kinematic_list(&kinematic_HEAD);
    free_target_list(&target_HEAD);
    free_afterimages(&player);
    //--------------------------------------------------------------------------------------

    return;
}