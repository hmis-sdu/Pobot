# Pobot
手机机器人arduino下位机源码以及安卓上位机示例APP

## 面向对象架构
![架构图](https://github.com/hmis-sdu/Pobot/blob/main/image/structure%20chart.png)

#### 控制APP：[https://github.com/hmis-sdu/Pobot/tree/main/BluetoothTouch](https://github.com/hmis-sdu/Pobot/tree/main/BluetoothTouch)
1. 里面 BluetoothChatService 用安卓原生API([https://developer.android.google.cn/guide/topics/connectivity/bluetooth](https://developer.android.google.cn/guide/topics/connectivity/bluetooth))实现了蓝牙通讯服务，可以参考 MainActivity 和 JoystickView 如何使用；
2. 命令可以用 string 构造，发送前要使用 getbytes() 方法转成字节流;
3. 项目主要提供手机蓝牙参考，通过安卓手机如何发送命令，同时，只实现了 client 端，只能主动连接其它设备。

#### 命令
#### 1. 小车移动通用控制
* 命令格式：GMC,direction,speed;
* 参数范围及说明: 
  1. GMC: general move control
  2. direction = d/w/a/s
右转/前进/左转/后退，为了方便记，看键盘
  3. speed = [0, 255]
speed = 0 就是停止，速度改变太快小车可能摔倒！

注意参数之间使用逗号分隔，命令结束是分号。

**之前小车可以自动停止，现在需要发送停止命令！**
##### 自动停止命令
* 命令格式： CS,;
* 作用等价于 GMC,a,0;

可以在不控制小车运动时，不断发送该命令。

#### 2. 云台通用控制
* 命令格式：CC,direction;
* 参数范围及说明: 
  1. CC: console control
  2. direction = d/w/a/s


