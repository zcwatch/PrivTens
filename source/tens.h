#ifndef __TENS_H
#define __TENS_H

#include "motortip.h"
#include "button.h"
#include "hc595.h"
#include "tensoutput.h"
#include "battery.h"
#include "heat.h"
#include "vib.h"
#include "tensconfig.h"
#include "tensproto.h"
#include "therapy.h"


#define TENS_NONE			0
#define TENS_IDLE			1
#define TENS_RUN			2
#define TENS_PAUSE			3
#define TENS_FW				4


#define KEYFUNC_VIB		0
#define KEYFUNC_HEAT	1
#define KEYFUNC_TIME	2


typedef struct {
	unsigned char state;
	unsigned char keyFunc;			//key function: vib & heat & run time
	unsigned char tensMode;
	unsigned char tensChannel;
	unsigned char chargeState:1;
	unsigned int chargeCnt;
	unsigned char btState:1;
	unsigned char pwrState:2;
	unsigned short pwrCount;
	unsigned char heatLevel;
	unsigned char vibMode;
	unsigned char channelCnt;

	unsigned char prescId;

	unsigned short runTime;
	unsigned short runTimeCount;
	unsigned short defaultTime;

	unsigned char tensLevel;
	unsigned char tarLevel;
	
	MotorTip_t motorTip;
	TensProto_t proto;
	Button_t button;
	HC595_t hc595;
	Battery_t battery;
	Heat_t heat;
	Vib_t vib;
	
	TensOutput_t tensOutput;
	TherapyManager_t therapyManager;
} Tens_t;

extern Tens_t privateTens;

void tensInit(Tens_t *tens);
void tensProcess(Tens_t *tens);
void tensChannelProcess(Tens_t *tens);
void tensSetLevel(Tens_t *tens, unsigned char level);
void tensPowerDown(Tens_t *tens);
void tensPowerUp(Tens_t *tens);
void tensCloseOutputChannel(Tens_t *tens);
void tensSetOutputChannel(Tens_t *tens, unsigned char channel, unsigned char mode);
void tensStartOutput(Tens_t *tens);
void tensStopOutput(Tens_t *tens);
void tensPauseOutput(Tens_t *tens);
unsigned short tensLoadPrescData(Tens_t *tens, unsigned char chann);
void tensSetOutputTime(Tens_t *tens, unsigned short time, unsigned short defaultTime);
void tensRunTimeUpdate(Tens_t *tens);
void tensSetHeat(Tens_t *tens, unsigned char level);
void tensSetVib(Tens_t *tens, unsigned char mode);
void tensLevelUpProcess(Tens_t *tens);


#endif
