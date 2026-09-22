#include "stm32f10x.h"
#include "LED.h"

/*
 * LED wiring (active-low):
 * 3.3V -> resistor -> LED anode(+)
 * LED cathode(-) -> MCU GPIO
 *
 * Red LED   cathode -> PC15
 * Green LED cathode -> PB9
 *
 * GPIO low  = LED ON
 * GPIO high = LED OFF
 */

static volatile uint8_t LED_Mode = MODE_STOP;
static volatile uint8_t LED_State = 0;

static void Red_LED_ON(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_15);
}

static void Red_LED_OFF(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_15);
}

static void Green_LED_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);
}

static void Green_LED_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
}

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC |
                           RCC_APB2Periph_TIM1,
                           ENABLE);

    /* PC15: red LED, use 2 MHz because PC13~PC15 are low-drive pins. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* PB9: green LED. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Active-low LEDs: output high means OFF. */
    Red_LED_OFF();
    Green_LED_OFF();

    /* TIM1 update every 0.5 s:
     * 72 MHz / 7200 = 10 kHz
     * 10 kHz / 5000 = 2 Hz -> one interrupt every 0.5 s
     */
    TIM_InternalClockConfig(TIM1);
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 5000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM1, ENABLE);
}

void LED_SetMode(uint8_t mode)
{
    if ((mode != MODE_STOP) &&
        (mode != MODE_BLUETOOTH) &&
        (mode != MODE_AUTO))
    {
        mode = MODE_STOP;
    }

    LED_Mode = mode;
    LED_State = 1;

    Red_LED_OFF();
    Green_LED_OFF();
    TIM_SetCounter(TIM1, 0);

    /* Turn the active mode LED on immediately, then TIM1 keeps blinking it. */
    if (LED_Mode == MODE_BLUETOOTH)
    {
        Green_LED_ON();
    }
    else if (LED_Mode == MODE_AUTO)
    {
        Red_LED_ON();
    }
}

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

        LED_State = !LED_State;

        if (LED_Mode == MODE_STOP)
        {
            Red_LED_OFF();
            Green_LED_OFF();
        }
        else if (LED_Mode == MODE_BLUETOOTH)
        {
            Red_LED_OFF();

            if (LED_State)
            {
                Green_LED_ON();
            }
            else
            {
                Green_LED_OFF();
            }
        }
        else if (LED_Mode == MODE_AUTO)
        {
            Green_LED_OFF();

            if (LED_State)
            {
                Red_LED_ON();
            }
            else
            {
                Red_LED_OFF();
            }
        }
    }
}
