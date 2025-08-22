#include "pwm.h"

#define TIMER0_CH0CV			0x40012C34
#define PWM_PERIOD				300000

//SPWM DEFINE
#define SPWMNEN_PIN			GPIO_PIN_12
#define SPWMNEN_PORT		GPIOA

#define SPWMN_PIN			GPIO_PIN_8
#define SPWMN_PORT			GPIOA

#define SPWMP_PIN			GPIO_PIN_13
#define SPWMP_PORT			GPIOB

#define PLATEDET_PIN		GPIO_PIN_15
#define PLATEDET_PORT		GPIOB

unsigned short pwmPeriod;



uint16_t buff[1];


static void timer0DmaInit(void)
{
	dma_parameter_struct dma_init_struct;
	
	rcu_periph_clock_enable(RCU_DMA0);
	
	dma_deinit(DMA0, DMA_CH1);
	
	dma_init_struct.periph_addr = (uint32_t)TIMER0_CH0CV;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_addr = (uint32_t)buff;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.number = 1;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_init(DMA0, DMA_CH1, &dma_init_struct);
	
	dma_circulation_disable(DMA0, DMA_CH1);
	nvic_irq_enable(DMA0_Channel1_IRQn, 0, 0);
	dma_interrupt_enable(DMA0,DMA_CH1, DMA_INT_FTF);
	dma_channel_disable(DMA0, DMA_CH1);
}

static void timer0Init(void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	timer_break_parameter_struct timer_breakpara;
	
	rcu_periph_clock_enable(RCU_TIMER0);
	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	
	gpio_init(SPWMN_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPWMN_PIN);	//SPWM
	gpio_init(SPWMP_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPWMP_PIN);

	//TENS_EN
	gpio_init(SPWMNEN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPWMNEN_PIN);
	tmrSpwmNEn(0);
	
	
	timer_deinit(TIMER0);
	
	pwmPeriod = SystemCoreClock / PWM_PERIOD;
	//pwm_max_ccr = pwmPeriod * 0.9;
	timer_initpara.prescaler = 0;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = pwmPeriod - 1;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER0, &timer_initpara);
	
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.outputnstate = TIMER_CCXN_ENABLE;
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
	timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
	timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, pwmPeriod / 2);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
#if 0
	timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocintpara);
	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, pwmPeriod / 2);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
#endif
#if 1
	timer_breakpara.runoffstate      = TIMER_ROS_STATE_ENABLE;
	timer_breakpara.ideloffstate     = TIMER_IOS_STATE_ENABLE ;
	timer_breakpara.deadtime         = 5;
	timer_breakpara.breakpolarity    = TIMER_BREAK_POLARITY_LOW;
	timer_breakpara.outputautostate  = TIMER_OUTAUTO_ENABLE;
	timer_breakpara.protectmode      = TIMER_CCHP_PROT_OFF;
	timer_breakpara.breakstate       = TIMER_BREAK_DISABLE;
	timer_break_config(TIMER0,&timer_breakpara);
#endif
	
	timer_primary_output_config(TIMER0, ENABLE);
	
	//timer_dma_transfer_config(TIMER0, TIMER_DMACFG_DMATA_CH0CV, TIMER_DMACFG_DMATC_1TRANSFER);
	timer_dma_enable(TIMER0,TIMER_DMA_CH0D);
	timer_channel_dma_request_source_select(TIMER0, TIMER_DMAREQUEST_UPDATEEVENT);
	
	timer_auto_reload_shadow_enable(TIMER0);
	
#if 0
	timer_interrupt_enable(TIMER0, TIMER_INT_UP);
	nvic_irq_enable(TIMER0_UP_IRQn, 1, 1);
#endif
	timer_disable(TIMER0);
}

static void timer11Init(void)
{
	unsigned short period;
	timer_parameter_struct timer_initpara;
	timer_ic_parameter_struct timer_icinitpara;
	
	rcu_periph_clock_enable(RCU_TIMER2);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);
	
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_15);
	//nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
  //nvic_irq_enable(TIMER2_IRQn, 1, 1);
	
	timer_deinit(TIMER11);
	
	period = SystemCoreClock / 2000;
	
	timer_initpara.prescaler = 1;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = period - 1;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER2, &timer_initpara);
	
	timer_icinitpara.icpolarity = TIMER_IC_POLARITY_RISING;
	timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
	timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
	timer_icinitpara.icfilter = 0x0;
	timer_input_capture_config(TIMER11, TIMER_CH_1, &timer_icinitpara);
	
	timer_auto_reload_shadow_enable(TIMER11);
	
	timer_enable(TIMER11);	
}

static void timer2Init(void)
{
	unsigned short period;
	timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable(RCU_TIMER2);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);
	

	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
	nvic_irq_enable(TIMER2_IRQn, 1, 1);
	
	timer_deinit(TIMER2);
	
	period = SystemCoreClock / 1000;
	
	timer_initpara.prescaler = 1;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = period - 1;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER2, &timer_initpara);
	
	timer_auto_reload_shadow_enable(TIMER2);
	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH3);
	timer_interrupt_enable(TIMER2, TIMER_INT_CH3);
	
	timer_enable(TIMER2);	
}

void timerInit(void)
{
	timer0DmaInit();
	timer0Init();
	timer11Init();
	timer2Init();
}

void tmrSpwmNEn(unsigned char state)
{
	state ? (GPIO_BOP(SPWMNEN_PORT) = SPWMNEN_PIN) : (GPIO_BC(SPWMNEN_PORT) = SPWMNEN_PIN);;
}

void spwmEnable(void)
{
	timer_dma_transfer_config(TIMER0, TIMER_DMACFG_DMATA_CH0CV, TIMER_DMACFG_DMATC_1TRANSFER);
	timer_enable(TIMER0);
}

void spwmDisable(void)
{
	TIMER_CH0CV(TIMER0) = pwmPeriod / 2;
	timer_dma_disable(TIMER0,TIMER_DMA_UPD);
	timer_disable(TIMER0);
}

void tmrGetTimer11Count(unsigned int *value)
{
	*value = TIMER_CH1CV(TIMER11);
}
