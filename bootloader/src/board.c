#include "board.h"


static void systick_config(void)
{
	if(SysTick_Config(SystemCoreClock / 1000)) {
		while(1) {
		}
	}
	//NVIC_SetPriority(SysTick_IRQn, 0);
}

void feed_dog(void)
{
	fwdgt_counter_reload();
}

static void wdt_init(void)
{
	rcu_osci_on(RCU_IRC40K);
	while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
	}
	fwdgt_config(2*500, FWDGT_PSC_DIV64);
	fwdgt_enable();
}

void hardware_init(void)
{
	//systick_config();
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);	//POWER HOLD OFF
	gpio_bit_set(GPIOB, GPIO_PIN_8);
	wdt_init();
}
