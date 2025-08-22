#ifndef __PRESCOUTPUT_H
#define __PRESCOUTPUT_H

#include "prescdata.h"
#include "carrier.h"

#define MAX_OUTPUT_DATA_COUNT       512

#define PRESCOUT_STOP               0
#define PRESCOUT_RUN                1

//wave data:bit7 0~data, 1~interval
typedef struct {
    unsigned short *data;
    unsigned short buffLen;
    unsigned short wIdx;
    unsigned short rIdx;
    unsigned short size;
} WaveBuff_t;

typedef struct {
    unsigned int ccnt;
    unsigned int cidx;
    unsigned int wcnt;
    unsigned int widx;

    unsigned short period;
    unsigned short maxAar;
    unsigned short amp;
    unsigned short base;
    unsigned short range;

    unsigned char step:4;
    unsigned char curStep:4;
    //unsigned char isWait:1;
    unsigned char isFsh:1;
    unsigned char times;

	unsigned char waveBuffFsh;
    unsigned char pwmBuffFsh;
    unsigned char pwmFsh;

    Wave_t *wave;
    WaveBuff_t *waveBuff;
} WaveOutput_t;

typedef struct {
    //unsigned char isWait:1;
    unsigned char isFsh:1;
    unsigned char count;
    unsigned char widx;
    unsigned char waitCnt;
    //unsigned short amp;
    WaveOutput_t wout;
    WaveGroup_t *group;
} WaveGroupOutput_t;

typedef struct {
    unsigned char state:1;
    //unsigned char run;
    unsigned char isFsh:1;
    unsigned char gIdx;
    //unsigned char wIdx;
    unsigned int freq;
    unsigned short period;

    unsigned char *prescData;
    unsigned short prescDataLen;
    unsigned char prescDataState;
    unsigned short prescDataIdx;
    unsigned short groupDataIdx;
    unsigned short groupDataLen;
    unsigned short waveDataIdx;
    unsigned short waveDataLen;
    Prescription_t prescription;
    WaveGroup_t group;
    Wave_t wave;
    WaveGroupOutput_t gout;
    Carrier_t *carrier;
} PrescOutput_t;

void prescoutInit(PrescOutput_t *ppout, Carrier_t *pc);
void prescoutSetPwmParams(PrescOutput_t *pout, unsigned int freq, unsigned short period);
void prescoutWaveBuffInit(PrescOutput_t *pout, WaveBuff_t *waveBuff);
void prescoutSetCarrier(PrescOutput_t *ppout, Carrier_t *pc);
void prescoutSetPrescData(PrescOutput_t *ppout, unsigned char *data, unsigned short len);
void prescoutProcess(PrescOutput_t *pout);
void prescoutStart(PrescOutput_t *pout);
void prescoutStop(PrescOutput_t *pout);

#endif
