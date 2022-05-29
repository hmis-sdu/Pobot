#include <Arduino.h>
#include <JY901.h>
#include "Define.h"
#include "moveControler.h"
#include "PID.h"

/*
移动控制
蓝牙通讯
pid控制精确转向 todo
  实际调控车轮转速
  蓝牙调试pid 
*/

struct cmdInterpreter
{
    cmdInterpreter(moveControl *ml, PID *mpid) {
      this->ml = ml;
      this->mpid = mpid;
    }
    char cmd[50];
    String type;
    int cnt;

    void run();

    moveControl *ml;
    PID *mpid;
};

void cmdInterpreter::run() {
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == ';')
    {
      // 接收到命令结束符进行命令解析
      Serial.println(cmd);
      cmd[cnt] = c;
      cnt = 0;

      type = strtok(cmd, ",");

      if (type = "GMC") {   // general move control: 通用移动控制
        ml->Dir = (strtok(NULL, ","))[0];
        ml->Speed = atoi(strtok(NULL, ";"));
        ml->moveExec();
      }
      // untested
      else if (type == "PS") {    // precisely steering：精确转向控制
        // 精确转向(方向，旋转角)
        // 命令格式：PS,a,45;
        float nowVal = (float)JY901.stcAngle.Angle[2] / 32768 * 180 + 180.0f;  // 获取当前航偏角,修正至[0, 360)
        Serial.println(nowVal);
        ml->Dir = (strtok(NULL, ","))[0];
        float delta = atoi(strtok(NULL, ";"));
        float exVal;

        if (ml->Dir == 'd') { // 假设逆时针和0点形成角为航偏角,范围为[0, 360)
          exVal = nowVal - delta;
          if (exVal < 0.0f) exVal += 360.0f;
        }
        else if (ml->Dir == 'a') {
          exVal = nowVal + delta;
          if (exVal > 360.0f) exVal -= 360.0f;
        }

        mpid->init(nowVal, exVal);  // 初始化pid计算单元

        while (true) {
          float Gyroz = (float)JY901.stcAngle.Angle[2] / 32768 * 180 + 180.0f;
          mpid->GyroRealize(Gyroz);
          mpid->out2Speed(ml);    // 将pid运算结果转化为移动控制参数
          Serial.println(mpid->out);
          ml->moveExec();   
          if (mpid->out < 3.0f) break;
          delay(10);
        }
      }
      else if (type == "SP") {  // set parameter: 设置(pid)参数
        // 命令格式：SP,3,0,0;
        mpid->Kp = atoi(strtok(NULL, ","));
        mpid->Ki = atoi(strtok(NULL, ","));
        mpid->Kd = atoi(strtok(NULL, ";"));
      }
    }
    else
    {
      cmd[cnt++] = c;
    }
  }
}

void setup() {
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);

  Serial.begin(115200);   // 是否会影响蓝牙串口?

  Serial.println("Hello!");
}

moveControl ml;
PID mpid(3.0f, 0, 0);
cmdInterpreter cr(&ml, &mpid);


void test1();
void test2();

void loop() {
  // test1();
  // test2();

  /*float nowVal = (float)JY901.stcAngle.Angle[2] / 32768 * 180 + 180.0f;
  Serial.println(nowVal);
  delay(500);*/

  cr.run();
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent()
{
  while (Serial.available())
  {
    JY901.CopeSerialData(Serial.read()); // Call JY901 data cope function
  }
}