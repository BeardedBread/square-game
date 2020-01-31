#include <raylib.h>
#include <stdio.h>

// Global Constants
#define BEZIER_POINTS 5
#define LEFT KEY_LEFT
#define RIGHT KEY_RIGHT
#define JUMP KEY_SPACE
#define DASH KEY_Z

struct kinematic_obj
{
    Rectangle rect;
    Vector2 velocity;
    Vector2 pos;
    int ori_width;
    int ori_height;
    double dim_reduction[4];
    double set_dim_reduction[4];
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
    struct squishy_square *image;
};

extern unsigned int PLAYER_SIZE;

struct squishy_square
{
    struct kinematic_obj *parent;
    Color color;
    Vector2 center;
    
    double top_offset;
    double bottom_offset;
    double left_offset;
    double right_offset;

    double target_offsets[4];

    Vector2 top_vertices[BEZIER_POINTS+1];
    Vector2 bottom_vertices[BEZIER_POINTS+1];
    Vector2 left_vertices[BEZIER_POINTS+1];
    Vector2 right_vertices[BEZIER_POINTS+1];
};

// Object functions, kinematics.c
struct kinematic_obj init_kinematic_obj(int width, int height);
void move(struct kinematic_obj *obj, Vector2 acceleration);
void set_position(struct kinematic_obj *obj, int x, int y);
bool place_meeting(struct kinematic_obj *obj, Vector2 dir);
void scale_rect(struct kinematic_obj *obj);
void adjust_hitbox(struct kinematic_obj *obj);

// Math functions, math.c
long mag(Vector2 vec);
int sign(float val);
Vector2 dir(Vector2 vec);
void approach(double *val, double target, float f);

// Linked list, linked_list.c
extern struct kinematic_obj_node *kinematic_HEAD;
void create_list(void);
void add_node(struct kinematic_obj *obj);
//struct kinematic_obj_node **get_list();
void free_list(void);

// Squishy Square functions, squishy.c
struct squishy_square init_squishy_square(struct kinematic_obj *parent, Color color);
void update_squishy(struct squishy_square *square);
void draw_squishy(struct squishy_square *square);
void set_squish_target_offset(struct squishy_square *square, unsigned int dir, int val);

//Player stuff, player.c
struct player_obj init_player_obj();
void player_input_check(struct player_obj *player);

//Debug stuff, debug.c
void state_string(char *str, enum PLAYER_STATE state);