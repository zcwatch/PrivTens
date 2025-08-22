#include "rtdataoutput.h"


void rtoutInit(RtDataOutput_t *rout, Carrier_t *pc)
{
    rout->carrier = pc;
    rout->state = RTOUTSTAT_STOP;
}

void rtoutSetPwmParams(RtDataOutput_t *rout, unsigned int freq, unsigned short period)
{
    rout->pwmFreq = freq;
    rout->period = period;
    carrierSetPwmParams(rout->carrier, freq, period);
}

void rtoutSetFreq(RtDataOutput_t *rout, unsigned int freq)
{
    rout->freq = freq;
    rout->count = rout->sampleTime * freq / 1000;
    carrierSetFreq(rout->carrier, freq);
}

void rtoutSetAmp(RtDataOutput_t *rout, unsigned short amp)
{
    rout->amp = amp;
    rout->sidx = 0;
    carrierSetAmp(rout->carrier, amp);
}

void rtoutSetSamplingTime(RtDataOutput_t *rout, unsigned int time)
{
    rout->sampleTime = time;
}

void rtoutProcess(RtDataOutput_t *rout)
{
    unsigned result;
    if(rout->state == RTOUTSTAT_STOP) return;
    while(rout->sidx < rout->count) {
        result = carrierPutPwmData(rout->carrier);
        if(result == 2) break;
        rout->sidx++;
        if(result == 1) break;
        if(rout->sidx == rout->count) carrierPwmBuffWriteDone(rout->carrier);
    }
}

void rtoutStart(RtDataOutput_t *rout)
{
    rout->state = RTOUTSTAT_RUN;
}

void rtoutStop(RtDataOutput_t *rout)
{
    rout->state = RTOUTSTAT_STOP;
}

