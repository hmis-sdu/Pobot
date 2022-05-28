
#ifndef _PID_H_
#define _PID_H_

#include <math.h>
#include "moveControler.h"

struct PID
{
    PID(float Kp, float Ki, float Kd);
    void init(float nowVal, float exVal);
    void GyroRealize(float Gyroz);
    void out2Speed(moveControl *ml);

    float nowVal, lastVal, exVal;
    float nowErr, lastErr, sumErr;
    float Kp, Ki, Ki_, Kd;
    float out, lastout;
};

#endif /* CODE_PID_H_ */
