
#ifndef _MOVE_H_
#define _MOVE_H_

#include "Define.h"

struct moveControl
{
    char Dir;
    int Speed;

    void moveExec();
};

#endif