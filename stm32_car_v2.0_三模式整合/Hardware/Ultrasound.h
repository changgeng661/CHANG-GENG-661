#ifndef __ULTRASOUND_H
#define __ULTRASOUND_H

#include "stm32f10x.h"

/*
 * HC-SR04 wiring used by this project:
 * PB14 -> Trig
 * PB15 <- Echo
 */
void Ultrasound_Init(void);
uint16_t Test_Distance(void);

#endif
