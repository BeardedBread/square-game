#include "header.h"
#include <math.h>

double mag(Vector2 vec){
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

Vector2 dir(Vector2 vec){
    long vec_mag = mag(vec);
    return (Vector2){.x = vec.x/vec_mag,.y = vec.y/vec_mag};
}

int sign(double val){
    return (val > 0)?1:-1;
}

void approach(double *val, double target, float f){
    *val += (target - *val) * f;
}