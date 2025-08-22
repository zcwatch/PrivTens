#ifndef __BTUART_H
#define __BTUART_H

#include "gd32f30x.h"

void btuartInit(void);
int btSenddata(unsigned char *data, int len);
unsigned char btReadBtSta(void);

#endif
