#ifndef __BATTERY_H
#define __BATTERY_H

typedef unsigned char(*BatteryChargeStateFunc)(void);
typedef unsigned short(*BatteryGetAdcFunc)(void);


typedef struct {
	unsigned char isCharge:1;
	unsigned char en:1;
	unsigned char per;
	unsigned short adc;

	BatteryChargeStateFunc getChargeState;
	BatteryGetAdcFunc getAdc;
} Battery_t;

void batteryInit(Battery_t *bat, BatteryGetAdcFunc getAdc, BatteryChargeStateFunc getChargeState);
void batteryProcess(Battery_t *bat);

#endif
