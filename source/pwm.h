#ifndef __PWM_H
#define __PWM_H

#include "gd32f30x.h"

#define HIGH_FREQ				300000

void timerInit(void);
void tmrSpwmNEn(unsigned char state);
void tmrGetTimer11Count(unsigned int *value);
void spwmEnable(void);
void spwmDisable(void);


extern uint16_t pwmPeriod;

#endif
