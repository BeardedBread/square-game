#include <raylib.h>
#include <stdio.h>
#include <stdlib.h> 

// Global Constants
#define BEZIER_POINTS 5
#define LEFT KEY_LEFT
#define RIGHT KEY_RIGHT
#define DOWN KEY_DOWN
#define UP KEY_UP
#define JUMP KEY_SPACE
#define DASH KEY_Z

struct afterImage
{
    Vector2 velocity;
    Vector2 pos;
    Vector2 top_vertices[BEZIER_POINTS+1];
    Vector2 bottom_vertices[BEZIER_POINTS+1];
    Vector2 left_vertices[BEZIER_POINTS+1];
    Vector2 right_vertices[BEZIER_POINTS+1];

    float opacity;
    Color color;
    struct afterImage *prev;  
    struct afterImage *next;    
};

struct kinematic_obj
{
    Rectangle rect;
    Vector2 velocity;
    Vector2 pos;
    int ori_width;
    int ori_height;
    double dim_reduction[4];
    double set_dim_reduction[4];
    double x_shear;
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
    struct afterImage *after_img_head;
    struct afterImage *after_img_tail;
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

struct target_obj
{
    double radius;
    struct kinematic_obj kinematic;
    bool destroyed;
};
struct target_obj_node
{
    struct target_obj *obj;
    struct target_obj_node *next;
};

// Object functions, kinematics.c
struct kinematic_obj init_kinematic_obj(int width, int height);
void move(struct kinematic_obj *obj, Vector2 acceleration);
void set_position(struct kinematic_obj *obj, int x, int y);
bool place_meeting(struct kinematic_obj *obj, Vector2 dir);
void scale_rect(struct kinematic_obj *obj);
void adjust_hitbox(struct kinematic_obj *obj);
void add_kinematic_node(struct kinematic_obj *obj, struct kinematic_obj_node **HEAD);
//struct kinematic_obj_node **get_list();
void free_kinematic_list(struct kinematic_obj_node **HEAD);

// Math functions, math.c
double mag(Vector2 vec);
int sign(double val);
Vector2 dir(Vector2 vec);
void approach(double *val, double target, float f);
Vector2 center(Rectangle rect);

// Linked list, linked_list.c
void create_list(void);

// Squishy Square functions, squishy.c
struct squishy_square init_squishy_square(struct kinematic_obj *parent, Color color);
void update_squishy(struct squishy_square *square);
void draw_squishy(struct squishy_square *square);
void set_squish_target_offset(struct squishy_square *square, unsigned int dir, int val);
void draw_afterimages(struct player_obj *player);

//Player stuff, player.c
struct player_obj init_player_obj();
void player_input_check(struct player_obj *player);
void create_afterimage(struct player_obj *player, Color color);
void remove_last_afterimage(struct player_obj *player);
void free_afterimages(struct player_obj *player);

//Target stuff, target.c
struct target_obj init_target();
void free_target_list(struct target_obj_node **HEAD);
void add_target_node(struct target_obj *obj, struct target_obj_node **HEAD);
bool collide_target(struct kinematic_obj *obj, struct target_obj *target);

//Menu
void draw_menu();
void main_menu_check();
int get_selected_options();

//Debug stuff, debug.c
void state_string(char *str, enum PLAYER_STATE state);
void display_input(char *dir);
