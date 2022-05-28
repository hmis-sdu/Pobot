#include <Arduino.h>
#include "Define.h"
#include "moveControler.h"

extern moveControl ml;

void test1()
{
    {
        int value;
        for (value = 0; value <= 255; value += 5)
        {
            digitalWrite(M1, HIGH);
            digitalWrite(M2, HIGH);
            analogWrite(E1, value); // PWM调速
            analogWrite(E2, value); // PWM调速
            delay(30);
        }
        delay(1000);
    }
    {
        int value;
        for (value = 0; value <= 255; value += 5)
        {
            digitalWrite(M1, LOW);
            digitalWrite(M2, LOW);
            analogWrite(E1, value); // PWM调速
            analogWrite(E2, value); // PWM调速
            delay(30);
        }
        delay(1000);
    }
}

void test2()
{
    ml.Speed = 255;
    ml.Dir = 'w';
    ml.moveExec();
    delay(3000);
    ml.Dir = 's';
    ml.moveExec();
    delay(3000);
    ml.Dir = 'a';
    ml.moveExec();
    delay(3000);
    ml.Dir = 'd';
    ml.moveExec();
    delay(3000);
}
