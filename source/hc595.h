#ifndef __HC595_H
#define __HC595_H

typedef void(*HC595IoFunc)(unsigned char state);

typedef struct {
	HC595IoFunc sda;
	HC595IoFunc clk;
	HC595IoFunc la;
} HC595_t;

void hc595Init(HC595_t *hc);
void hc595SetData(HC595_t *hc, unsigned char data);


#endif
