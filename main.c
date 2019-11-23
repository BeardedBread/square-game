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
int GRAV = 750;
int main() 
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib");

    Camera2D camera = { 0 };
    camera.offset = (Vector2){0,0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    struct kinematic_obj player = init_kinematic_obj(40, 40);
    player.color = BLUE;
    struct kinematic_obj tile = init_kinematic_obj(900, 100);
    set_position(&player, 400, 300);
    set_position(&tile, -50, 380);
    struct squishy_square sqr = init_squishy_square(&player.rect, RED);

    // TODO: get a linked list implementation
    struct obj_node tile_node = {.obj=&tile, .next=NULL};
    struct obj_node player_node = {.obj=&player, .next=&tile_node};
    HEAD = &player_node;
    
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    struct obj_node *current;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        //UpdateCamera(&camera);
            Vector2 accel = (Vector2){
                .x = PLAYER_ACCEL*(IsKeyDown(KEY_RIGHT)-IsKeyDown(KEY_LEFT)),
                .y = 0
                };

            if (!place_meeting(&player, (Vector2){0,1})){
                accel.y = GRAV;
            }
            accel.x -= player.velocity.x * 6.5;
            
            if (IsKeyDown(KEY_SPACE) && place_meeting(&player, (Vector2){0,1}))
                accel.y -= JUMP_ACCEL;
            move(&player, accel);

            update_squishy(&sqr);
            Vector2 center = (Vector2){
                .x = (sqr.topleft.x + sqr.topright.x)/2,
                .y = (sqr.topleft.y + sqr.bottomleft.y)/2
            };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                current = HEAD;
                while(current){
                    DrawRectangleRec(current->obj->rect, current->obj->color);
                    current = current->next;
                }
                DrawFPS(100,100);
                draw_squishy(&sqr);
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