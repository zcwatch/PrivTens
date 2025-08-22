#include "board.h"


#if HW_V11
//KEY DEFINE
#define KEYIO1_PIN			GPIO_PIN_10
#define KEYIO1_PORT		 	GPIOB

#define KEYIO2_PIN			GPIO_PIN_11
#define KEYIO2_PORT		 	GPIOB

#define KEYUP_PIN			GPIO_PIN_12
#define KEYUP_PORT			GPIOA

#define KEYCON_PIN			GPIO_PIN_11
#define KEYCON_PORT			GPIOA

#define KEYDOWN_PIN			GPIO_PIN_10
#define KEYDOWN_PORT		GPIOA

#define KEYPWR_PIN			GPIO_PIN_6
#define KEYPWR_PORT			GPIOB

//
#define MODEDET_PIN			GPIO_PIN_9
#define MODEDET_PORT		GPIOA

#define CHGSTAT_PIN			GPIO_PIN_13
#define CHGSTAT_PORT		GPIOC

#define BATADC_PIN			GPIO_PIN_0
#define BATADC_PORT			GPIOA

#define MOTORWARN_PIN		GPIO_PIN_4
#define MOTORWARN_PORT		GPIOA

#define HVDAC_PIN			GPIO_PIN_5
#define HVDAC_PORT			GPIOA

#define HVADC_PIN			GPIO_PIN_6
#define HVADC_PORT			GPIOA

#define HVEN_PIN			GPIO_PIN_14
#define HVEN_PORT			GPIOB

//HC595
#define HC595SDA_PIN		GPIO_PIN_7
#define HC595SDA_PORT		GPIOA

#define HC595LA_PIN			GPIO_PIN_0
#define HC595LA_PORT		GPIOB

#define HC595CLK_PIN		GPIO_PIN_12
#define HC595CLK_PORT		GPIOB


#define MOTOREN_PIN			GPIO_PIN_1
#define MOTOREN_PORT		GPIOB

#define HEATEN_PIN			GPIO_PIN_14
#define HEATEN_PORT			GPIOB

#define PWREN_PIN			GPIO_PIN_7
#define PWREN_PORT			GPIOB

#define TEMPDET_PIN			GPIO_PIN_9
#define TEMPDET_PORT		GPIOA
#else
//KEY DEFINE
#define KEYIO1_PIN			GPIO_PIN_13
#define KEYIO1_PORT		 	GPIOC

#define KEYIO2_PIN			GPIO_PIN_11
#define KEYIO2_PORT		 	GPIOB

#define KEYUP_PIN			GPIO_PIN_8
#define KEYUP_PORT			GPIOB

#define KEYCON_PIN			GPIO_PIN_7
#define KEYCON_PORT			GPIOB

#define KEYDOWN_PIN			GPIO_PIN_6
#define KEYDOWN_PORT		GPIOB

#define KEYPWR_PIN			GPIO_PIN_4
#define KEYPWR_PORT			GPIOB

//
#define MODEDET_PIN			GPIO_PIN_9
#define MODEDET_PORT		GPIOA

#define CHGSTAT_PIN			GPIO_PIN_14
#define CHGSTAT_PORT		GPIOC

#define BATADC_PIN			GPIO_PIN_0
#define BATADC_PORT			GPIOA

#define MOTORWARN_PIN		GPIO_PIN_1
#define MOTORWARN_PORT		GPIOA

#define HVDAC_PIN			GPIO_PIN_5
#define HVDAC_PORT			GPIOA

#define HVADC_PIN			GPIO_PIN_6
#define HVADC_PORT			GPIOA

#define HVEN_PIN			GPIO_PIN_14
#define HVEN_PORT			GPIOB

//HC595
#define HC595SDA_PIN		GPIO_PIN_7
#define HC595SDA_PORT		GPIOA

#define HC595LA_PIN			GPIO_PIN_0
#define HC595LA_PORT		GPIOB

#define HC595CLK_PIN		GPIO_PIN_1
#define HC595CLK_PORT		GPIOB


#define MOTOREN_PIN			GPIO_PIN_2
#define MOTOREN_PORT		GPIOB

#define HEATEN_PIN			GPIO_PIN_10
#define HEATEN_PORT			GPIOB

#define PWREN_PIN			GPIO_PIN_9
#define PWREN_PORT			GPIOB

#define TEMPDET_PIN			GPIO_PIN_15
#define TEMPDET_PORT		GPIOB
#endif


unsigned char halKeyIO1(void)
{
	return (GPIO_ISTAT(KEYIO1_PORT)&(KEYIO1_PIN)) ? 1 : 0;
}

unsigned char halKeyIO2(void)
{
	return (GPIO_ISTAT(KEYIO2_PORT)&(KEYIO2_PIN)) ? 1 : 0;
}

unsigned char halKeyTensUp(void)
{
	return (GPIO_ISTAT(KEYUP_PORT)&(KEYUP_PIN)) ? 0 : 1; //gpio_input_bit_get(KEYUP_PORT, KEYUP_PIN);
}

unsigned char halKeyTensDown(void)
{
	return (GPIO_ISTAT(KEYDOWN_PORT)&(KEYDOWN_PIN)) ? 0 : 1; //gpio_input_bit_get(KEYDOWN_PORT, KEYDOWN_PIN);
}

unsigned char halKeyTensConfirm(void)
{
	return (GPIO_ISTAT(KEYCON_PORT)&(KEYCON_PIN)) ? 0 : 1; //gpio_input_bit_get(KEYCON_PORT, KEYCON_PIN);
}

unsigned char halKeyPower(void)
{
	return (GPIO_ISTAT(KEYPWR_PORT)&(KEYPWR_PIN)) ? 1 : 0;
}

unsigned char halReadChargeSta(void)
{
	return (GPIO_ISTAT(CHGSTAT_PORT)&(CHGSTAT_PIN)) ? 0 : 1; //gpio_input_bit_get(CHGSTAT_PORT, CHGSTAT_PIN);
}

void halPowerEn(unsigned char state)
{
	state ? (GPIO_BOP(PWREN_PORT) = PWREN_PIN) : (GPIO_BC(PWREN_PORT) = PWREN_PIN);
}

void halMotor(unsigned char state)
{
	state ? (GPIO_BOP(MOTOREN_PORT) = MOTOREN_PIN) : (GPIO_BC(MOTOREN_PORT) = MOTOREN_PIN);
}

void halMotorWarn(unsigned char state)
{
	state ? (GPIO_BOP(MOTORWARN_PORT) = MOTORWARN_PIN) : (GPIO_BC(MOTORWARN_PORT) = MOTORWARN_PIN);
}

void halHVEnable(unsigned char state)
{
	state ? (GPIO_BOP(HVEN_PORT) = HVEN_PIN) : (GPIO_BC(HVEN_PORT) = HVEN_PIN);
}

void halHC595SDA(unsigned char state)
{
	state ? (GPIO_BOP(HC595SDA_PORT) = HC595SDA_PIN) : (GPIO_BC(HC595SDA_PORT) = HC595SDA_PIN);
}

void halHC595CLK(unsigned char state)
{
	state ? (GPIO_BOP(HC595CLK_PORT) = HC595CLK_PIN) : (GPIO_BC(HC595CLK_PORT) = HC595CLK_PIN);
}

void halHC595LA(unsigned char state)
{
	state ? (GPIO_BOP(HC595LA_PORT) = HC595LA_PIN) : (GPIO_BC(HC595LA_PORT) = HC595LA_PIN);
}

void halHeat(unsigned char state)
{
	state ? (GPIO_BOP(HEATEN_PORT) = HEATEN_PIN) : (GPIO_BC(HEATEN_PORT) = HEATEN_PIN);
}

static void halGpioInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_AF);
	//gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP,ENABLE); 
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE); 
	
	//KEY
	gpio_init(KEYUP_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYUP_PIN);
	gpio_init(KEYDOWN_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYDOWN_PIN);
	gpio_init(KEYCON_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYCON_PIN);
	gpio_init(KEYPWR_PORT, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, KEYPWR_PIN);
	
	//charge state
	gpio_init(CHGSTAT_PORT, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, CHGSTAT_PIN);
	
	
	//power enable
	gpio_init(PWREN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, PWREN_PIN);
	//motor en
	gpio_init(MOTOREN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MOTOREN_PIN);
	gpio_init(MOTORWARN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, MOTORWARN_PIN);
	//Heat
	gpio_init(HEATEN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HEATEN_PIN);
	//hv enable
	gpio_init(HVEN_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HVEN_PIN);
	//HC595
	gpio_init(HC595SDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HC595SDA_PIN);
	gpio_init(HC595CLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HC595CLK_PIN);
	gpio_init(HC595LA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, HC595LA_PIN);
	
	halPowerEn(1);
}

uint16_t adcValue[4];
static void ad_dma_init(void)
{
	dma_parameter_struct dma_data_parameter;
	dma_deinit(DMA0, DMA_CH0);
	
	dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
	dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
	dma_data_parameter.memory_addr  = (uint32_t)(&adcValue);
	dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
  dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
  dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
  dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
  dma_data_parameter.number       = ADCH_SIZE;
  dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
  dma_init(DMA0, DMA_CH0, &dma_data_parameter);

  dma_circulation_enable(DMA0, DMA_CH0);
	dma_channel_enable(DMA0, DMA_CH0);
}

static void halAdcInit(void)
{
	uint32_t i;
	rcu_periph_clock_enable(RCU_ADC0);
	rcu_periph_clock_enable(RCU_DMA0);
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
	
	gpio_init(BATADC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, BATADC_PIN);
	gpio_init(HVADC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, HVADC_PIN);
	
	ad_dma_init();
	
	adc_mode_config(ADC_MODE_FREE);
	adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
	adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
	adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
	adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADCH_SIZE);
	
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_239POINT5);
	adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_6, ADC_SAMPLETIME_239POINT5);
	
	adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
	adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

	adc_dma_mode_enable(ADC0);
	adc_enable(ADC0);

	for(i = 0; i < 1000000; i++) ;
	adc_calibration_enable(ADC0);
#if 1
	adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
#endif
}

unsigned short halGetBatAdc(void)
{
	return adcValue[0];
}

unsigned short halGetHVAdc(void)
{
	return adcValue[1];
}

static void halDacInit(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_DAC);
	gpio_init(HVDAC_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, HVDAC_PIN);

	dac_deinit();
	dac_trigger_disable(DAC1);
	dac_wave_mode_config(DAC1, DAC_WAVE_DISABLE);
	dac_output_buffer_enable(DAC1);
	dac_enable(DAC1);
	dac_data_set(DAC1, DAC_ALIGN_12B_R, 0);
}

void halSetDac1(unsigned short val)
{
	dac_data_set(DAC1, DAC_ALIGN_12B_R, val);
}

static void systickConfig(void)
{
	if(SysTick_Config(SystemCoreClock / 1000)) {
		while(1) {
		}
	}
}

#if WDT_EN
static void wdtInit(void)
{
	rcu_osci_on(RCU_IRC40K);
	while(SUCCESS != rcu_osci_stab_wait(RCU_IRC40K)){
	}
	fwdgt_config(2*500,FWDGT_PSC_DIV64);
	fwdgt_enable();
}

void feedDog(void)
{
	fwdgt_counter_reload();
}
#endif

void halHardwareInit(void)
{
	systickConfig();
#if WDT_EN
	wdtInit();
#endif
	halGpioInit();
	halAdcInit();
	halDacInit();
	btuartInit();
	timerInit();
}

unsigned char halScanKey(void)
{
	unsigned char keyValue = 0;
#if 0		//test
	gpio_init(KEYIO1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYIO1_PIN);
	gpio_bit_reset(KEYIO1_PORT, KEYIO1_PIN);
	gpio_bit_set(KEYIO1_PORT, KEYIO1_PIN);
	
	gpio_init(KEYIO2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYIO2_PIN);
	gpio_bit_reset(KEYIO2_PORT, KEYIO2_PIN);
	gpio_bit_set(KEYIO2_PORT, KEYIO2_PIN);
#endif
	//step 1,io1 & io2 set 1
	gpio_init(KEYIO1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYIO1_PIN);
	gpio_init(KEYIO2_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYIO2_PIN);
	if((!gpio_input_bit_get(KEYIO1_PORT, KEYIO1_PIN)) && (!gpio_input_bit_get(KEYIO2_PORT, KEYIO2_PIN))) keyValue = 3;
	else if(!gpio_input_bit_get(KEYIO1_PORT, KEYIO1_PIN)) keyValue = 2;
	else if(!gpio_input_bit_get(KEYIO2_PORT, KEYIO2_PIN)) keyValue = 1;
	else {
		gpio_init(KEYIO1_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYIO1_PIN);
		gpio_bit_reset(KEYIO1_PORT, KEYIO1_PIN);
		if(!gpio_input_bit_get(KEYIO2_PORT, KEYIO2_PIN)) {
			gpio_init(KEYIO1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYIO1_PIN);
			gpio_init(KEYIO2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYIO2_PIN);
			gpio_bit_reset(KEYIO2_PORT, KEYIO2_PIN);
			if(!gpio_input_bit_get(KEYIO1_PORT, KEYIO1_PIN)) keyValue = 4;
			else keyValue = 6;
		} else {
			gpio_init(KEYIO1_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, KEYIO1_PIN);
			gpio_init(KEYIO2_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, KEYIO2_PIN);
			gpio_bit_reset(KEYIO2_PORT, KEYIO2_PIN);
			if(!gpio_input_bit_get(KEYIO1_PORT, KEYIO1_PIN)) keyValue = 5;
			else return 0;
		}
	}
	
	return keyValue;
}

int halFlashWrite(unsigned int addr, unsigned char *data, int len)
{
	int i;
	unsigned short dat;
	len = (len % 2 == 0) ? (len / 2) : (len / 2 + 1);
	fmc_unlock();
	for(i = 0; i < len; i++) {
		dat = ((unsigned short*)data)[i];
		fmc_halfword_program(addr, dat);
		addr += 2;
	}
	fmc_lock();
	return 1;
}

void halFlashErasePage(unsigned int addr)
{
	fmc_unlock();
	fmc_page_erase(addr);
	fmc_lock();
}

void halFlashErase(unsigned int addr, unsigned int size)
{
	
}


