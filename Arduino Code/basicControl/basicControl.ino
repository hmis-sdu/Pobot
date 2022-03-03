
//引脚定义
#define PWMA 3        // 3为模拟引脚，用于PWM控制
#define AIN1 5
#define AIN2 4
#define PWMB 10       // 10为模拟引脚，用于PWM控制
#define BIN1 8
#define BIN2 9
#define STBY 7        // 2、4、8、12、7为数字引脚，用于开关量控制
#define Voltage A0    // 使用模拟引脚

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
int Cnt;
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
  // TB6612电机驱动模块控制信号初始化
  pinMode(AIN1, OUTPUT);    // 控制电机A的方向，(AIN1, AIN2)=(1, 0)为正转，(AIN1, AIN2)=(0, 1)为反转
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);    // 控制电机B的方向，(BIN1, BIN2)=(0, 1)为正转，(BIN1, BIN2)=(1, 0)为反转
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);    // A电机PWM
  pinMode(PWMB, OUTPUT);    // B电机PWM
  pinMode(STBY, OUTPUT);    // TB6612FNG使能, 置0则所有电机停止, 置1才允许控制电机
  
  // 初始化TB6612电机驱动模块
  digitalWrite(AIN1, 1);
  digitalWrite(AIN2, 0);
  digitalWrite(BIN1, 1);
  digitalWrite(BIN2, 0);
  digitalWrite(STBY, 1);
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  // 初始化串口，用于输出电池电压
  Serial.begin(9600);
  pinMode(Voltage,INPUT);  // 初始化作为输入端
}

/**************************************************************************
函数功能：设置指定电机转速
入口参数：指定电机motor，motor=1（2）代表电机A（B）； 指定转速pwm，大小范围为0~255，代表停转和满速
返回值：无
**************************************************************************/
void SetPWM(int motor, int pwm)
{
  if(motor == 1 && pwm >= 0)        // 电机A正转
  {
    digitalWrite(AIN1, 1);
    digitalWrite(AIN2, 0);
    analogWrite(PWMA, pwm);
  }
  else if(motor == 1 && pwm < 0)    // 电机A反转
  {
    digitalWrite(AIN1, 0);
    digitalWrite(AIN2, 1);
    analogWrite(PWMA, -pwm);
  }
  else if(motor == 2 && pwm >= 0)   // 电机B正转
  {
    digitalWrite(BIN1, 0);
    digitalWrite(BIN2, 1);
    analogWrite(PWMB, pwm);
  }
  else if(motor == 2 && pwm < 0)    // 电机B反转
  {
    digitalWrite(BIN1, 1);
    digitalWrite(BIN2, 0);
    analogWrite(PWMB, -pwm);
  }
}

void loop() 
{
//  test();
  work();
}

void work() {
  if (Serial.available()) {
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
    SetPWM(1, -Speed);
    SetPWM(2, -Speed);
  }
  else if (Dir == 'w') {
    SetPWM(1, +Speed);
    SetPWM(2, -Speed);
  }
  else if (Dir == 'a') {
    SetPWM(1, +Speed);
    SetPWM(2, +Speed);
  }
  else if (Dir == 's') {
    SetPWM(1, -Speed);
    SetPWM(2, +Speed);
  }
  
  delay(300);
  SetPWM(1, 0);
  SetPWM(2, 0);
}

void test() {
  SetPWM(1, 255);             // 电机AB同时满速正转
  SetPWM(2, 255);
  V = analogRead(Voltage);      // 读取模拟引脚A0模拟量
  Serial.print(V*0.05371);    // 对模拟量转换并通过串口输出
  Serial.println("V");
  delay(500);                 // 正转3s
  
  SetPWM(1, 0);               // 电机AB停止
  SetPWM(2, 0);
  delay(1000);                // 停止1s
  
  SetPWM(1, 128);             // 电机AB同时半速正转
  SetPWM(2, 128);
  delay(3000);                // 半速正转3s
  
  SetPWM(1, 0);               // 电机AB停止
  SetPWM(2, 0);
  delay(1000);                // 停止1s
  
  SetPWM(1, -255);            // 电机AB同时满速反转
  SetPWM(2, -255);
  delay(3000);                // 反转3s
  
  SetPWM(1, 0);               // 电机AB停止
  SetPWM(2, 0);
  delay(1000);                // 停止1s
  
  SetPWM(1, 255);             // 电机A满速正转
  SetPWM(2, -255);            // 电机B满速反转
  delay(3000);                // 持续3s
  
  SetPWM(1, 0);               // 电机AB停止
  SetPWM(2, 0);
  delay(1000);                // 停止1s
}
