#include "prescdata.h"


#define CHK_SHAKE_EN                0

#define SINE_SIZE					360				
#define ROUND_SIZE					100				
#define EXPON_SIZE					100				
#define LOG_SIZE					100	



static unsigned char sine[SINE_SIZE] = {
32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 
42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 51, 51, 
52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 
59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 63, 63, 63, 
63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 
63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 60, 60, 60, 60, 
59, 59, 59, 59, 58, 58, 58, 57, 57, 57, 56, 56, 56, 55, 55, 54, 54, 54, 53, 53, 
52, 52, 51, 51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 43, 
43, 42, 42, 41, 41, 40, 40, 39, 38, 38, 37, 37, 36, 36, 35, 35, 34, 34, 33, 32, 
32, 31, 31, 30, 30, 29, 28, 28, 27, 27, 26, 26, 25, 25, 24, 24, 23, 22, 22, 21, 
21, 20, 20, 19, 19, 18, 18, 17, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 12, 
11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 
4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 
4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 
11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 
20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31
};

static unsigned char round[ROUND_SIZE] = {
0, 12, 18, 21, 25, 28, 30, 32, 34, 36, 38, 40, 41, 43, 44, 45, 47, 48, 49, 
50, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 
62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 
62, 62, 62, 61, 61, 61, 60, 60, 59, 59, 58, 58, 57, 57, 56, 55, 54, 54, 53, 52, 
51, 50, 49, 48, 47, 45, 44, 43, 41, 40, 38, 36, 34, 32, 30, 28, 25, 21, 18, 12, 0
};

static unsigned char exponential[EXPON_SIZE] = {
0, 6, 11, 16, 21, 25, 28, 32, 35, 37, 40, 42, 44, 46, 48, 49, 51, 52, 53, 
54, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 
62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 57, 52, 47, 42, 38, 35, 31, 28, 
26, 23, 21, 19, 17, 15, 14, 12, 11, 10, 9, 8, 7, 7, 6, 5, 5, 4, 4, 3, 
3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
};

static unsigned char logdata[LOG_SIZE] = {
0, 0, 0, 0, 0, 0, 0, 0, 1, 
1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 
3, 3, 3, 4, 4, 5, 5, 6, 7, 7, 
8, 9, 10, 11, 12, 14, 15, 17, 19, 21, 
23, 26, 28, 31, 35, 38, 42, 47, 52, 57, 
63, 63, 63, 63, 63, 63, 63, 63, 63, 62, 
62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 
60, 60, 60, 59, 59, 58, 58, 57, 56, 56, 
55, 54, 53, 52, 51, 49, 48, 46, 44, 42, 
40, 37, 35, 32, 28, 25, 21, 16, 11, 6, 
0, };

PaintData_t paintData[] = {
	{WAVE_PAINT, 0, 0}, {WAVE_PAINT + 1, 0, 0}, {WAVE_PAINT + 2, 0,0},
	{WAVE_PAINT + 3, 0, 0}, {WAVE_PAINT + 4, 0, 0}, {WAVE_PAINT + 5, 0,0},
	{WAVE_PAINT + 6, 0, 0}, {WAVE_PAINT + 7, 0, 0}, {WAVE_PAINT + 8, 0,0},
	{WAVE_PAINT + 9, 0, 0}, {WAVE_PAINT + 10, 0, 0}, {WAVE_PAINT + 11, 0,0},
	{WAVE_PAINT + 12, 0, 0}, {WAVE_PAINT + 13, 0, 0}, {WAVE_PAINT + 14, 0,0},
	{WAVE_ROUND, round, ROUND_SIZE},
	{WAVE_SINE, sine, SINE_SIZE},
	{WAVE_EXPO, exponential, EXPON_SIZE}
};

static unsigned char checkWavePaintRange(unsigned char tid)
{
    if((tid >= WAVE_PAINT) && (tid <= (WAVE_PAINT + WAVE_PAINT_SIZE))) return 1;
    else return 0;
}

static unsigned char checkWaveDeep(unsigned char deep)
{
    if((deep > 0) && (deep <= 0x0a)) return 1;
    else return 0;
}

static unsigned char checkWaveShake(unsigned char shake)
{
#if CHK_SHAKE_EN
    if((shake > 0) && (shake <= 0x0a)) return 1;
    else return 0;
#else
    return 1;
#endif
}

int decodeWave(Wave_t *wave, unsigned char *data, int len)
{
    int  decode_len = 0;
    unsigned char paintId;
    wave->type = data[decode_len++];

    if(checkWavePaintRange(wave->type)) {
        wave->type = WAVE_PAINT;
        paintId = wave->type - WAVE_PAINT;
    }

    switch(wave->type) {
        case WAVE_INTERVAL:
        wave->freq = 10;
        wave->data.interval.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = 1;
        break;
        case WAVE_SQUARE:
        wave->type = WAVE_TRAP;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.trap.time[TIME_RISING] = 0;
        wave->data.trap.time[TIME_HIGH] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_FALLING] = 0;
        wave->data.trap.time[TIME_LOW] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        break;
        case WAVE_TRAP:
        wave->type = WAVE_TRAP;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.trap.time[TIME_RISING] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_HIGH] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_FALLING] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_LOW] = 0;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        break;
        case WAVE_TRIANGLE:
        wave->type = WAVE_TRAP;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.trap.time[TIME_RISING] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_HIGH] = 0;
        wave->data.trap.time[TIME_FALLING] = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->data.trap.time[TIME_LOW] = 0;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        break;
        case WAVE_ROUND:
        wave->type = WAVE_PAINT;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.paint.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        wave->data.paint.data = round;
        wave->data.paint.len = ROUND_SIZE;
        break;
        case WAVE_SINE:
        wave->type = WAVE_PAINT;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.paint.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        wave->data.paint.data = sine;
        wave->data.paint.len = SINE_SIZE;
        break;
        case WAVE_EXPO:
        wave->type = WAVE_PAINT;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.paint.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        wave->data.paint.data = exponential;
        wave->data.paint.len = EXPON_SIZE;
        break;
        case WAVE_LOG:
        wave->type = WAVE_PAINT;
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.paint.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        wave->data.paint.data = logdata;
        wave->data.paint.len = LOG_SIZE;
        break;
        case WAVE_PAINT:
        wave->freq = data[decode_len++];
        if(wave->freq == 0) return 0;
        wave->deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave->deep)) return 0;
        wave->shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave->shake)) return 0;
        decode_len++;
        wave->data.paint.time = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->interval = GETSHORT(data, decode_len);
        decode_len += 2;
        wave->times = data[decode_len++];
        //wave->data.paint.data = logdata;
        //wave->data.paint.len = LOG_SIZE;
        break;
        default:
        decode_len = 0;
        break;
    }
    if(decode_len > len) return 0;
    return decode_len;
}

static unsigned char checkGroupMode(unsigned char mode)
{
    if((mode >= 1) && (mode <= 5)) return 1;
    else return 0;
}

static unsigned char checkGroupShake(unsigned char shake)
{
#if CHK_SHAKE_EN
    if((shake > 0) && (shake <= 0x0a)) return 1;
    else return 0;
#else
    return 1;
#endif   
}

int decodeGroup(WaveGroup_t *group, unsigned char *data, int len)
{
    int idx = 0;

    group->id = data[idx++];
    group->times = data[idx++];
    group->interval = GETSHORT(data, idx);
    idx += 2;
    group->count = data[idx++];
    group->mode = (data[idx] >> 4) & 0x0f;
    if(!checkGroupMode(group->mode)) return 0;
    group->shake = data[idx] & 0x0f;
    if(!checkGroupShake(group->shake)) return 0;
    idx++;

    return idx;
}

int decodePrescription(Prescription_t *p, unsigned char *data, int len)
{
    int idx = 0;
    unsigned short prescLen;
    prescLen = GETSHORT(data, idx);
    idx += 2;
    p->id = GETSHORT(data, idx);
    idx += 2;
    p->count = data[idx++];
    return idx;
}

static int checkWave(unsigned char *data, int len)
{
    int  decode_len = 0;
    unsigned char paintId;
    Wave_t wave;
    wave.type = data[decode_len++];

    if(checkWavePaintRange(wave.type)) {
        wave.type = WAVE_PAINT;
        paintId = wave.type - WAVE_PAINT;
    }

    switch(wave.type) {
        case WAVE_INTERVAL:
        decode_len += 2;
        break;
        case WAVE_SQUARE:
        wave.type = WAVE_TRAP;
        wave.freq = data[decode_len++];
        if(wave.freq == 0) return 0;
        wave.deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave.deep)) return 0;
        wave.shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave.shake)) return 0;
        decode_len += 8;
        break;
        case WAVE_TRAP:
        wave.type = WAVE_TRAP;
        wave.freq = data[decode_len++];
        if(wave.freq == 0) return 0;
        wave.deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave.deep)) return 0;
        wave.shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave.shake)) return 0;
        decode_len += 10;
        break;
        case WAVE_TRIANGLE:
        wave.type = WAVE_TRAP;
        wave.freq = data[decode_len++];
        if(wave.freq == 0) return 0;
        wave.deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave.deep)) return 0;
        wave.shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave.shake)) return 0;
        decode_len += 8;
        break;
        case WAVE_ROUND:
        case WAVE_SINE:
        case WAVE_EXPO:
        case WAVE_LOG:
        case WAVE_PAINT:
        wave.type = WAVE_PAINT;
        wave.freq = data[decode_len++];
        if(wave.freq == 0) return 0;
        wave.deep = (data[decode_len] >> 4) & 0x0f;
        if(!checkWaveDeep(wave.deep)) return 0;
        wave.shake = data[decode_len] & 0x0f;
        if(!checkWaveShake(wave.shake)) return 0;
        decode_len += 6;
        break;
        default:
        decode_len = 0;
        break;
    }
    if(decode_len > len) return 0;
    return decode_len;
}

static int checkGroup(unsigned char *data, int len)
{
    int idx = 0, wlen, i;
    WaveGroup_t group;

    group.id = data[idx++];
    group.times = data[idx++];
    group.interval = GETSHORT(data, idx);
    idx += 2;
    group.count = data[idx++];
    group.mode = (data[idx] >> 4) & 0x0f;
    if(!checkGroupMode(group.mode)) return 0;
    group.shake = data[idx] & 0x0f;
    if(!checkGroupShake(group.shake)) return 0;
    idx++;

    for(i = 0; i < group.count; i++) {
        wlen = checkWave(data + idx, len - idx);
        if(wlen == 0) return 0;
        idx += wlen;
    }
    if(idx > len) return 0;
    return idx;
}

int checkPrescription(unsigned char *data, int len)
{
    Prescription_t prescription;
    int idx = 0, glen, i;
    unsigned short prescLen;
    prescLen = GETSHORT(data, idx);
    idx += 2;
    prescription.id = GETSHORT(data, idx);
    idx += 2;
    prescription.count = data[idx++];
    for(i = 0; i < prescription.count; i++) {
        glen = checkGroup(data + idx, len + 2 - idx);
        if(glen == 0) {
            prescription.count = 0;
            return 0;
        }
        idx += glen;
    }
    if(idx > len + 2) return 0;
    return 1;
}
