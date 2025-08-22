#include "carrier.h"




static unsigned char spwm_sine[SPWM_SINE_SIZE] = {
64, 65, 66, 67, 68, 69, 70, 71, 72, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 
84, 85, 86, 87, 88, 90, 91, 92, 93, 94, 95, 95, 96, 97, 98, 99, 100, 101, 102, 103, 
104, 105, 105, 106, 107, 108, 109, 110, 110, 111, 112, 113, 113, 114, 115, 115, 116, 117, 117, 118, 
118, 119, 119, 120, 121, 121, 121, 122, 122, 123, 123, 124, 124, 124, 125, 125, 125, 126, 126, 126, 
126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 128, 128, 128, 127, 127, 127, 127, 127, 127, 127, 
127, 127, 126, 126, 126, 126, 125, 125, 125, 124, 124, 124, 123, 123, 122, 122, 122, 121, 121, 120, 
120, 119, 118, 118, 117, 117, 116, 115, 115, 114, 113, 113, 112, 111, 110, 110, 109, 108, 107, 106, 
106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 
87, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 71, 70, 69, 68, 67, 66, 
65, 64, 62, 61, 60, 59, 58, 57, 56, 55, 54, 52, 51, 50, 49, 48, 47, 46, 45, 44, 
43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 
23, 22, 22, 21, 20, 19, 18, 18, 17, 16, 15, 15, 14, 13, 12, 12, 11, 11, 10, 9, 
9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 
7, 8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21, 
21, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
40, 41, 42, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 56, 57, 58, 59, 60, 61, 62, 64
};

//calculate sine data
//pc: carrier point
//sine: sine array
static void carrierCalSineData(Carrier_t *pc, unsigned short *sine)
{
    unsigned short i, amp = pc->amp * 2;
	for(i = 0; i < SPWM_SINE_SIZE; i++) {
		sine[i] = pc->fix + (((unsigned int)pc->data[i] * amp) >> 7);
        if(pc->pwmBuff.phase == 3) sine[i + SPWM_SINE_SIZE] = sine[i];
	}
}

//set pwm data buffer full
static unsigned char carrierSetPwmBufFull(PwmBuff_t *pwmbuf)
{
	if(pwmbuf->datau[pwmbuf->wIdx].len == 0) 
		return 1;
    pwmbuf->datau[pwmbuf->wIdx].state = PWMDATASTAT_READY;
    if(++pwmbuf->wIdx == PWMDATA_BUFF_COUNT) pwmbuf->wIdx = 0;
	return 1;
}

//pwm data buff is empty or not
static unsigned char carrierPwmBufIsEmpty(PwmBuff_t *pwmbuf)
{
    if((pwmbuf->datau[pwmbuf->wIdx].state == PWMDATASTAT_NONE) || (pwmbuf->datau[pwmbuf->wIdx].state == PWMDATASTAT_WRITING)) return 1;
    else return 0;
}

//pwm data buffer is read, and set it empty
void carrierPwmBuffReadDone(Carrier_t *pc)
{
    pc->pwmBuff.datau[pc->pwmBuff.rIdx].state = PWMDATASTAT_NONE;
    pc->pwmBuff.datau[pc->pwmBuff.rIdx].len = 0;
	pc->pwmBuff.datau[pc->pwmBuff.rIdx].hasInterval = 0;
	pc->pwmBuff.datau[pc->pwmBuff.rIdx].interval = 0;
    if(++pc->pwmBuff.rIdx == PWMDATA_BUFF_COUNT) pc->pwmBuff.rIdx = 0;
}

void carrierInit(Carrier_t *pc)
{
    pc->pwmFreq = PWM_FREQ;
	pc->data = spwm_sine;
	pc->dataLen = SPWM_SINE_SIZE;
	pc->amp = 1;
}

void carrierPwmBuffInit(Carrier_t *pc, unsigned short *buff, unsigned char phase)
{
    unsigned char i;

    if(!((phase == 1) || (phase == 3))) phase = 1;
    pc->pwmBuff.phase = phase;
    for(i = 0; i < PWMDATA_BUFF_COUNT; i++) {
        pc->pwmBuff.datau[i].data = buff + i * PWMDATA_BUFF_SIZE;
    }
    if(phase == 3) {
        for(i = 0; i < PWMDATA_BUFF_COUNT; i++) {
            pc->pwmBuff.datav[i].data = buff + (PWMDATA_BUFF_COUNT + i) * PWMDATA_BUFF_SIZE;
        }
        for(i = 0; i < PWMDATA_BUFF_COUNT; i++) {
            pc->pwmBuff.dataw[i].data = buff + (PWMDATA_BUFF_COUNT * 2 + i) * PWMDATA_BUFF_SIZE;
        }
    }
    pc->pwmBuff.count = PWMDATA_BUFF_COUNT;
    carrierResetPwmBuff(pc);
}

void carrierSineInit(Carrier_t *pc, unsigned short *sine)
{
//    pc->pwmBuff.sineAmp = 0;
    pc->pwmBuff.sineBuff = sine;
    carrierCalSineData(pc, pc->pwmBuff.sineBuff);
}

void carrierSetPwmParams(Carrier_t *pc, unsigned int freq, unsigned short period)
{
    pc->pwmFreq = freq;
    pc->period = period;
}

void carrierSetFreq(Carrier_t *pc, unsigned int freq)
{
    if(pc->freq != freq) {
        pc->freq = freq;
        pc->count = pc->pwmFreq / pc->freq;
        if(pc->count == 0) pc->count = 1;
    }
}

void carrierSetAmp(Carrier_t *pc, unsigned short amp)
{
    if(pc->amp != amp) {
        pc->amp = amp;
        pc->fix = pc->period / 2 - pc->amp;
        carrierCalSineData(pc, pc->pwmBuff.sineBuff);
    }
	pc->idx = 0;
}

void carrierResetPwmBuff(Carrier_t *pc)
{
    unsigned char i;
    for(i = 0; i < PWMDATA_BUFF_COUNT; i++) {
        pc->pwmBuff.datau[i].len = 0;
        pc->pwmBuff.datau[i].state = PWMDATASTAT_NONE;
        pc->pwmBuff.datau[i].hasInterval = 0;
    }
    pc->pwmBuff.wIdx = 0;
    pc->pwmBuff.rIdx = 0;
}

void carrierPwmBuffWriteDone(Carrier_t *pc)
{
	if(pc->pwmBuff.datau[pc->pwmBuff.wIdx].len == 0) 
		return;
    pc->pwmBuff.datau[pc->pwmBuff.wIdx].state = PWMDATASTAT_READY;
    if(++pc->pwmBuff.wIdx == PWMDATA_BUFF_COUNT) pc->pwmBuff.wIdx = 0;
}

unsigned char carrierPutPwmData(Carrier_t *pc)
{
    unsigned char flag = 0;
    unsigned short idx;
	PwmBuff_t *pBuff = &pc->pwmBuff;
    if(!carrierPwmBufIsEmpty(pBuff)) {
        flag = 2;
        return flag;
    }
	if(pBuff->datau[pBuff->wIdx].state == PWMDATASTAT_NONE) pBuff->datau[pBuff->wIdx].state = PWMDATASTAT_WRITING;
    for(; pc->idx < pc->count; pc->idx++) {
        idx = pc->idx * pc->dataLen / pc->count;
        pBuff->datau[pBuff->wIdx].data[pBuff->datau[pBuff->wIdx].len] = pBuff->sineBuff[idx];
        if(pBuff->phase == 3) {
            pBuff->datav[pBuff->wIdx].data[pBuff->datau[pBuff->wIdx].len] = pBuff->sineBuff[(idx + 120) % SPWM_SINE_SIZE];
            pBuff->dataw[pBuff->wIdx].data[pBuff->datau[pBuff->wIdx].len] = pBuff->sineBuff[(idx + 240) % SPWM_SINE_SIZE];
        }
        if(++pBuff->datau[pBuff->wIdx].len == PWMDATA_BUFF_SIZE) {   //array is write ok
            carrierSetPwmBufFull(&pc->pwmBuff);
            flag = 1;
            break;
        }
    }
    return flag;
}

unsigned char carrierPutInterval(Carrier_t *pc, unsigned short time)
{
    unsigned char wid, flag = 1;

	if(pc->pwmBuff.wIdx == 0) wid = (PWMDATA_BUFF_COUNT - 1);
	else wid = pc->pwmBuff.wIdx - 1;
	if((pc->pwmBuff.datau[wid].state == PWMDATASTAT_READY) && (pc->pwmBuff.datau[pc->pwmBuff.wIdx].state != PWMDATASTAT_WRITING)) { //add to pre buff
		pc->pwmBuff.datau[wid].hasInterval = 1;
		pc->pwmBuff.datau[wid].interval += time;
		return flag;
	}
	
    if(!carrierPwmBufIsEmpty(&pc->pwmBuff)) { //no buff
		flag = 2;
        return flag;
    }
    pc->pwmBuff.datau[pc->pwmBuff.wIdx].hasInterval = 1;
	pc->pwmBuff.datau[pc->pwmBuff.wIdx].interval = time;
    pc->pwmBuff.datau[pc->pwmBuff.wIdx].state = PWMDATASTAT_READY;
	if(pc->pwmBuff.datau[pc->pwmBuff.wIdx].len == 0) {
		pc->pwmBuff.datau[pc->pwmBuff.wIdx].data[pc->pwmBuff.datau[pc->pwmBuff.wIdx].len++] = pc->period / 2;
	}
    carrierSetPwmBufFull(&pc->pwmBuff);
    return flag;
}

unsigned char carrierGetPwmData(Carrier_t *pc, unsigned short *data, unsigned short *len, unsigned char *phase)
{
    if(pc->pwmBuff.datau[pc->pwmBuff.rIdx].state == PWMDATASTAT_READY) {
		//pc->pwmData = pc->pwmBuff.datau[pc->pwmBuff.rIdx].data;
        data = pc->pwmBuff.datau[pc->pwmBuff.rIdx].data;
//        if(pc->pwmBuff.phase == 3) {
//            data[1] = pc->pwmBuff.datav[pc->pwmBuff.rIdx].data;
//            data[2] = pc->pwmBuff.dataw[pc->pwmBuff.rIdx].data;
//        }
        *len = pc->pwmBuff.datau[pc->pwmBuff.rIdx].len;
        *phase = pc->pwmBuff.phase;
        return 1;
    } else return 0;
}

unsigned char carrierIsEnd(Carrier_t *pc)
{
    if(pc->count == pc->idx) 
		return 1;
    else 
		return 0;
}

//-----------------------------------------------PwmOutput-----------------------------------------

void pwmoutInit(PwmOutput_t *pout, pwmOutputUpdateCallbackFunc cb)
{
    pout->state = PWMOUTSTAT_STOP;
    pout->updateCallback = cb;
}

// void pwmoutSetPwmBuff(PwmOutput_t *pout, Carrier_t *pc)
// {
//     pout->carrier = pc;
// }

Carrier_t* pwmoutGetCarrier(PwmOutput_t *pout)
{
    return &pout->carrier;
}

unsigned char pwmoutGetPwmData(PwmOutput_t *pout)
{ 
	if(pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].state == PWMDATASTAT_READY) {
		pout->pwmDataU = pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].data;
		pout->pwmDataLen = pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].len;
		if(pout->carrier.pwmBuff.phase == 3) {
			pout->pwmDataV = pout->carrier.pwmBuff.datav[pout->carrier.pwmBuff.rIdx].data;
			pout->pwmDataW = pout->carrier.pwmBuff.dataw[pout->carrier.pwmBuff.rIdx].data;
		}
		if(pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].len == 0) 
			pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].len = 0;
		return 1;
    } else return 0;
}

unsigned short pwmoutGetPwmDataLen(PwmOutput_t *pout)
{
	return pout->pwmDataLen;
}

unsigned short* pwmoutGetPwmDataUPtr(PwmOutput_t *pout)
{
	return pout->pwmDataU;
}

unsigned short* pwmoutGetPwmDataVPtr(PwmOutput_t *pout)
{
	return pout->pwmDataV;
}

unsigned short* pwmoutGetPwmDataWPtr(PwmOutput_t *pout)
{
	return pout->pwmDataW;
}

unsigned char pwmoutGetPhase(PwmOutput_t *pout)
{
	return pout->carrier.pwmBuff.phase;
}

void pwmoutProcess(PwmOutput_t *pout)
{
#if 0
    if((pout->state == PWMOUTSTAT_STOP) || (pout->state == PWMOUTSTAT_RUN) || (pout->state == PWMOUTSTAT_INTERVAL)) return; //pwm output is outputing or waitting interval
    //get next buffer
    pout->updateCallback(pout);
#else 
	if(pout->state == PWMOUTSTAT_WAIT) pout->updateCallback(pout);
#endif
}

void pwmoutIntervalProcess(PwmOutput_t *pout)
{
    if(pout->state != PWMOUTSTAT_INTERVAL) return;
    if(++pout->intervalCnt == pout->interval) {
        //carrierPwmBuffReadDone(&pout->carrier);   //set read is done
        //get next buffer
        pout->state = PWMOUTSTAT_WAIT;
        pout->updateCallback(pout);
    }
}

unsigned char pwmoutHasInterval(PwmOutput_t *pout)
{
    if(pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].hasInterval) {
        pout->state = PWMOUTSTAT_INTERVAL;
        pout->interval = pout->carrier.pwmBuff.datau[pout->carrier.pwmBuff.rIdx].interval;
        pout->intervalCnt = 0;
        carrierPwmBuffReadDone(&pout->carrier);   //set read is done
		return 1;
    }
		return 0;
}

void pwmoutStart(PwmOutput_t *pout)
{
	pout->state = PWMOUTSTAT_WAIT;
}

void pwmoutStop(PwmOutput_t *pout)
{
	pout->state = PWMOUTSTAT_STOP;
}

void pwmoutSetState(PwmOutput_t *pout, unsigned char state)
{
	pout->state = state;
}
