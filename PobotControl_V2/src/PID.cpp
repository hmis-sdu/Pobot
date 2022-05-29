
#include "PID.h"

PID::PID(float Kp, float Ki, float Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;
    Ki_ = 1.0f;
}

void PID::init(float nowVal, float exVal) {
    this->nowVal = nowVal;
    this->exVal = exVal;
}

/*
0 -> 350
330 -> 350
*/

void PID::GyroRealize(float Gyroz) {
    float diffErr = 2.0f;
    float diffOut = 3.0f;

    // 求取误差
    nowVal = Gyroz;
    float c1 = nowVal + 180.0f;
    if (c1 > 360.0f) {  // 跨越
        if (c1 - 360.0f < exVal && exVal < nowVal) {    // 顺时针到达,nowErr<0
            nowErr = exVal - nowVal;    
        }
        else {      // 逆时针到达,nowErr>0
            if (exVal > nowVal) {
                nowErr = exVal - nowVal;
            }
            else {
                nowErr = exVal + 360.f - nowVal;
            }
        }
    }
    else {
        if (nowVal < exVal && exVal < c1) {     // 逆时针到达,nowErr>0
            nowErr = exVal - nowVal;
        }
        else {
            if (exVal < nowVal) {       // 顺时针到达,nowErr<0
                nowErr = exVal - nowVal;
            }
            else {
                nowErr = exVal - 360.0f - nowVal;   
            }
        }
    }
    

    // 误差较小时取消误差
    if (fabs(nowErr) < diffErr) {
        nowErr = 0.0f;
    }

    sumErr += nowErr;

    // PID计算
    out = Kp * nowErr + Ki * Ki_ * sumErr + Kp * (nowErr - lastErr);
    // out = -out;

    // 输出项一阶低通滤波
    out = 0.800f * out + 0.200f * lastout;

    // 消除静差
    if (fabs(out) < diffOut) {
        out = 0.0f;
    }

    // 更新
    lastErr = nowErr;
    lastout = out;
}

void PID::out2Speed(moveControl *ml) {
    // todo
    // 考虑要素：量纲,模运算
    if (out > 0) {  // 逆时针可达
        ml->Dir = 'a';
        ml->Speed = out / 1.0f;
    }
    else {  // 顺时针可达
        ml->Dir = 'd';
        ml->Speed = out / 1.0f;
    }
}