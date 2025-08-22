#ifndef __TENSGUI_H
#define __TENSGUI_H

#include "tens.h"

void guiInit(Tens_t *tens);
void guiPoweroff(void);
void guiUpdateBluetooth(Tens_t *tens);
void guiUpdateBattery(Tens_t *tens);
void guiUpdatePresc(Tens_t *tens);
void guiUpdateStrength(Tens_t *tens);
void guiUpdateHeat(Tens_t *tens);
void guiUpdateVib(Tens_t *tens);
void guiUpdateTime(Tens_t *tens);
void guiDisplayRunTime(Tens_t *tens);
void guiUpdateTensState(Tens_t *tens);
void guiFwUpdateFsh(Tens_t *tens);

#endif
