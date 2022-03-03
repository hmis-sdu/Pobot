
//引脚定义
#define PWMA 5
#define PWMB 10

int PwmA, PwmB;
double V;

const float Pi = 3.1415926;

/**********  控制方案  **********
1. 四种基本控制： 前后左右；
2. 三挡调速： 低中高；

**********  控制方案  **********/

const int lowSpeed = 50;
const int midSpeed = 100;
const int highSpeed = 200;

char Dir;
int Speed;

/**********  命令解释器  **********

**********  命令解释器  **********/
char Cmd[50];
String Type;    // 命令类型
int Cnt, noData;
bool valid;     // 命令是否有效
float X, Y;     // 控制参数

const String CJ = "CJ";
const String CM = "CM";

/**********  工作函数  **********
work(): 通讯、命令解析
Trans*(): 控制参数归一化
Exec(): 执行单元
**********  工作函数  **********/


void setup() {
  // 初始化
  pinMode(PWMA, OUTPUT);    // A电机PWM
  pinMode(PWMB, OUTPUT);    // B电机PWM
  
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  
  Serial.begin(9600);
}

void loop() 
{
//  test();
  work();
}

void work() {
  if (Serial.available()) {
    noData = 0;
    char c = Serial.read();
    if (c == ';') {
      Cmd[Cnt] = c;
      Cnt = 0;
      Serial.println(c);
      
      Type = strtok(Cmd, ",");
//      Serial.println(Type);
      if (Type == CJ) {
        X = atof(strtok(NULL, ","));
        Y = atof(strtok(NULL, ";"));
//        Serial.println(X);
//        Serial.println(Y);
        valid = true;
        Trans();
        if (valid) Exec();
      }
      else if (Type == CM) {
        // X: picth Y: roll
        X = atof(strtok(NULL, ","));
        Y = atof(strtok(NULL, ";"));
        valid = true;
//        Trans2();
//        if (valid) Exec();
      }
      else {
        valid = false;
//        Serial.println("invalid");
      }
    }
    else {
      Cmd[Cnt++] = c;
      Serial.print(c);
    }
  }
  else {
    if (noData > 1.2e4) {
      analogWrite(PWMA, 0);
      analogWrite(PWMB, 0);
      noData = 0;      
    }
    else noData++;
  }
}

void Trans() {
  float x = X, y = Y;
  X = x * cos(Pi / 4) - y * sin(Pi / 4);
  Y = y * cos(Pi / 4) + x * sin(Pi / 4);

  if (X > 0 && Y > 0) {       // 右转
    Dir = 'd';
  }
  else if (X < 0 && Y > 0) {  // 前进
    Dir = 'w';
  }
  else if (X < 0 && Y < 0) {  // 左转
    Dir = 'a';
  }
  else if (X > 0 && Y < 0) {  // 后退
    Dir = 's';
  }

  float Val = X * X + Y * Y;
  if (Val > 0.9) {
    Speed = highSpeed;
  }
  else if (Val <= 0.9 && Val > 0.3){
    Speed = midSpeed;
  }
  else {
    Speed = lowSpeed;
  }
}

void Exec() {
  if (Dir == 'd') {
    analogWrite(PWMB, Speed);
  }
  else if (Dir == 'w') {
    analogWrite(PWMA, Speed);
    analogWrite(PWMB, Speed);
  }
  else if (Dir == 'a') {
    analogWrite(PWMA, Speed);
  }
  else if (Dir == 's') {
    
  }
  
//  delay(10);
//  analogWrite(PWMA, 0);
//  analogWrite(PWMB, 0);
}

void test() {
  analogWrite(PWMA, midSpeed);
  analogWrite(PWMB, midSpeed);
}
