#ifndef __VIB_H
#define __VIB_H

typedef void(*VibControlFunc)(unsigned char state);

typedef struct {
	unsigned char freq;
	unsigned char duty;
	unsigned short time;
} VibData_t;

typedef struct {
	unsigned char state;
	unsigned char count;
	//unsigned char level;
	VibControlFunc control;
} Vib_t;


void vibInit(Vib_t *vib, VibControlFunc control);
void vibProcess(Vib_t *vib);
void vibSet(Vib_t *vib, unsigned char level);



#endif
