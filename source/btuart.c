#include "btuart.h"

#if 1		//HW_V11
#define BTSTA_PIN			GPIO_PIN_2
#define BTSTA_PORT			GPIOB
#else
#define BTSTA_PIN			GPIO_PIN_4
#define BTSTA_PORT			GPIOA
#endif

void btuartInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_USART1);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	
	usart_deinit(USART1);
	usart_baudrate_set(USART1, 115200U);
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
	usart_enable(USART1);
	
	nvic_irq_enable(USART1_IRQn, 0, 0);
	//usart_interrupt_enable(USART1, USART_INT_TBE);
	usart_interrupt_enable(USART1, USART_INT_RBNE);
	
	//bluetooth state
	gpio_init(BTSTA_PORT, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, BTSTA_PIN);
}

int btSenddata(unsigned char *data, int len)
{
	int i;
	if(!btReadBtSta()) return 0;
	
	for(i=0; i<len; i++) {
		while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
		usart_data_transmit(USART1, data[i]);
	}
	return len;
}

unsigned char btReadBtSta(void)
{
	return (GPIO_ISTAT(BTSTA_PORT)&(BTSTA_PIN)) ? 1 : 0; //gpio_input_bit_get(BTSTA_PORT, BTSTA_PIN); //((BTSTA_PORT->IDR & BTSTA_PIN) == 0) ? 0 : 1;
}


