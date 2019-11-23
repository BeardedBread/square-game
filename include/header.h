#include <raylib.h>
#include <stdio.h>

#define BEZIER_POINTS 10

struct kinematic_obj
{
    Rectangle rect;
    Vector2 velocity;
    Color color;
};

struct squishy_square
{
    Rectangle *rect;
    Color color;
    Vector2 topleft;
    Vector2 topright;
    Vector2 bottomleft;
    Vector2 bottomright;
    Vector2 top_handle;
    Vector2 bottom_handle;
    Vector2 left_handle;
    Vector2 right_handle;
    double top_offset;
    double bottom_offset;
    double left_offset;
    double right_offset;
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

// Linked list, linked_list.c

// Squishy Square functions, squishy.c
struct squishy_square init_squishy_square(Rectangle *rect, Color color);
void update_squishy(struct squishy_square *square);
void draw_squishy(struct squishy_square *square);
