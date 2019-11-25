#include "header.h"

void state_string(char *str, enum PLAYER_STATE state){
    switch(state){
        case IDLE: sprintf(str, "%s", "IDLE");break;
        case JUMP_SQUAT: sprintf(str, "%s", "JUMP_SQUAT");break;
        case JUMPING: sprintf(str, "%s", "JUMPING");break;
        case FALLING: sprintf(str, "%s", "FALLING");break;
        case LANDING: sprintf(str, "%s", "LANDING");break;
        case DASH_START: sprintf(str, "%s", "DASH_START");break;
        case DASHING: sprintf(str, "%s", "DASHING");break;
        case DASH_END: sprintf(str, "%s", "DASH_END");break;
        case RUN_START: sprintf(str, "%s", "RUN_START");break;
        case RUNNING: sprintf(str, "%s", "RUNNING");break;
        case RUN_END: sprintf(str, "%s", "RUN_END");break;
        case TURN_AROUND: sprintf(str, "%s", "TURN_AROUND");break;
    }
}