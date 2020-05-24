#include "header.h"
#include <raymath.h>

static unsigned int option = 0;

const unsigned int max_options = 2;
const char menu_options[2][10] = {"Play", "Exit"};

Vector2 menu_list_pos = (Vector2){200,100};

Vector2 t1 = (Vector2){180, 100};
Vector2 t2 = (Vector2){190, 105};
Vector2 t3 = (Vector2){180, 110};
Vector2 offset = (Vector2){0, 50};


void main_menu_check(){
    if (IsKeyPressed(UP))
        --option;
    if (IsKeyPressed(DOWN))
        ++option;
}

void draw_menu(){
    DrawFPS(0,0);
    for(unsigned int i=0;i<2;++i){
        DrawText(menu_options[i], menu_list_pos.x, 
        menu_list_pos.y + 50 * i, 12, BLACK);
    }
    //display_input(dir);
    //DrawText(dir, 0, 50, 12, BLACK);

    // Triangles has to be anticlockwse, because z axis
    Vector2 off = Vector2Scale(offset, option);
    DrawTriangle(Vector2Add(t1, off), Vector2Add(t3, off), Vector2Add(t2, off), BLACK);
}