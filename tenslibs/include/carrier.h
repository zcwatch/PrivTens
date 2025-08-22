#ifndef __CARRIER_H
#define __CARRIER_H

#define PWM_FREQ                        300000
#define SPWM_SINE_SIZE			        361
#define PWMDATA_BUFF_COUNT              4
#define PWMDATA_BUFF_SIZE               1024

//无状态、写数据、数据装备好、正在读数据
#define PWMDATASTAT_NONE                0
#define PWMDATASTAT_WRITING             1
#define PWMDATASTAT_READY               2
#define PWMDATASTAT_READING             3

//停止、正在输出、间隔、暂停、等待数据
#define PWMOUTSTAT_STOP                 0   
#define PWMOUTSTAT_RUN                  1
#define PWMOUTSTAT_INTERVAL             2
#define PWMOUTSTAT_PAUSE                3
#define PWMOUTSTAT_WAIT                 4


typedef struct {
    unsigned char state:4;              //0~4
    unsigned char hasInterval:4;        //0~no interval, 1~has interval
    unsigned short *data;               //pwm data point
    unsigned short len;                 //pwm data length
    unsigned short interval;            //interval time ms
} PwmData_t;

typedef struct {
    unsigned char count;                    //pwm data count
    unsigned char wIdx;                     //write index
    unsigned char rIdx;                     //read index
    unsigned char phase;                    //count of phase,1 or 3
    PwmData_t datau[PWMDATA_BUFF_COUNT];    //data of u
    PwmData_t datav[PWMDATA_BUFF_COUNT];    //data of v
    PwmData_t dataw[PWMDATA_BUFF_COUNT];    //data of w

//    unsigned short sineAmp;                 //amp of sine
    unsigned short *sineBuff;               //buffer of sine data
} PwmBuff_t;

typedef struct {
    unsigned int pwmFreq;               //
    unsigned int freq;                  //
    unsigned int amp;                   //
    unsigned char *data;                //
    unsigned short dataLen;             //

    unsigned int count;                 //
    unsigned int idx;                   //
    unsigned int period;                //
    unsigned int fix;                   //

    PwmBuff_t pwmBuff;                  //
} Carrier_t;

struct _PwmOutput_t;
typedef void(*pwmOutputUpdateCallbackFunc)(struct _PwmOutput_t *pc);

typedef struct _PwmOutput_t{
    unsigned char state;                //
    unsigned short intervalCnt;         //
    unsigned short interval;            //
	unsigned short pwmDataLen;
	unsigned short *pwmDataU, *pwmDataV, *pwmDataW;
    Carrier_t carrier;                  //
    pwmOutputUpdateCallbackFunc updateCallback;
} PwmOutput_t;


void carrierInit(Carrier_t *pc);
void carrierPwmBuffInit(Carrier_t *pc, unsigned short *buff, unsigned char phase);
void carrierSineInit(Carrier_t *pc, unsigned short *sine);
void carrierSetPwmParams(Carrier_t *pc, unsigned int freq, unsigned short period);
void carrierSetFreq(Carrier_t *pc, unsigned int freq);
void carrierSetAmp(Carrier_t *pc, unsigned short amp);
void carrierResetPwmBuff(Carrier_t *pc);
void carrierPwmBuffReadDone(Carrier_t *pc);
void carrierPwmBuffWriteDone(Carrier_t *pc);
unsigned char carrierPutPwmData(Carrier_t *pc);
unsigned char carrierPutInterval(Carrier_t *pc, unsigned short time);
unsigned char carrierGetPwmData(Carrier_t *pc, unsigned short *data, unsigned short *len, unsigned char *phase);
unsigned char carrierIsEnd(Carrier_t *pc);

void pwmoutInit(PwmOutput_t *pout, pwmOutputUpdateCallbackFunc cb);
//void pwmoutSetPwmBuff(PwmOutput_t *pout, Carrier_t *pc);
unsigned char pwmoutGetPwmData(PwmOutput_t *pout);
unsigned short pwmoutGetPwmDataLen(PwmOutput_t *pout);
unsigned short* pwmoutGetPwmDataUPtr(PwmOutput_t *pout);
unsigned short* pwmoutGetPwmDataVPtr(PwmOutput_t *pout);
unsigned short* pwmoutGetPwmDataWPtr(PwmOutput_t *pout);
unsigned char pwmoutGetPhase(PwmOutput_t *pout);
void pwmoutProcess(PwmOutput_t *pout);
void pwmoutIntervalProcess(PwmOutput_t *pout);
unsigned char pwmoutHasInterval(PwmOutput_t *pout);
void pwmoutStart(PwmOutput_t *pout);
void pwmoutStop(PwmOutput_t *pout);
Carrier_t* pwmoutGetCarrier(PwmOutput_t *pout);
void pwmoutSetState(PwmOutput_t *pout, unsigned char state);

#endif
