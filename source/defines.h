#ifndef __DEFINES_H
#define __DEFINES_H

#include "gd32f30x.h"

#ifdef RELEASE
#define WDT_EN			1
#else
#define WDT_EN			0
#define TENS_TEST		0
#endif

#define TICK_CNT		10




extern unsigned char tick, tickCnt;
extern unsigned int secCnt, secTick, seconds;


#endif
