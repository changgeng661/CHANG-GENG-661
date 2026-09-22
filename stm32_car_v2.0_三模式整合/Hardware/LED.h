#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define MODE_STOP        1U
#define MODE_BLUETOOTH   2U
#define MODE_AUTO        3U

void LED_Init(void);
void LED_SetMode(uint8_t mode);

#endif
