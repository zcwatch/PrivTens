#include "heat.h"


void heatInit(Heat_t *heat, HeatControlFunc control)
{
	heat->control = control;
}

void heatProcess(Heat_t *heat)
{
	if(heat->state) {
		if(++heat->count == heat->state) {
			heat->control(0);
		} else if(heat->count == 100) {
			heat->control(1);
			heat->count = 0;
		}
	}
}

void heatSet(Heat_t *heat, unsigned char level)
{
	heat->state = level;
	heat->count = 0;
	heat->control(level);
}
