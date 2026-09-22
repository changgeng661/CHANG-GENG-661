#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetCompare2(uint16_t Compare);   // PA1 -> TIM2_CH2 -> PWMA
void PWM_SetCompare3(uint16_t Compare);   // PA2 -> TIM2_CH3 -> PWMB

#endif
