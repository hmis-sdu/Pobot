#include <Arduino.h>


#define E1 10
#define M1 12
#define E2 11
#define M2 13

/*
移动控制
蓝牙通讯
*/

struct moveControl
{
    char Dir;
    int Speed;

    void moveExec();
};

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

struct cmdInterpreter
{
    char cmd[50];
    String type;
    int cnt;

    void run();

  private:
    moveControl ml;
};

void cmdInterpreter::run() {
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == ';')
    {
      // 接收到命令结束符进行命令解析
      cmd[cnt] = c;
      cnt = 0;

      type = strtok(cmd, ",");

      if (type = "GMC") {
        ml.Dir = (strtok(NULL, ","))[0];
        ml.Speed = atoi(strtok(NULL, ";"));
        ml.moveExec();
      }
    }
    else
    {
      cmd[cnt++] = c;
    }
  }
}

moveControl ml;
cmdInterpreter cr;

void setup() {
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);

  Serial.begin(9600);
}


// void test1();
// void test2();

void loop() {
  // test1();
  // test2();
}






/*
void test1() {
  { int value;
    for (value = 0 ; value <= 255; value += 5)
    {
      digitalWrite(M1, HIGH);
      digitalWrite(M2, HIGH);
      analogWrite(E1, value); //PWM调速
      analogWrite(E2, value); //PWM调速
      delay(30);
    }
    delay(1000);
  }
  { int value;
    for (value = 0 ; value <= 255; value += 5)
    {
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      analogWrite(E1, value); //PWM调速
      analogWrite(E2, value); //PWM调速
      delay(30);
    }
    delay(1000);
  }
}

void test2() {
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
*/