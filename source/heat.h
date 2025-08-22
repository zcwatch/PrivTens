#ifndef __HEAT_H
#define __HEAT_H


typedef void(*HeatControlFunc)(unsigned char state);

typedef struct {
	unsigned char state;
	unsigned char count;
	//unsigned char level;
	HeatControlFunc control;
} Heat_t;


void heatInit(Heat_t *heat, HeatControlFunc control);
void heatProcess(Heat_t *heat);
void heatSet(Heat_t *heat, unsigned char level);

#endif
