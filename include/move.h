#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "utils.h"

// REFACTOR to use 3,3,3,3 bits for pos, 3 bits for step, 1 bit for color
struct Move {
    byte r1, c1, r2, c2;
    byte step;
    byte color;
    unsigned long origin;
    
    Move()
    { 
        r1=r2=c1=c2=0; 
        color=0;
        step=0;
        origin=0;
    };

    Move(byte _r1, byte _c1, byte _r2, byte _c2, byte _step, byte _color, unsigned long _origin) :
        r1(_r1) , c1(_c1) , r2(_r2) , c2(_c2), step(_step), color(_color), origin(_origin)
    {};
    
};

bool str_to_Move(std::string s, Move& out);

#endif