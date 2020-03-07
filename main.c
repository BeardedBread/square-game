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
extern struct kinematic_obj_node *kinematic_HEAD;
int PLAYER_ACCEL = 1500;
int JUMP_ACCEL = 15000;
int JUMP_SPD = 350;
int GRAV = 1000;
int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    char current_state[20];
    char current_spd[50];
    //char dir[7]; 

    InitWindow(screenWidth, screenHeight, "raylib");

    Camera2D camera = { 0 };
    camera.offset = (Vector2){0,0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

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

    add_node(&tile);
    add_node(&tile2);
    add_node(&tile3);
    add_node(&tile4);
    add_node(&tile5);
    add_node(&tile6);
    add_node(&tile7);
    add_node(&player.kinematic);
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    struct kinematic_obj_node *current;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
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
            draw_afterimages(&player);
            draw_squishy(&sqr);

            BeginMode2D(camera);
                current = kinematic_HEAD;
                while(current){
                    DrawRectangleLinesEx(current->obj->rect, 1, BLACK);
                    current = current->next;
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
    free_list();
    free_afterimages(&player);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}