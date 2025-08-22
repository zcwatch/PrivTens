#ifndef __MOTORTIP_H
#define __MOTORTIP_H

#define MOTORTIP_MODE1				1		//振动一次
#define MOTORTIP_MODE2				2		//振动两次

typedef void(*MotorControlFunc)(unsigned char state);

typedef struct {
	unsigned char run;
	unsigned char time;
	unsigned char timeCnt;
	unsigned char step;
} MotorTipState_t;

typedef struct {
	unsigned char mode;
	MotorControlFunc control;
	MotorTipState_t state;
} MotorTipInfo_t;

typedef struct {
	unsigned char count;
	MotorTipInfo_t *motors;
} MotorTip_t;

void mtInit(MotorTip_t *mt, MotorTipInfo_t *motors, unsigned char count);
void mtStart(MotorTip_t *mt, unsigned char id, unsigned char mode);
void mtStop(MotorTip_t *mt, unsigned char id);
void mtProcess(MotorTip_t *mt);

#endif
