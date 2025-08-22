#ifndef __RTDATAOUTPUT_H
#define __RTDATAOUTPUT_H

#include "carrier.h"

#define RTOUTSTAT_STOP              0
#define RTOUTSTAT_RUN               1

//typedef struct {

//} RtBuff_t;

typedef struct {
    unsigned char state;
    unsigned int pwmFreq;
    unsigned short period;
    unsigned int freq;
    unsigned short amp;

    unsigned int sampleTime;            //sampling time(ms)
    unsigned int count;                 //count of carrier
    unsigned int sidx;                  //index of carrier
    Carrier_t *carrier;
} RtDataOutput_t;

void rtoutInit(RtDataOutput_t *rout, Carrier_t *pc);
void rtoutSetPwmParams(RtDataOutput_t *rout, unsigned int freq, unsigned short period);
void rtoutSetFreq(RtDataOutput_t *rout, unsigned int freq);
void rtoutSetAmp(RtDataOutput_t *rout, unsigned short amp);
void rtoutSetSamplingTime(RtDataOutput_t *rout, unsigned int time);
void rtoutProcess(RtDataOutput_t *rout);
void rtoutStart(RtDataOutput_t *rout);
void rtoutStop(RtDataOutput_t *rout);
#endif
