#include "stm32f10x.h"
#include "PWMServo.h"
#include "Servo.h"

void Servo_Init(void)
{
    PWMServo_Init();
}

void Servo_SetAngle(uint16_t Angle)
{
    uint16_t pulse;

    if (Angle > 180U)
    {
        Angle = 180U;
    }

    /* 0 deg -> 500 us, 90 deg -> 1500 us, 180 deg -> 2500 us. */
    pulse = (uint16_t)(((uint32_t)Angle * 2000U) / 180U + 500U);
    PWMServo_SetCompare4(pulse);
}
