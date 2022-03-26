# Pobot
手机机器人arduino下位机源码以及安卓上位机示例APP

#### 控制APP：[https://github.com/hmis-sdu/Pobot](https://github.com/hmis-sdu/Pobot)
1. 里面 BluetoothChatService 用安卓原生API实现了蓝牙通讯，可以参考 MainActivity 和 JoystickView 如何使用；
2. 命令可以用 string 构造，发送前要使用 getbytes() 方法转成字节流。

#### 命令
#### 1. 小车移动通用控制
命令格式：GMC,direction,speed;
参数范围及说明: 
1. GMC: general move control
2. direction = d/w/a/s
右转/前进/左转/后退，为了方便记，看键盘
3. speed = [0, 255]
speed = 0 就是停止，速度改变太快小车可能摔倒！

注意参数之间使用逗号分隔，命令结束是分号。
#### 2. 云台通用控制
命令格式：CC,direction;
参数范围及说明: 
1. CC: console control
2. direction = d/w/a/s
