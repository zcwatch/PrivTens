#include "tens.h"
#include "board.h"
#include "tensgui.h"
#include "lcd.h"



unsigned char tick = 0, tickCnt = 0;
unsigned int secCnt = 0, secTick = 0, seconds = 0;


void SysTick_Handler(void)
{
	if(++tickCnt == TICK_CNT) {
		tick = 1;
		tickCnt = 0;
	}
	
	if(++secCnt == 1000) {
		secTick = 1;
		seconds++;
		secCnt = 0;
		
	}
	pwmoutIntervalProcess(&privateTens.tensOutput.pwmOutput);
	
	vibProcess(&privateTens.vib);
	heatProcess(&privateTens.heat);
}

void USART1_IRQHandler(void)
{
	unsigned char data;
	if(usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET) {
		data = usart_data_receive(USART1);
		//protoPutData(&privateTens.proto, data);
		tprotoRxData(&privateTens.proto, &data, 1);
	}
}

void TIMER2_IRQHandler(void)
{
	if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_CH3)) {
		timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH3);
		mtProcess(&privateTens.motorTip);
	}
}

void DMA0_Channel1_IRQHandler(void)
{
	dma_channel_disable(DMA0, DMA_CH1);
	//timer_disable(TIMER0);
	//pwmChannelEnable(1, 0, 0);
	dma_flag_clear(DMA0, DMA_CH1, DMA_INTF_FTFIF); 
	if(!pwmoutHasInterval(&privateTens.tensOutput.pwmOutput)) {
		carrierPwmBuffReadDone(&privateTens.tensOutput.pwmOutput.carrier);
		privateTens.tensOutput.pwmOutput.updateCallback(&privateTens.tensOutput.pwmOutput);
	}
}

#if TENS_TEST
extern unsigned char tmpBuff[];
unsigned char prescData[] = {
#if 1
	0x00, 0x81, 0x00, 0x72, 0x01, 0x01, 0x01, 0x00, 0x00, 0x0f, 0x10, 0xF6, 0x0a, 0xa0, 0x13, 0x88, 0x00, 0x00, 0x06, 0xF6,
	0x0a, 0xa0, 0x00, 0xc8, 0x00, 0x00, 0xc8, 0xF6, 0x0a, 0xa0, 0x07, 0xd0, 0x00, 0x00, 0x0f, 0xF6, 0x0a, 0xa0, 0x00, 0x85,
	0x00, 0x00, 0xff, 0xF6, 0x0a, 0xa0, 0x13, 0x88, 0x00, 0x00, 0x06, 0xF6, 0x0a, 0xa0, 0x00, 0x64, 0x00, 0x00, 0xff, 0xF6,
	0x0a, 0xa0, 0x03, 0xe8, 0x00, 0x00, 0x1e, 0xF6, 0x0a, 0xa0, 0x00, 0x50, 0x00, 0x00, 0xff, 0xF6, 0x0a, 0xa0, 0x07, 0xd0,
	0x00, 0x00, 0x0f, 0xF6, 0x0a, 0xa0, 0x00, 0x64, 0x00, 0x00, 0xff, 0xF6, 0x0a, 0xa0, 0x13, 0x88, 0x00, 0x00, 0x06, 0xF6,
	0x0a, 0xa0, 0x00, 0x43, 0x00, 0x00, 0xff, 0xF6, 0x0a, 0xa0, 0x07, 0xd0, 0x00, 0x00, 0x0f, 0xF6, 0x0a, 0xa0, 0x00, 0x32,
	0x00, 0x00, 0xff, 0xF6, 0x0a, 0xa0, 0x03, 0xe8, 0x00, 0x00, 0x1e, 0x16
#else
	0x00, 0x14, 0x00, 0x72, 0x01, 0x01, 0xff, 0x00, 0x00, 0x01, 0x10, 0xf2, 0x0a, 0xa0, 0x00, 0x64, 0x00, 0x64, 0x00, 0x64, 0x03, 0xaa
//	0x00, 0x14, 0x00, 0x72, 
//	0x01, 0x01, 0xff, 0x00, 0x00, 
//	0x01, 0x10, 
//	0xf7, 0x0a, 0xa0, 0x01, 0x64, 0x00, 0x64, 0xff
#endif
};
#endif

int main(void)
{
#ifdef RELEASE
	SCB->VTOR = APP_ADDR;
#endif
	halHardwareInit();
	lcdInit();
	
	tensInit(&privateTens);

	//check power key
#if 1
	while(1) {
		if(tick) {
			tick = 0;
			if(seconds == 1) {
#if 0 //test
				halPowerEn(1);
#else
				if(halKeyPower()) tensPowerUp(&privateTens);
				else halPowerEn(0);
#endif
				break;
			}
#if WDT_EN
			feedDog();
#endif			
		}
	}
#else
	tensPowerUp(&privateTens);
#endif	

	//test
#if TENS_TEST
	tensLoadPrescData(&privateTens, 1);
	tensoutSetPrescription(&privateTens.tensOutput, tmpBuff, sizeof(prescData));
	tensStartOutput(&privateTens);
	privateTens.tensLevel = 1;
	tensSetLevel(&privateTens, privateTens.tensLevel);
#endif

	while(1) {
		if(tick) {
			tick = 0;
			tensProcess(&privateTens);
			//second deal
			if(secTick) {
				secTick = 0;
				tensRunTimeUpdate(&privateTens);
				tensLevelUpProcess(&privateTens);
			}
#if WDT_EN
			feedDog();
#endif
		}
		prescoutProcess(&privateTens.tensOutput.prescOutput);
		pwmoutProcess(&privateTens.tensOutput.pwmOutput);
		tprotoDecode(&privateTens.proto);
	}
}
