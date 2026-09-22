#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/*
 * STM32F103C8T6 -> TB6612 wiring
 * PA1 -> PWMA
 * PA2 -> PWMB
 * PA4 -> AIN2
 * PA5 -> AIN1
 * PA6 -> BIN1
 * PA7 -> BIN2
 *
 * Channel A controls the LEFT side motors.
 * Channel B controls the RIGHT side motors.
 */

void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Start with all direction inputs low. */
    GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    PWM_Init();
    PWM_SetCompare2(0);
    PWM_SetCompare3(0);
}

void Motor_SetLeftSpeed(int8_t Speed)
{
    /* Left side = TB6612 channel A: AIN1=PA5, AIN2=PA4, PWMA=PA1 */
    if (Speed > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_5);      // AIN1 = 1
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);    // AIN2 = 0
        PWM_SetCompare2((uint16_t)Speed);
    }
    else if (Speed < 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);    // AIN1 = 0
        GPIO_SetBits(GPIOA, GPIO_Pin_4);      // AIN2 = 1
        PWM_SetCompare2((uint16_t)(-Speed));
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
        PWM_SetCompare2(0);
    }
}

void Motor_SetRightSpeed(int8_t Speed)
{
    /* Right side = TB6612 channel B: BIN1=PA6, BIN2=PA7, PWMB=PA2 */
    if (Speed > 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_6);      // BIN1 = 1
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);    // BIN2 = 0
        PWM_SetCompare3((uint16_t)Speed);
    }
    else if (Speed < 0)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);    // BIN1 = 0
        GPIO_SetBits(GPIOA, GPIO_Pin_7);      // BIN2 = 1
        PWM_SetCompare3((uint16_t)(-Speed));
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_7);
        PWM_SetCompare3(0);
    }
}
