#ifndef __PWMSERVO_H
#define __PWMSERVO_H

#include "stm32f10x.h"

void PWMServo_Init(void);
void PWMServo_SetCompare4(uint16_t Compare);

#endif
