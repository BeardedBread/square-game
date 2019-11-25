/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
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

#include "header.h"
struct obj_node *HEAD;
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

    InitWindow(screenWidth, screenHeight, "raylib");

    Camera2D camera = { 0 };
    camera.offset = (Vector2){0,0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    struct player_obj player = {
        .kinematic = init_kinematic_obj(40, 40),
        .state = IDLE
    };

    struct kinematic_obj tile = init_kinematic_obj(900, 100);
    set_position(&player, 400, 300);
    set_position(&tile, -50, 380);
    struct squishy_square sqr = init_squishy_square(&player.kinematic.rect, RED);

    // TODO: get a linked list implementation
    struct obj_node tile_node = {.obj=&tile, .next=NULL};
    struct obj_node player_node = {.obj=&player.kinematic, .next=&tile_node};
    HEAD = &player_node;
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    struct obj_node *current;
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
            draw_squishy(&sqr);

            BeginMode2D(camera);
                current = HEAD;
                while(current){
                    DrawRectangleLinesEx(current->obj->rect, 1, BLACK);
                    current = current->next;
                }
                DrawFPS(0,0);
                state_string(current_state, player.state);
                DrawText(current_state, 250, 0, 12, BLACK);
                sprintf(current_spd, "Velocity: {%.2f,%.2f}", player.kinematic.velocity.x,player.kinematic.velocity.y);
                DrawText(current_spd, 350, 0, 12, BLACK);
            EndMode2D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}