#include "header.h"
#include <math.h>

long mag(Vector2 vec){
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

Vector2 dir(Vector2 vec){
    long vec_mag = mag(vec);
    return (Vector2){.x = vec.x/vec_mag,.y = vec.y/vec_mag};
}

int sign(float val){
    return (val > 0)?1:-1;
}

Vector2* three_point_beizer(Vector2 start, Vector2 mid, Vector2 end){
    Vector2 vertices[BEZIER_POINTS];
    double t;
    double t_prime;
    for (int i=0;i<BEZIER_POINTS;++i){
        t = i/BEZIER_POINTS;
        t_prime = 1-t;
        vertices[i].x = t_prime*t_prime*start.x + 2*t_prime*t*mid.x + t*t*end.x;
        vertices[i].y = t_prime*t_prime*start.y + 2*t_prime*t*mid.y + t*t*end.y;
    }
    return vertices;
}