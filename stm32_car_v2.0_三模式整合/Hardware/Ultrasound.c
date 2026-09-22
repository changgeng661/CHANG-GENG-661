#include "stm32f10x.h"
#include "Delay.h"
#include "Ultrasound.h"

/*
 * HC-SR04 wiring:
 * PB14 -> Trig
 * PB15 <- Echo
 * TIM4 is used only as a 1 us time base, so it does not occupy TIM4 GPIO pins.
 */
void Ultrasound_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* PB14: Trig output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    /* PB15: Echo input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* TIM4: 72 MHz / 72 = 1 MHz, one count = 1 us */
    TIM_InternalClockConfig(TIM4);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    TIM_Cmd(TIM4, DISABLE);
    TIM_SetCounter(TIM4, 0);
}

uint16_t Test_Distance(void)
{
    uint16_t pulse_us;
    uint32_t distance_cm;

    /* Send at least a 10 us trigger pulse. */
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    Delay_us(2);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    Delay_us(15);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    /* Wait for Echo to go high. Timeout prevents the MCU from getting stuck. */
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
    {
        if (TIM_GetCounter(TIM4) >= 30000)
        {
            TIM_Cmd(TIM4, DISABLE);
            TIM_SetCounter(TIM4, 0);
            return 0xFFFF;
        }
    }

    /* Measure the Echo high pulse width in microseconds. */
    TIM_SetCounter(TIM4, 0);
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == SET)
    {
        if (TIM_GetCounter(TIM4) >= 30000)
        {
            TIM_Cmd(TIM4, DISABLE);
            TIM_SetCounter(TIM4, 0);
            return 0xFFFF;
        }
    }

    pulse_us = TIM_GetCounter(TIM4);
    TIM_Cmd(TIM4, DISABLE);
    TIM_SetCounter(TIM4, 0);

    /* distance(cm) = time(us) * 0.034 / 2 = time(us) * 17 / 1000 */
    distance_cm = ((uint32_t)pulse_us * 17U) / 1000U;

    if (distance_cm > 500U)
    {
        return 0xFFFF;
    }

    return (uint16_t)distance_cm;
}
