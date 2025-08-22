#ifndef __TENSOUTPUT_H
#define __TENSOUTPUT_H

#include "prescapi.h"


#define TENS_OUT_TYPE_PRESC             0
#define TENS_OUT_TYPE_RT                1

#define TENS_OUT_STAT_STOP              0
#define TENS_OUT_STAT_RUN               1
#define TENS_OUT_STAT_PAUSE             2


#define WBUFF_SIZE      2048

typedef struct {
    unsigned char state;
    unsigned char outType;
    RtDataOutput_t rtDataOutput;
    PrescOutput_t prescOutput;
    PwmOutput_t pwmOutput;
} TensOutput_t;

void tensoutInit(TensOutput_t * pt);
void tensoutProcess(TensOutput_t *pt);
void tensoutSetPrescription(TensOutput_t *pt, unsigned char *data, unsigned short len);
unsigned short tensoutGetPrescription(TensOutput_t *pt);
void tensoutStartOutput(TensOutput_t *pt);
void tensoutStopOutput(TensOutput_t *pt);
void tensoutPauseOutput(TensOutput_t *pt);

#endif
