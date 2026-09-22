# STM32 Smart Car v2.0 - Three Mode Integrated Robot Car

基于 **STM32F103C8T6** 的三模式智能小车项目，集成了 **手动蓝牙控制、自动避障、基础运动控制** 等功能。

本项目采用模块化 C 语言开发，将底层硬件驱动与上层控制逻辑分离，适合作为 STM32 入门学习、嵌入式开发实践以及智能车项目开发参考。

## ✨ Project Features

### 🚗 三种工作模式

#### Mode 1：待机模式 (STOP)

* 上电默认进入停止状态
* 电机关闭，等待模式切换
* LED 显示当前状态

#### Mode 2：蓝牙遥控模式 (Bluetooth Control)

通过串口蓝牙模块接收控制指令，实现无线遥控：

| Command | Function   |
| ------- | ---------- |
| W / w   | Forward    |
| S / s   | Backward   |
| A / a   | Turn Left  |
| D / d   | Turn Right |
| 0       | Stop       |

支持实时控制小车前进、后退、转向和停止。

#### Mode 3：自动避障模式 (Auto Avoidance)

利用超声波模块 + 舵机扫描环境，实现自主移动：

* 前方距离检测
* 左右方向环境扫描
* 自动选择可通行方向
* 遇到障碍自动转向
* 死路情况下自动后退

避障逻辑：

```
          Front
            |
            v
     Measure Distance
            |
    -------------------
    |                 |
 Clear             Blocked
    |                 |
 Move Forward   Scan Left/Right
                      |
              Select Direction
                      |
                  Turn / Back
```

---

## 🛠 Hardware Configuration

### Main Controller

* STM32F103C8T6 Minimum System Board

### Motor Driver

* TB6612FNG Dual Motor Driver

PWM Mapping:

```
PA1  -> PWMA (TIM2_CH2)
PA2  -> PWMB (TIM2_CH3)

PA4  -> AIN2
PA5  -> AIN1
PA6  -> BIN1
PA7  -> BIN2
```

### Modules

| Module             | Function             |
| ------------------ | -------------------- |
| TB6612FNG          | Motor drive          |
| HC-05 Bluetooth    | Wireless control     |
| HC-SR04 Ultrasonic | Distance measurement |
| Servo Motor        | Sensor scanning      |
| OLED Display       | Status display       |
| LED Indicator      | Mode indication      |

---

## 📂 Project Structure

```
stm32_car_v2.0
│
├── Hardware
│   ├── Motor.c          # Motor driver
│   ├── Car.c            # Vehicle movement control
│   ├── PWM.c            # PWM generation
│   ├── Servo.c          # Servo control
│   ├── Ultrasound.c     # Ultrasonic sensor
│   ├── Serial.c         # Bluetooth UART
│   ├── OLED.c           # OLED display
│   └── LED.c            # Status indicator
│
├── Library
│   └── STM32 Standard Peripheral Library
│
├── User
│   └── main.c           # Main control program
│
└── Objects
    └── Keil build files
```

---

## ⚙️ Software Environment

* MCU: STM32F103C8T6
* IDE: Keil MDK
* Language: C
* Framework: STM32 Standard Peripheral Library

---

## 🔧 Improvements Compared With Previous Version

* ✅ Added dual PWM motor control using TIM2_CH2 and TIM2_CH3
* ✅ Fixed TB6612 dual-channel motor speed control
* ✅ Added complete three-mode switching system
* ✅ Added Bluetooth command filtering
* ✅ Added servo initialization protection
* ✅ Improved automatic obstacle avoidance algorithm
* ✅ Optimized motor stop behavior during mode switching

---

## 🚀 Future Improvements

Possible extensions:

* Add PID speed control
* Add encoder feedback
* Add WiFi / ESP8266 remote control
* Add camera vision recognition
* Add RTOS multitasking support
* Add mobile APP controller

---

## 📌 Project Purpose

This project demonstrates the application of embedded systems including:

* GPIO control
* PWM motor control
* UART communication
* Timer peripherals
* Ultrasonic ranging
* Servo control
* Modular embedded software design

Suitable for STM32 learning, embedded development practice, and intelligent vehicle projects.

---

## License

This project is for learning and educational purposes.
