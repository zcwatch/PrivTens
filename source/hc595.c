#include "hc595.h"
//#include "gd32f30x.h"

void hc595Init(HC595_t *hc)
{
	hc->sda(0);
	hc->clk(0);
	hc->la(0);
}

static void hc595Delay(unsigned int n)
{
	unsigned int cnt;
	for(cnt = 0; cnt < n; cnt++);
}

void hc595SetData(HC595_t *hc, unsigned char data)
{
	unsigned char i;
	for(i = 0; i < 8; i++) {
		if((data << i) & 0x80) hc->sda(1);
		else hc->sda(0);
		hc->clk(0);
		//__nop();
		hc->clk(1);
	}
	hc->la(0);
	//__nop();
	hc->la(1);
}
