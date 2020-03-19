#include "header.h"
#include <math.h>

int sign(double val){
    return (val > 0)?1:-1;
}

void approach(double *val, double target, float f){
    *val += (target - *val) * f;
}