#include "prescoutput.h"

void waveBuffReset(WaveBuff_t *buff)
{
    buff->wIdx = 0;
    buff->rIdx = 0;
    buff->size = 0;
}

void waveBuffInit(WaveBuff_t *buff, unsigned short *data, unsigned short len)
{
    buff->data = data;
    buff->buffLen = len;
    waveBuffReset(buff);
}

unsigned short waveBuffLeft(WaveBuff_t *buff)
{
    return (buff->buffLen - buff->size);
}

unsigned short waveBuffRead(WaveBuff_t *buff)
{
    unsigned short data = buff->data[buff->rIdx++];
    buff->rIdx %= buff->buffLen;
    buff->size--;
    return data;
}

unsigned char waveBuffWrite(WaveBuff_t *buff, unsigned short data)
{
    if(buff->size >= buff->buffLen) return 0;
    buff->data[buff->wIdx++] = data;
    buff->wIdx %= buff->buffLen;
    buff->size++;
    return 1;
}

unsigned short waveBuffDataSize(WaveBuff_t *buff)
{
	return buff->size;
}

//freq: 100HZ, time: 0.1ms
//count = freq * time
//return count of one wave 
static unsigned int prescoutGetWaveCount(unsigned char freq, unsigned short time)
{
    unsigned int count;
    count = (unsigned int)freq * time / 100;
    return count;
}

static void prescoutMaxCount(unsigned int *count)
{
    if(*count > MAX_OUTPUT_DATA_COUNT) *count = MAX_OUTPUT_DATA_COUNT;
}

//static unsigned short prescoutGetWaveData(PrescOutput_t *pout)
//{
//    unsigned short data;
//    WaveOutput_t *wout = &pout->gout.wout;
//    if(wout->ccnt == wout->cidx) return 0;
//    if(wout->waveBuff->size == 0) return 0;
//    data  = waveBuffRead(wout->waveBuff);
//    wout->cidx++;
//    return data;
//}

static void prescoutPutPwmData(PrescOutput_t *pout)
{
    unsigned short data;
    WaveOutput_t *wout = &pout->gout.wout;

    if(wout->pwmBuffFsh) return;
    while(1) {
        if(carrierPutPwmData(pout->carrier) != 0) break;
        if(carrierIsEnd(pout->carrier)) {  
            if(((wout->ccnt == wout->cidx) && (wout->waveBuffFsh))) {
                if(wout->pwmBuffFsh == 0) wout->pwmBuffFsh = 1;
                break;
            }
			if(!waveBuffDataSize(wout->waveBuff)) break;
            data  = waveBuffRead(wout->waveBuff);
            pout->gout.wout.cidx++;
            carrierSetAmp(pout->carrier, data);
        } 
    }
}

//put wave data to buffer
static void prescoutPutWaveData(WaveOutput_t *wout)
{
    unsigned int i, count;
    unsigned short ccr;

	if(wout->waveBuffFsh) return;
	
    while((waveBuffLeft(wout->waveBuff) > 0) && (wout->curStep < wout->step)) {
        switch (wout->wave->type) {
        case WAVE_TRAP:
            if(wout->curStep >= wout->step) return;
            if(wout->ccnt == 0) {
                wout->widx = 0;
                wout->wcnt = prescoutGetWaveCount(wout->wave->freq, wout->wave->data.trap.time[wout->curStep]);
                wout->ccnt += wout->wcnt;
            }
            while(waveBuffLeft(wout->waveBuff) > 0) {
                if(wout->wcnt == wout->widx) {
                    wout->curStep++;
                    if(wout->curStep >= wout->step) {
						wout->waveBuffFsh = 1;
						break;
					}
                    wout->widx = 0;
                    wout->wcnt = prescoutGetWaveCount(wout->wave->freq, wout->wave->data.trap.time[wout->curStep]);
                    wout->ccnt += wout->wcnt;
                }
                count = ((wout->wcnt - wout->widx) <= (waveBuffLeft(wout->waveBuff))) ? (wout->wcnt - wout->widx) : (waveBuffLeft(wout->waveBuff));
                prescoutMaxCount(&count);
                for(i = 0; i < count; i++) {  //rise,high,fall,low
                    if(wout->curStep == 0) ccr = wout->base + wout->range * ((wout->widx + i) / (float)wout->wcnt);
                    else if(wout->curStep == 1) ccr = wout->amp / 2;
                    else if(wout->curStep == 2) ccr = wout->base + wout->range * ((1 - (wout->widx + 1) / (float)wout->wcnt));
                    else ccr = wout->base;
                    waveBuffWrite(wout->waveBuff, ccr);
                }
                wout->widx += count;
                if(count > 0) return;
            }
            break;
        case WAVE_PAINT:
            if(wout->ccnt == 0) {
                wout->ccnt = prescoutGetWaveCount(wout->wave->freq, wout->wave->data.paint.time);
                count = (wout->ccnt <= waveBuffLeft(wout->waveBuff)) ? wout->ccnt : waveBuffLeft(wout->waveBuff);
            } else {
                count = ((wout->ccnt - wout->widx) <= waveBuffLeft(wout->waveBuff)) ? (wout->ccnt - wout->widx) : waveBuffLeft(wout->waveBuff);
            }
            prescoutMaxCount(&count);
            for(i = 0; i < count; i++) {
                ccr = wout->base + (((unsigned int)wout->range * wout->wave->data.paint.data[(i + wout->widx) * wout->wave->data.paint.len / wout->ccnt]) >> 6);
                waveBuffWrite(wout->waveBuff, ccr);
            }
            wout->widx += count;
            if(wout->ccnt == wout->widx) {
                wout->curStep++;
				wout->waveBuffFsh = 1;
            }
            if(count > 0) return;
            break;
        case WAVE_PULSE:
            if(wout->curStep >= wout->step) return;
            if(wout->ccnt == 0) {
                wout->widx = 0;
                wout->wcnt = 1;
                wout->ccnt += wout->wcnt;
                wout->curStep = 1;
            }
            while(waveBuffLeft(wout->waveBuff) > 0) {
                if(wout->curStep >= wout->step) break;
                if(wout->wcnt == wout->widx) {
                    wout->curStep++;
                    if(wout->curStep >= wout->step) break;
                    wout->widx = 0;
                    wout->wcnt = prescoutGetWaveCount(wout->wave->freq, wout->wave->data.trap.time[wout->curStep]);
                    wout->ccnt += wout->wcnt;
                }
                count = ((wout->wcnt - wout->widx) <= waveBuffLeft(wout->waveBuff)) ? (wout->wcnt - wout->widx) : waveBuffLeft(wout->waveBuff);
                prescoutMaxCount(&count);
                for(i = 0; i < count; i++) {
                    if(wout->curStep == 0) ccr = wout->base + wout->range * ((wout->widx + 1) / (float)wout->wcnt);
                    else if(wout->curStep == 1) ccr = wout->amp / 2;
                    else ccr = wout->base;
                    waveBuffWrite(wout->waveBuff, ccr);
                }
                wout->widx += count;
                if(count > 0) return;
            }
            break;
        case WAVE_INTERVAL:
            // if(wout->curStep >= wout->step) return;
            // if(waveBuffLeft(wout->waveBuff) > 0) {
            //     //waveBuffWrite(wout->waveBuff, (wout->wave->data.interval.time / 10) | INTERVAL_MARK);
                
            //     wout->curStep = wout->step;
            //     wout->ccnt = wout->cidx = wout->wcnt = wout->widx = 1;
            //     wout->isFsh = 1;
            // }
            break;
        default:
            break;
        }
    }
}

static void prescoutWaveInit(PrescOutput_t *pout, unsigned char mode) 
{
    WaveOutput_t *wout = &pout->gout.wout;
    if(mode) {  //read wave data
        pout->waveDataLen = decodeWave(&pout->wave, pout->prescData + pout->prescDataIdx, pout->prescDataLen - pout->prescDataIdx);
        pout->prescDataIdx += pout->waveDataLen;
        wout->times = 0;
    }
    wout->isFsh = 0;
    wout->wcnt = 0;
    wout->ccnt = 0;
    wout->cidx = 0;
    wout->widx = 0;
	wout->waveBuffFsh = 0;
    wout->pwmBuffFsh = 0;
    wout->pwmFsh = 0;
    switch (wout->wave->type) {
    case WAVE_INTERVAL:
        wout->step = 1;
        wout->curStep = 1;
        waveBuffReset(wout->waveBuff);
        //这个做法有问题，当PWM BUFFER满时设置不了间隔
        carrierPutInterval(pout->carrier, wout->wave->data.interval.time / 10);
        wout->isFsh = 1;
        break;
    case WAVE_TRAP:
        wout->step = 4;
        wout->curStep = 0;
        wout->base = wout->amp / 2.0 * (1 - wout->wave->deep / 10.0);
        wout->range = wout->amp / 2.0 * wout->wave->deep / 10.0;
        waveBuffReset(wout->waveBuff);
        prescoutPutWaveData(wout);
	#if 0
        if(wout->wave->freq > 10) carrierSetFreq(pout->carrier, (wout->wave->freq - 10) * 100 + 400);
        else carrierSetFreq(pout->carrier, wout->wave->freq * 1000);
	#else
		carrierSetFreq(pout->carrier, wout->wave->freq * 100);
	#endif
        carrierSetAmp(pout->carrier, wout->waveBuff->data[0]);
        prescoutPutPwmData(pout);
        break;
    case WAVE_PAINT:
        wout->step = 1;
        wout->curStep = 0;
        wout->base = wout->amp / 2.0 * (1 - wout->wave->deep / 10.0);
        wout->range = wout->amp / 2.0 * wout->wave->deep / 10.0;
        waveBuffReset(wout->waveBuff);
        prescoutPutWaveData(wout);   
        if(wout->wave->freq > 10) carrierSetFreq(pout->carrier, (wout->wave->freq - 10) * 100 + 400);
        else carrierSetFreq(pout->carrier, wout->wave->freq * 1000);
        carrierSetAmp(pout->carrier, wout->waveBuff->data[0]);    
        prescoutPutPwmData(pout);
        break;
    case WAVE_PULSE:
        wout->step = 4;
        wout->curStep = 0;
        wout->base = wout->amp / 2.0 * (1 - wout->wave->deep / 10.0);
        wout->range = wout->amp / 2.0 * wout->wave->deep / 10.0;
        waveBuffReset(wout->waveBuff);
        prescoutPutWaveData(wout);
        if(wout->wave->freq > 10) carrierSetFreq(pout->carrier, (wout->wave->freq - 10) * 100 + 400);
        else carrierSetFreq(pout->carrier, wout->wave->freq * 1000);
        carrierSetAmp(pout->carrier, wout->waveBuff->data[0]);
        prescoutPutPwmData(pout);
        break;
    default:
        break;
    }
}

static void prescoutGroupInit(PrescOutput_t *pout, unsigned char mode)
{
    WaveGroupOutput_t *gout = &pout->gout;
    if(mode) {  //read group data
        pout->groupDataIdx = pout->prescDataIdx;
        pout->groupDataLen = decodeGroup(&pout->group, pout->prescData + pout->groupDataIdx, pout->prescDataLen - pout->groupDataIdx);
        pout->prescDataIdx += pout->groupDataLen;
        if(gout->group->count == 0) {
            gout->isFsh = 1;
            return;
        }
        gout->count = 0;
    }
    gout->widx = 0;
    gout->isFsh = 0;
    //gout->wout.wave = &gout->group->waves[gout->widx];
    switch(gout->group->mode) {
        case AMP_MODE_UP:
        gout->wout.amp = gout->wout.maxAar * (0.2 + 0.8 * (gout->count + 1) / gout->group->times);
        break;
        case AMP_MODE_DOWN:
        gout->wout.amp = gout->wout.maxAar * (gout->group->times - gout->count) / gout->group->times;
        break;
        case AMP_MODE_SUP:
        if(gout->count < 4) {
            gout->wout.amp = gout->wout.maxAar * (gout->count + 1) * 0.25;
        } else {
            gout->wout.amp = gout->wout.maxAar;
        }
        break;
        case AMP_MODE_SDOWN:
        if(gout->count < 4) {
            gout->wout.amp = gout->wout.maxAar * (1 - gout->count * 0.25);
        } else {
            gout->wout.amp = gout->wout.maxAar;
        }
        break;
        default:
        gout->wout.amp = gout->wout.maxAar;
        break;
    }
    pout->prescDataIdx = pout->groupDataIdx + GROUP_DATA_LEN;
    prescoutWaveInit(pout, 1);
}

static void prescoutWaveProcess(PrescOutput_t *pout)
{
    WaveOutput_t *wout = &pout->gout.wout;
    
    if(wout->isFsh) return;

    if((wout->pwmBuffFsh) && (wout->curStep >= wout->step) && (wout->ccnt == wout->cidx)) {
        if(wout->wave->interval > 0) {
            carrierPutInterval(pout->carrier, wout->wave->interval / 10);
        } else carrierPwmBuffWriteDone(pout->carrier);
        if(++wout->times == wout->wave->times) {
            wout->isFsh = 1;
        } else {
            prescoutWaveInit(pout, 0);
        }
    } else {
        prescoutPutWaveData(wout);
        prescoutPutPwmData(pout);
    }
}

static void prescoutGroupProcess(PrescOutput_t *pout)
{
    WaveGroupOutput_t *gout = &pout->gout;

    if(gout->wout.isFsh) {
        if(++gout->widx == pout->group.count) {
            if(gout->group->interval > 10) {
                carrierPutInterval(pout->carrier, gout->group->interval / 10);
            } 
            if(++gout->count == gout->group->times) gout->isFsh = 1;
            else prescoutGroupInit(pout, 0);
            
        } else {
            prescoutWaveInit(pout, 1);
        }
    } else {
        prescoutWaveProcess(pout);
    }
}

void prescoutProcess(PrescOutput_t *pout)
{   
    if(pout->state == PRESCOUT_STOP) return;

    if(pout->isFsh) {
        pout->state = PRESCOUT_STOP;
		prescoutStart(pout);
        // pout->callback_data.type = PRESC_CB_PRESC_FSH;
        // pout->callback(&pout->callback_data);
    } else {
        if(pout->gout.isFsh) {  //wavegroup is finish
            if(++pout->gIdx == pout->prescription.count) {  //prescription is finish
                pout->isFsh = 1;
            } else {    //init next wavegroup
                prescoutGroupInit(pout, 1);
            }
        } else {
            prescoutGroupProcess(pout);
        }
    }
}

void prescoutInit(PrescOutput_t *pout, Carrier_t *pc)
{
    pout->carrier = pc;
    pout->state = PRESCOUT_STOP;
    pout->gout.group = &pout->group;
    pout->gout.wout.wave = &pout->wave;
    pout->prescDataLen = 0;
}

void prescoutWaveBuffInit(PrescOutput_t *pout, WaveBuff_t *waveBuff)
{
    pout->gout.wout.waveBuff = waveBuff;
}

void prescoutSetPwmParams(PrescOutput_t *pout, unsigned int freq, unsigned short period)
{
    pout->freq = freq;
    pout->period = period;
	pout->gout.wout.period = period;
	pout->gout.wout.maxAar = period * 0.85;
}

void prescoutSetCarrier(PrescOutput_t *ppout, Carrier_t *pc)
{
    ppout->carrier = pc;
}

void prescoutSetPrescData(PrescOutput_t *ppout, unsigned char *data, unsigned short len)
{
    if(checkPrescription(data, len)) {
        ppout->prescData = data;
        ppout->prescDataLen = len;
    }
}

void prescoutStart(PrescOutput_t *pout)
{
    if(pout->prescDataLen == 0) return; 
    pout->state = PRESCOUT_RUN;
    pout->isFsh = 0;
    pout->prescDataIdx = decodePrescription(&pout->prescription, pout->prescData, pout->prescDataLen);
    pout->groupDataIdx = pout->prescDataIdx;
    prescoutGroupInit(pout, 1);
}

void prescoutStop(PrescOutput_t *pout)
{
    pout->state = PRESCOUT_STOP;
}
