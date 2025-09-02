#include "tensgui.h"
#include "lcd.h"

#define MW_RECT				0, 0, 320, 240

#define BAT_POS				248, 11
#define BAT_RECT			BAT_POS, 303, 38
#define BT_POS				218, 10
#define BT_RECT				BT_POS, 233, 37

#define PRESC_POS			70, 65
#define PRESC_RECT		PRESC_POS, 169, 124
#define STRENGTH_POS	65, 135
#define STRENGTH_RCT	STRENGTH_POS, 120, 185
#define STRENGTH_CLOSE_POS 70, 143, 93, 166
#define VIB_POS				160, 135
#define VIB_RECT			VIB_POS, 199, 174
#define VIBNUM_POS		173, 142
#define HEAT_POS			235, 135
#define HEAT_RECT			HEAT_POS, 300, 185
#define HEAT_CLOSE_POS 245, 143, 268, 166
#define C_POS					276, 143, 299, 166
#define LTIME_POS			0, 0
#define LTIME_RECT		LTIME_POS, 0, 0
#define TENSSTAT_RECT	140, 195, 179, 236
#define TENSTIME_POS	200, 65
#define TENSTIME_RECT	TENSTIME_POS, 300, 105

extern const unsigned char gImage_mainui[];	//320X240
extern const unsigned char gImage_bat0[];	//50X28
extern const unsigned char gImage_bat25[];
extern const unsigned char gImage_bat50[];
extern const unsigned char gImage_bat75[];
extern const unsigned char gImage_bat100[];
extern const unsigned char gImage_cbat0[];
extern const unsigned char gImage_cbat25[];
extern const unsigned char gImage_cbat50[];
extern const unsigned char gImage_cbat75[];
extern const unsigned char gImage_btoff[];	//14X28
extern const unsigned char gImage_bton[];
extern const unsigned char gImage_run[];	//40X40
extern const unsigned char gImage_stop[];
extern const unsigned char gImage_pause[];

extern const unsigned char gImage_vib[];
extern const unsigned char gImage_rev1[];
extern const unsigned char gImage_rev2[];
extern const unsigned char gImage_close[];
extern const unsigned char gImage_c[];


void guiInit(Tens_t *tens)
{
	//lcdClear(WHITE);
	//draw main gui picture
	//lcdDispPic1(gImage_tensui);
	lcdDispPic(MW_RECT, gImage_mainui, BLACK, WHITE);
	//draw data
	guiUpdateBattery(tens);
	guiUpdateBluetooth(tens);
	guiUpdateHeat(tens);
	guiUpdatePresc(tens);
	guiUpdateVib(tens);
	guiUpdateStrength(tens);
	guiUpdateTime(tens);
	guiUpdateTensState(tens);

	//lcdDispString(100, 50, 100, 50, 24, "Hello");
	lcdPower(1);
}

void guiPoweroff(void)
{
	lcdPower(0);
	lcdClear(BLACK);
}

void guiUpdateBluetooth(Tens_t *tens)
{
	//lcdFill(BT_RECT, WHITE);
	if(tens->btState) lcdDispPic(BT_RECT, gImage_bton, BLACK, WHITE);
	else lcdDispPic(BT_RECT, gImage_btoff, BLACK, WHITE);
}

void guiUpdateBattery(Tens_t *tens)
{
#if 1
	if(tens->chargeState) {
		if(tens->battery.per <= 5) lcdDispPic(BAT_RECT, gImage_cbat0, BLACK, WHITE);
		else if(tens->battery.per <= 30) lcdDispPic(BAT_RECT, gImage_cbat25, BLACK, WHITE);
		else if(tens->battery.per <= 55) lcdDispPic(BAT_RECT, gImage_cbat50, BLACK, WHITE);
		else if(tens->battery.per <= 80) lcdDispPic(BAT_RECT, gImage_cbat75, BLACK, WHITE);
		else lcdDispPic(BAT_RECT, gImage_bat100, BLACK, WHITE);
	} else {
		if(tens->battery.per <= 5) lcdDispPic(BAT_RECT, gImage_bat0, BLACK, WHITE);
		else if(tens->battery.per <= 30) lcdDispPic(BAT_RECT, gImage_bat25, BLACK, WHITE);
		else if(tens->battery.per <= 55) lcdDispPic(BAT_RECT, gImage_bat50, BLACK, WHITE);
		else if(tens->battery.per <= 80) lcdDispPic(BAT_RECT, gImage_bat75, BLACK, WHITE);
		else lcdDispPic(BAT_RECT, gImage_bat100, BLACK, WHITE);
	}
#else
	lcdDispPic(BAT_RECT, gImage_bat100, BLACK, WHITE);
#endif
}

void guiUpdatePresc(Tens_t *tens)
{
	char num[10] = {'R', 'S', 0, 0, 0};
	lcdUint2Str(tens->prescId, num + 2, 2);
	lcdFill(PRESC_RECT, WHITE);
	//lcdDispString(PRESC_POS, 100, 50, 24, num);
	lcdDispString(PRESC_POS, 100, 50, 40, num);
}

void guiUpdateStrength(Tens_t *tens)
{
	char num[3] = {0};
	lcdFill(STRENGTH_RCT, WHITE);
	if(tens->tensLevel == 0) {
		lcdDispPic(STRENGTH_CLOSE_POS, gImage_close, BLACK, WHITE); 
		return;
	}
	lcdUint2Str(tens->tensLevel, num, 2);
	lcdFill(STRENGTH_RCT, WHITE);
	//lcdDispString(STRENGTH_POS, 120, 50, 24, num);
	lcdDispString(STRENGTH_POS, 180, 80, 40, num);
}

void guiUpdateHeat(Tens_t *tens)
{
	char num[3] = {0};
	unsigned char temp;
	lcdFill(HEAT_RECT, WHITE);
	if(tens->heatLevel == 0) {
		if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_HEAT)) {	//selected
			lcdDispPic(HEAT_CLOSE_POS, gImage_close, WHITE, BLACK); 
		} else {
			lcdDispPic(HEAT_CLOSE_POS, gImage_close, BLACK, WHITE); 
		}
		return;
	}
	if(tens->heatLevel == 1) temp = 35;
	else if(tens->heatLevel == 2) temp = 38;
	else if(tens->heatLevel == 3) temp = 42;
	else temp = 45;
	lcdUint2Str(temp, num, 2);
	if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_HEAT)) {	//selected
		lcdDispString1(HEAT_POS, 50, 80, 40, num);
	} else {	
		lcdDispString(HEAT_POS, 50, 80, 40, num); 
	}
	lcdDispPic(C_POS, gImage_c, BLACK, WHITE);
	
}

void guiUpdateVib(Tens_t *tens)
{
	char num[3] = {0};
	lcdUint2Str(tens->vibMode, num, 1);
	lcdFill(VIB_RECT, WHITE);
	//lcdDispString(VIB_POS, 100, 50, 24, num);
	switch(tens->vibMode) {
		case 0: 
		case 1: 
		case 2: 
		case 3: 
			if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_VIB)) {	//selected
				lcdDispPic(VIB_RECT, gImage_vib, WHITE, BLACK); 
				lcdDispString1(VIBNUM_POS, 24, 24, 24, num);
			} else {
				lcdDispPic(VIB_RECT, gImage_vib, BLACK, WHITE); 
				lcdDispString(VIBNUM_POS, 24, 24, 24, num);
			}
			break;
		case 4: 
			if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_VIB)) {	//selected
				lcdDispPic(VIB_RECT, gImage_rev1, WHITE, BLACK); 
			} else {
				lcdDispPic(VIB_RECT, gImage_rev1, BLACK, WHITE); 
			}
		break;
		case 5: 
			if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_VIB)) {	//selected
				lcdDispPic(VIB_RECT, gImage_rev2, WHITE, BLACK);
			} else {
				lcdDispPic(VIB_RECT, gImage_rev2, BLACK, WHITE); 
			}
		break;
		default: break;
	}
}

void guiUpdateTime(Tens_t *tens)
{
	char time[8] = {0, 0, ':', 0, 0, 0};
	unsigned short lefttime = tens->runTime - tens->runTimeCount;
	unsigned char min, sec;
	min = lefttime / 60;
	sec = lefttime % 60;
	lcdUint2Str(min, time, 2);
	lcdUint2Str(sec, time + 3, 2);
	
	if((tens->state == TENS_RUN) && (tens->keyFunc == KEYFUNC_TIME)) {
		lcdFill(TENSTIME_RECT, BLACK);
		if(tens->state != TENS_OUT_STAT_STOP) lcdDispString1(TENSTIME_POS, 100, 50, 40, time);
	} else {
		lcdFill(TENSTIME_RECT, WHITE);
		if(tens->state != TENS_OUT_STAT_STOP) lcdDispString(TENSTIME_POS, 100, 50, 40, time);
	}
}

void guiDisplayRunTime(Tens_t *tens)
{
	char time[8] = {0, 0, ':', 0, 0, 0};
	unsigned short lefttime = tens->runTime;
	unsigned char min, sec;
	min = lefttime / 60;
	sec = lefttime % 60;
	lcdUint2Str(min, time, 2);
	lcdUint2Str(sec, time + 3, 2);
	lcdFill(TENSTIME_RECT, WHITE);
	if(tens->state != TENS_OUT_STAT_STOP) lcdDispString(TENSTIME_POS, 100, 50, 40, time);
}

void guiUpdateTensState(Tens_t *tens)
{
	if(tens->state == TENS_RUN) lcdDispPic(TENSSTAT_RECT, gImage_run, BLACK, WHITE);
	else if(tens->state == TENS_PAUSE) lcdDispPic(TENSSTAT_RECT, gImage_pause, BLACK, WHITE);
	else lcdDispPic(TENSSTAT_RECT, gImage_stop, BLACK, WHITE);
}

void guiFwUpdateFsh(Tens_t *tens)
{
	lcdClear(WHITE);
	lcdDispString(100, 100, 180, 50, 40, "Reboot");
}
