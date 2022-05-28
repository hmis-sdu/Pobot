#include <Arduino.h>
#include "moveControler.h"

void moveControl::moveExec() {
  if (Dir == 'd')
  { // 右转
    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);
  }
  else if (Dir == 'w')
  { // 前进
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
  }
  else if (Dir == 'a')
  { // 左转
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
  }
  else if (Dir == 's')
  { // 后退
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
  }

  analogWrite(E1, Speed);
  analogWrite(E2, Speed);
}