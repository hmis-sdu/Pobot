#include <Arduino.h>

/*
3.28 
-添加抢占式移动优化（untested）
-添加云台控制（untested）
*/


// 抢占式移动优化
#define PREEMPTIVEOPTIMIZATION 1

#define MAXSPEED 255
#define MINSPEED 0

// 引脚定义
#define PIN1 2
#define PIN2 4
#define PWMA0 5
#define PWMA1 6
#define PWMB0 10
#define PWMB1 11

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
int Speed; // 当前速度
/******** 移动控制参数 **********/

/******** 云台控制参数 **********/
int myangle;      // 角度变量
int pulsewidth;   // 脉宽变量
int Serv;         // 使能舵机
int rot;          // 使能舵机运动方向
int Pos1, Pos2;   // 舵机当前位置
int tarPos;       // 舵机目标位置
int oneStep;      // 最小步
const int IdealPos1 = 65;
const int IdealPos2 = 60;
/******** 云台控制参数 **********/

/******** 抢占式移动优化 **********/
int tarSpeed;   // 目标速度
const int accel = 5;
/******** 抢占式移动优化 **********/

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
Trans*(): 控制参数归一化（考虑移至上位机）
moveExec(): 移动执行单元
cloudExec(): 云台执行单元
servopulse(): 数字信号转为模拟信号（使用数字引脚输出 PWM 信号）
**********  工作函数  **********/

void servopulse(int, int);

void setup()
{
  // 初始化
  pinMode(PWMA0, OUTPUT);   // A电机正向PWM
  pinMode(PWMA1, OUTPUT);   // A电机反向PWM
  pinMode(PWMB0, OUTPUT);   // B电机正向PWM
  pinMode(PWMB1, OUTPUT);   // B电机反向PWM
  pinMode(PIN1, OUTPUT);    // 1号舵机控制朝向
  pinMode(PIN2, OUTPUT);    // 1号舵机控制俯仰

  analogWrite(PWMA0, 0);
  analogWrite(PWMA1, 0);
  analogWrite(PWMB0, 0);
  analogWrite(PWMB1, 0);

  Pos1 = IdealPos1, Pos2 = IdealPos2;

  oneStep = 10;

  for (int i = 0; i <= 75; i++) {
    servopulse(PIN1, Pos1);
    servopulse(PIN2, Pos2);  
  }
  Serial.begin(9600);
}

void servopulse(int Pin, int myangle) {
  pulsewidth = myangle * 11 + 500;  // 将角度转化为 500 ~ 2480 的脉宽值
  digitalWrite(Pin, HIGH);
  delayMicroseconds(pulsewidth);    // 延时脉宽值的微妙数
  digitalWrite(Pin, LOW);
  delay(20 - pulsewidth / 1000);
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
    tarSpeed = highSpeed;
  }
  else if (Val <= 0.9 && Val > 0.3)
  {
    tarSpeed = midSpeed;
  }
  else
  {
    tarSpeed = lowSpeed;
  }
}

void cloudExec() {
  if (Dir == 'd') Serv = PIN1, rot = 1;
  else if (Dir == 'w') Serv = PIN2, rot = 1;
  else if (Dir == 'a') Serv = PIN1, rot = -1; 
  else if (Dir == 's') Serv = PIN2, rot = -1;

  if (Serv == PIN1) {
    Pos1 += oneStep * rot;
    tarPos = Pos1;
  }
  else if (Serv == PIN2) {
    Pos2 += oneStep * rot;
    tarPos = Pos2;
  }

  for (int i = 0; i < 5; i++) {
    servopulse(Serv, tarPos);
  }
}

void moveExec()
{

  // Speed = Speed + (tarSpeed - Speed) / 2;

  if (PREEMPTIVEOPTIMIZATION) {
    if (Speed < tarSpeed) {
      Speed += accel;
      Speed = min(Speed, MAXSPEED);
    }
    else if (Speed > tarSpeed) {
      Speed -= accel;
      Speed = max(Speed, MINSPEED);
    }    
  }
  else {
    Speed = tarSpeed;
  }


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
          moveExec();
      }
      else if (Type == GMC)
      {
        Dir = (strtok(NULL, ","))[0];
        tarSpeed = atoi(strtok(NULL, ";"));

        // Serial.print("D = ");
        // Serial.println(Dir);
        // Serial.print("V = ");
        // Serial.println(Speed);

        moveExec();
      }
      else if (Type == CC) {
        Dir = (strtok(NULL, ";"))[0];
        cloudExec();
      }
      else if (Type == CS)
      {
        // analogWrite(PWMA0, 0);
        // analogWrite(PWMA1, 0);
        // analogWrite(PWMB0, 0);
        // analogWrite(PWMB1, 0);
        tarSpeed = 0;
        moveExec();
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