
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

void PID::GyroRealize(float Gyroz) {
    float diffErr = 2.0f;
    float diffOut = 3.0f;

    // 求取误差
    nowVal = Gyroz;
    nowErr = exVal - nowVal;
    
    // 误差较小时取消误差
    if (fabs(nowErr) < diffErr) {
        nowErr = 0.0f;
    }

    sumErr += nowErr;

    // PID计算
    out = Kp * nowErr + Ki * Ki_ * sumErr + Kp * (nowErr - lastErr);

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
}