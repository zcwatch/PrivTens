#include "motortip.h"

void mtInit(MotorTip_t *mt, MotorTipInfo_t *motors, unsigned char count)
{
	mt->motors = motors;
	mt->count = count;
}

void mtStart(MotorTip_t *mt, unsigned char id, unsigned char mode)
{
	if(id >= mt->count) return;
	mt->motors[id].mode = mode;
	switch(mode) {
		case MOTORTIP_MODE1:	
			mt->motors[id].state.time = 30;
			break;
		case MOTORTIP_MODE2:
			mt->motors[id].state.time = 50;
			break;
		default:
			break;
	}
	mt->motors[id].control(1);
	mt->motors[id].state.run = 1;
	mt->motors[id].state.step = 0;
	mt->motors[id].state.timeCnt = 0;
}

void mtStop(MotorTip_t *mt, unsigned char id)
{
	if(id >= mt->count) return;
	mt->motors[id].control(0);
	mt->motors[id].state.run = 0;
}

static void mtMode1(MotorTipInfo_t *mi)
{
	if(++mi->state.timeCnt == mi->state.time) {
		mi->control(0);
		mi->state.run = 0;
	}
}

static void mtMode2(MotorTipInfo_t *mi)
{
	if(mi->state.step == 0) {
		if(++mi->state.timeCnt == mi->state.time) {
			mi->control(0);
			mi->state.step++;
			mi->state.timeCnt = 0;
		}
	} else if(mi->state.step == 1) {
		if(++mi->state.timeCnt == mi->state.time) {
			mi->control(1);
			mi->state.step++;
			mi->state.timeCnt = 0;
		}
	} else {
		if(++mi->state.timeCnt == mi->state.time) {
			mi->control(0);
			mi->state.run = 0;
			mi->state.timeCnt = 0;
		}
	}
}

void mtProcess(MotorTip_t *mt)
{
	unsigned char i;
	for(i = 0; i < mt->count; i++) {
		if(mt->motors[i].state.run) {
			switch(mt->motors[i].mode) {
				case MOTORTIP_MODE1:
					mtMode1(&mt->motors[i]);
					break;
				case MOTORTIP_MODE2:
					mtMode2(&mt->motors[i]);
					break;
				default:
					break;
			}
		}
	}
}
