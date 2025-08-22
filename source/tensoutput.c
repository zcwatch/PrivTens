#include "tensoutput.h"
#include "pwm.h"


unsigned short pwmBuff[PWMDATA_BUFF_COUNT * PWMDATA_BUFF_SIZE], wBuff[WBUFF_SIZE], sineBuff[WBUFF_SIZE * 2];
WaveBuff_t gbWaveBuff = {wBuff, WBUFF_SIZE, 0, 0, 0};

static void pwmChannelUpdate(PwmOutput_t *pout)
{
    unsigned short len;
    if(pwmoutGetPwmData(pout)) {
        len = pwmoutGetPwmDataLen(pout);
        if(len > 0) {
            dma_memory_address_config(DMA0, DMA_CH1, (unsigned int)pwmoutGetPwmDataUPtr(pout));
            dma_transfer_number_config(DMA0, DMA_CH1, len);
            dma_channel_enable(DMA0, DMA_CH1);
			pwmoutSetState(pout, PWMOUTSTAT_RUN);
        } else {
            pwmoutHasInterval(pout);
        }
    } else {
        pwmoutSetState(pout, PWMOUTSTAT_WAIT);
    }
}

void tensoutInit(TensOutput_t * pt)
{
    pwmoutInit(&pt->pwmOutput, pwmChannelUpdate);
    carrierInit(pwmoutGetCarrier(&pt->pwmOutput));
    carrierPwmBuffInit(pwmoutGetCarrier(&pt->pwmOutput), pwmBuff, 1);
    carrierSineInit(pwmoutGetCarrier(&pt->pwmOutput), sineBuff);
    carrierSetPwmParams(pwmoutGetCarrier(&pt->pwmOutput), HIGH_FREQ, pwmPeriod);
    prescoutInit(&pt->prescOutput, pwmoutGetCarrier(&pt->pwmOutput));
    prescoutWaveBuffInit(&pt->prescOutput, &gbWaveBuff);
    prescoutSetPwmParams(&pt->prescOutput, HIGH_FREQ, pwmPeriod);
    rtoutInit(&pt->rtDataOutput, pwmoutGetCarrier(&pt->pwmOutput));
}


void tensoutProcess(TensOutput_t *pt)
{
    if(pt->outType == TENS_OUT_TYPE_PRESC) {
        prescoutProcess(&pt->prescOutput);
    } else {
        rtoutProcess(&pt->rtDataOutput);
    }
}

void tensoutSetPrescription(TensOutput_t *pt, unsigned char *data, unsigned short len)
{
    prescoutSetPrescData(&pt->prescOutput, data, len);
}

unsigned short tensoutGetPrescription(TensOutput_t *pt)
{
    return 0;
}

void tensoutStartOutput(TensOutput_t *pt)
{
    prescoutStart(&pt->prescOutput);
    pwmoutStart(&pt->pwmOutput); 
    pt->state = TENS_OUT_STAT_RUN;
}

void tensoutStopOutput(TensOutput_t *pt)
{
    prescoutStop(&pt->prescOutput);
    pt->state = TENS_OUT_STAT_STOP;
}

void tensoutPauseOutput(TensOutput_t *pt)
{
    prescoutStop(&pt->prescOutput);
    pt->state = TENS_OUT_STAT_PAUSE;
}
