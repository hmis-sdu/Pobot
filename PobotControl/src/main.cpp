#include <Arduino.h>

// 引脚定义
#define PWMA0 5
#define PWMA1 6
#define PWMB0 10
#define PWMB1 11

// int PwmA, PwmB;

const float Pi = 3.1415926;

/**********  移动控制方案  **********
1. 例程摇杆控制（三挡调速）
2. 通用控制：
命令格式： GMC,direction,speed;
参数说明： direction = d/w/a/s
         speed = [0, 255]
**********  移动控制方案  **********/

/**********  云台控制方案  **********

通用控制：
命令格式： CC,direction;
参数说明： direction = d/w/a/s
**********  云台控制方案  **********/

/******** 移动控制参数 **********/
// 三挡调速
const int lowSpeed = 100;
const int midSpeed = 140;
const int highSpeed = 180;
char Dir;  // 方向
int Speed; // 速度大小
/******** 移动控制参数 **********/

/**********  命令解释器  **********/
char Cmd[50];
String Type;         // 命令类型
int Cnt;
bool valid;          // 命令是否有效
float X, Y;          // 控制参数

const String CJ = "CJ";      // control by joystick : 使用摇杆控制
const String GMC = "GMC";    // general move control : 通用移动控制
const String CS = "CS";      // automatic stop : 自动停止控制
const String CC = "CC";      // console control : 云台控制
/**********  命令解释器  **********/

/**********  工作函数  **********
work(): 通讯、命令解析
Trans*(): 控制参数归一化
Exec(): 执行单元
**********  工作函数  **********/

void setup()
{
  // 初始化
  pinMode(PWMA0, OUTPUT); // A电机正向PWM
  pinMode(PWMA1, OUTPUT); // A电机反向PWM
  pinMode(PWMB0, OUTPUT); // B电机正向PWM
  pinMode(PWMB1, OUTPUT); // B电机反向PWM

  analogWrite(PWMA0, 0);
  analogWrite(PWMA1, 0);
  analogWrite(PWMB0, 0);
  analogWrite(PWMB1, 0);

  Serial.begin(9600);
}

void Trans()
{
  float x = X, y = Y;
  X = x * cos(Pi / 4) - y * sin(Pi / 4);
  Y = y * cos(Pi / 4) + x * sin(Pi / 4);

  if (X > 0 && Y > 0)
  { // 右转
    Dir = 'd';
  }
  else if (X < 0 && Y > 0)
  { // 前进
    Dir = 'w';
  }
  else if (X < 0 && Y < 0)
  { // 左转
    Dir = 'a';
  }
  else if (X > 0 && Y < 0)
  { // 后退
    Dir = 's';
  }

  float Val = X * X + Y * Y;
  if (Val > 0.9)
  {
    Speed = highSpeed;
  }
  else if (Val <= 0.9 && Val > 0.3)
  {
    Speed = midSpeed;
  }
  else
  {
    Speed = lowSpeed;
  }
}

void Exec()
{
  if (Dir == 'd')
  { // 右转
    analogWrite(PWMB0, Speed * 0.75);
    analogWrite(PWMA1, Speed * 0.75);
  }
  else if (Dir == 'w')
  { // 前进
    analogWrite(PWMA0, Speed);
    analogWrite(PWMB0, Speed);
  }
  else if (Dir == 'a')
  { // 左转
    analogWrite(PWMA0, Speed * 0.75);
    analogWrite(PWMB1, Speed * 0.75);
  }
  else if (Dir == 's')
  { // 后退
    analogWrite(PWMA1, Speed);
    analogWrite(PWMB1, Speed);
  }
}

void work()
{
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == ';')
    {
      Cmd[Cnt] = c;
      Cnt = 0;
      Serial.println(c);

      Type = strtok(Cmd, ",");
      // Serial.println(Type);
      if (Type == CJ)
      {
        X = atof(strtok(NULL, ","));
        Y = atof(strtok(NULL, ";"));
        valid = true;
        Trans();
        if (valid)
          Exec();
      }
      else if (Type == GMC)
      {
        Dir = (strtok(NULL, ","))[0];
        Speed = atoi(strtok(NULL, ";"));

        // Serial.print("D = ");
        // Serial.println(Dir);
        // Serial.print("V = ");
        // Serial.println(Speed);

        Exec();
      }
      else if (Type == CS)
      {
        analogWrite(PWMA0, 0);
        analogWrite(PWMA1, 0);
        analogWrite(PWMB0, 0);
        analogWrite(PWMB1, 0);
      }
      else
      {
        valid = false;
        // Serial.println("invalid");
      }
    }
    else
    {
      Cmd[Cnt++] = c;
      Serial.print(c);
    }
  }
}

void test()
{
  analogWrite(PWMA0, midSpeed);
  analogWrite(PWMB0, midSpeed);
  delay(1000);
  analogWrite(PWMA0, 0);
  analogWrite(PWMB0, 0);
  delay(1000);
  analogWrite(PWMA1, midSpeed);
  analogWrite(PWMB1, midSpeed);
  delay(1000);
  analogWrite(PWMA1, 0);
  analogWrite(PWMB1, 0);

  // GMC,a,100;
  // GMC,w,200;
}

void loop()
{
  // test();
  work();
}