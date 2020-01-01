#include <raylib.h>
#include <stdio.h>

// Global Constants
#define BEZIER_POINTS 7
#define LEFT KEY_LEFT
#define RIGHT KEY_RIGHT
#define JUMP KEY_SPACE
#define DASH KEY_Z

struct kinematic_obj
{
    Rectangle rect;
    Vector2 velocity;
};

struct kinematic_obj_node
{
    struct kinematic_obj *obj;
    struct kinematic_obj_node *next;
};

enum PLAYER_STATE
{
    IDLE = 0,
    JUMP_SQUAT,
    JUMPING,
    FALLING,
    LANDING,
    DASH_START,
    DASHING,
    DASH_END,
    RUN_START,
    RUNNING,
    RUN_END,
    TURN_AROUND
};

struct player_obj
{
    struct kinematic_obj kinematic;
    enum PLAYER_STATE state;
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
extern struct kinematic_obj_node *kinematic_HEAD;
void create_list(void);
void add_node(struct kinematic_obj *obj);
//struct kinematic_obj_node **get_list();
void free_list(void);

// Squishy Square functions, squishy.c
struct squishy_square init_squishy_square(Rectangle *rect, Color color);
void update_squishy(struct squishy_square *square);
void draw_squishy(struct squishy_square *square);

//Player stuff, player.c
struct player_obj init_player_obj();
void player_input_check(struct player_obj *player);

//Debug stuff, debug.c
void state_string(char *str, enum PLAYER_STATE state);