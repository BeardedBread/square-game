#include "header.h"
#include <math.h>

int sign(double val){
    return (val > 0)?1:-1;
}

void approach(double *val, double target, float f){
    *val += (target - *val) * f;
}

Vector2 center(Rectangle rect){
    return (Vector2){
        .x = rect.x + rect.width/2, 
        .y = rect.y + rect.height/2
    };
}