#ifndef __BOARD_H
#define __BOARD_H

#include "defines.h"
#include "btuart.h"
#include "pwm.h"

#define HW_V11			        1

#define ADCH_SIZE				2

#if WDT_EN
void feedDog(void);
#endif

void halHardwareInit(void);

unsigned char halReadChargeSta(void);
unsigned char halKeyTensUp(void);
unsigned char halKeyTensDown(void);
unsigned char halKeyTensConfirm(void);
unsigned char halKeyPower(void);
unsigned char halScanKey(void);

void halPowerEn(unsigned char state);
void halMotor(unsigned char state);
void halMotorWarn(unsigned char state);
void halHVEnable(unsigned char state);
void halHC595SDA(unsigned char state);
void halHC595CLK(unsigned char state);
void halHC595LA(unsigned char state);

void halHeat(unsigned char state);

void halSetDac1(unsigned short val);
unsigned short halGetBatAdc(void);
unsigned short halGetHVAdc(void);

void halLcdWriteData(unsigned char data);
void halLcdWriteCmd(unsigned char cmd);

int halFlashWrite(unsigned int addr, unsigned char *data, unsigned int len);
void halFlashErasePage(unsigned int addr);
int halFlashErase(unsigned int addr, unsigned int size);


#endif
