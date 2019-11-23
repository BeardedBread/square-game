#include <raylib.h>
#include <stdio.h>

#define BEZIER_POINTS 25

struct kinematic_obj
{
    Rectangle rect;
    Vector2 velocity;
    Color color;
};

struct squishy_square
{
    Vector2 pos;
    double width;
    double height;
    double top_handle;
    double bott_handle;
    double left_handle;
    double right_handle;
};

// Placeholder collision checking structure. Use linked list for now
// Need to implement some sort of tree structure for efficient collision checking
struct obj_node
{
    struct kinematic_obj *obj;
    struct obj_node *next;
};

extern struct obj_node *HEAD;

// Object functions, kinematics.c
struct kinematic_obj init_kinematic_obj(int width, int height);
void move(struct kinematic_obj *obj, Vector2 acceleration);
void set_position(struct kinematic_obj *obj, int x, int y);
bool place_meeting(struct kinematic_obj *obj, Vector2 dir);

// Math functions, math.c
long mag(Vector2 vec);
int sign(float val);
Vector2 dir(Vector2 vec);
Vector2* three_point_beizer(Vector2 start, Vector2 mid, Vector2 end);

// Linked list, linked_list.c

// Squishy Square functions, squishy.c
void draw_squishy();