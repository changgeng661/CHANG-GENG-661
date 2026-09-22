#include "stm32f10x.h"
#include "Delay.h"
#include "Car.h"
#include "Serial.h"
#include "Ultrasound.h"
#include "Servo.h"
#include "LED.h"

/* Automatic obstacle avoidance parameters. */
#define OBSTACLE_DISTANCE_CM    15U
#define TURN_TIME_MS            700U
#define BACK_TIME_MS            500U

/* USART1 interrupt data. */
volatile uint8_t Bluetooth_RxData = 0;
volatile uint8_t Bluetooth_RxFlag = 0;
volatile uint8_t Mode_Request = 0;

/* Power-on default: mode 1, stopped. */
static uint8_t Car_Mode = MODE_STOP;
static uint8_t Servo_Started = 0;

static void Set_Car_Mode(uint8_t New_Mode)
{
    if ((New_Mode != MODE_STOP) &&
        (New_Mode != MODE_BLUETOOTH) &&
        (New_Mode != MODE_AUTO))
    {
        return;
    }

    if (New_Mode == Car_Mode)
    {
        return;
    }

    /* Always stop the motors first when switching modes. */
    Car_Stop();

    /* Clear any old movement command so it cannot run in the new mode. */
    Bluetooth_RxFlag = 0;
    Bluetooth_RxData = 0;

    Car_Mode = New_Mode;
    LED_SetMode(Car_Mode);

    /* Important: the servo is NOT initialized at power-on.
     * It only starts when mode 3 is selected, avoiding startup movement.
     */
    if (Car_Mode == MODE_AUTO)
    {
        if (Servo_Started == 0)
        {
            Servo_Init();
            Servo_Started = 1;
        }

        Servo_SetAngle(90);
        Delay_ms(500);
    }
}

static void Bluetooth_Control(void)
{
    uint8_t Command;

    if (Bluetooth_RxFlag == 0)
    {
        return;
    }

    Command = Bluetooth_RxData;
    Bluetooth_RxFlag = 0;

    /* Movement commands are valid ONLY in mode 2. */
    if (Car_Mode != MODE_BLUETOOTH)
    {
        return;
    }

    switch (Command)
    {
        case 'w':
        case 'W':
            Go_Ahead();
            break;

        case 's':
        case 'S':
            Go_Back();
            break;

        case 'a':
        case 'A':
            Turn_Left();
            break;

        case 'd':
        case 'D':
            Turn_Right();
            break;

        case '0':
            Car_Stop();
            break;

        default:
            /* Ignore CR/LF and other characters. */
            break;
    }
}

static void Auto_Avoidance(void)
{
    uint16_t front_distance;
    uint16_t right_distance;
    uint16_t left_distance;

    /* Look forward first. */
    Servo_SetAngle(90);
    Delay_ms(100);
    front_distance = Test_Distance();

    /* Measurement failed: stop for safety. */
    if (front_distance == 0xFFFF)
    {
        Car_Stop();
        Delay_ms(100);
        return;
    }

    /* Front is clear: keep moving forward. */
    if (front_distance >= OBSTACLE_DISTANCE_CM)
    {
        Go_Ahead();
        Delay_ms(60);
        return;
    }

    /* Front blocked: stop and check the right side. */
    Car_Stop();
    Delay_ms(200);

    Servo_SetAngle(0);
    Delay_ms(500);
    right_distance = Test_Distance();

    if ((right_distance != 0xFFFF) &&
        (right_distance >= OBSTACLE_DISTANCE_CM))
    {
        Servo_SetAngle(90);
        Delay_ms(200);

        Self_Right();
        Delay_ms(TURN_TIME_MS);

        Car_Stop();
        Delay_ms(200);
        return;
    }

    /* Right blocked: check the left side. */
    Servo_SetAngle(180);
    Delay_ms(500);
    left_distance = Test_Distance();

    if ((left_distance != 0xFFFF) &&
        (left_distance >= OBSTACLE_DISTANCE_CM))
    {
        Servo_SetAngle(90);
        Delay_ms(200);

        Self_Left();
        Delay_ms(TURN_TIME_MS);

        Car_Stop();
        Delay_ms(200);
        return;
    }

    /* Dead end: front/right/left are all blocked.
     * Back up for 0.5 s, stop, then the next loop measures again.
     */
    Servo_SetAngle(90);
    Car_Stop();
    Delay_ms(200);

    Go_Back();
    Delay_ms(BACK_TIME_MS);

    Car_Stop();
    Delay_ms(300);
}

int main(void)
{
    /* Initialize devices that are safe at power-on. */
    LED_Init();
    Car_Init();
    Serial_Init();
    Ultrasound_Init();

    /* Do NOT call Servo_Init() here.
     * Power-on state is mode 1: car stopped, both mode LEDs off.
     */
    Car_Mode = MODE_STOP;
    Car_Stop();
    LED_SetMode(MODE_STOP);

    while (1)
    {
        /* Mode commands have the highest priority:
         * '1' = stop mode
         * '2' = Bluetooth manual mode
         * '3' = automatic obstacle avoidance mode
         */
        if (Mode_Request != 0)
        {
            uint8_t Requested_Mode = Mode_Request;
            Mode_Request = 0;
            Set_Car_Mode(Requested_Mode);
        }

        if (Car_Mode == MODE_STOP)
        {
            Car_Stop();
        }
        else if (Car_Mode == MODE_BLUETOOTH)
        {
            Bluetooth_Control();
        }
        else if (Car_Mode == MODE_AUTO)
        {
            /* In mode 3, w/a/s/d/0 movement commands are ignored. */
            Bluetooth_RxFlag = 0;
            Auto_Avoidance();
        }
    }
}

/* USART1 receive interrupt: only stores commands; no long work here. */
void USART1_IRQHandler(void)
{
    uint8_t Data;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        Data = (uint8_t)USART_ReceiveData(USART1);

        /* 1/2/3 select the global working mode. */
        if ((Data == '1') || (Data == '2') || (Data == '3'))
        {
            Mode_Request = (uint8_t)(Data - '0');
        }
        else
        {
            /* Other bytes are movement commands for mode 2. */
            Bluetooth_RxData = Data;
            Bluetooth_RxFlag = 1;
        }
    }
}
