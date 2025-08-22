#include "vib.h"

void vibInit(Vib_t *vib, VibControlFunc control)
{
	vib->control = control;
}

void vibProcess(Vib_t *vib)
{
	if(vib->state) {
		if(++vib->count == vib->state) {
			vib->control(0);
		} else if(vib->count == 100) {
			vib->control(1);
			vib->count = 0;
		}
	}
}

void vibSet(Vib_t *vib, unsigned char level)
{
	vib->state = level;
	vib->count = 0;
	vib->control(level);
}
