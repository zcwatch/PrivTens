#include "lcd.h"
#include "gd32f30x.h"


#define LCD_WIDTH			320
#define LCD_HEIGHT		240



//LCD DEFINE
#if 1
#define LCMLEDA_PIN			GPIO_PIN_1
#define LCMLEDA_PORT		GPIOA

#define LCMSDA_PIN			GPIO_PIN_5
#define LCMSDA_PORT			GPIOB

#define LCMSCL_PIN			GPIO_PIN_3
#define LCMSCL_PORT			GPIOB

#define LCMCS_PIN				GPIO_PIN_15
#define LCMCS_PORT			GPIOA

#define LCMRST_PIN			GPIO_PIN_9
#define LCMRST_PORT			GPIOB

#define LCMRS_PIN				GPIO_PIN_8
#define LCMRS_PORT			GPIOB
#else
#define LCMLEDA_PIN			GPIO_PIN_15
#define LCMLEDA_PORT		GPIOC

#define LCMSDA_PIN			GPIO_PIN_5
#define LCMSDA_PORT			GPIOB

#define LCMSCL_PIN			GPIO_PIN_3
#define LCMSCL_PORT			GPIOB

#define LCMCS_PIN				GPIO_PIN_15
#define LCMCS_PORT			GPIOA

#define LCMRST_PIN			GPIO_PIN_11
#define LCMRST_PORT			GPIOA

#define LCMRS_PIN				GPIO_PIN_10
#define LCMRS_PORT			GPIOA
#endif

//extern const unsigned char asc2_1206[][12];
//extern const unsigned char asc2_1608[][16];
extern const unsigned char asc2_2412[][36];
extern const unsigned char asc2_4020[][100];

static void delayMs(unsigned int n)
{
	unsigned int i, k;
	for(i = 0; i < n; i++) {
		for(k = 0; k < 5000; k++) ;
	}
}

static void lcdHwInit(void)
{
	spi_parameter_struct spi_init_struct;

	rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_SPI2);
	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_3);

	spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode = SPI_MASTER;
	spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
	spi_init_struct.nss = SPI_NSS_SOFT;
	spi_init_struct.prescale = SPI_PSC_2;
	spi_init_struct.endian = SPI_ENDIAN_MSB;
	spi_init(SPI2, &spi_init_struct);
	
	spi_enable(SPI2);

	gpio_init(LCMCS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCMCS_PIN);
	gpio_init(LCMLEDA_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LCMLEDA_PIN);
	gpio_init(LCMRST_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCMRST_PIN);
	gpio_init(LCMRS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LCMRS_PIN);
	
	GPIO_BOP(LCMLEDA_PORT) = LCMLEDA_PIN;
}

static void lcdSpiWrite(unsigned char data)
{
	while(RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_TBE));
	spi_i2s_data_transmit(SPI2, data);
	while(SET == spi_i2s_flag_get(SPI2, SPI_FLAG_TRANS));
}

static void lcdWriteReg(unsigned char reg)
{
	GPIO_BC(LCMRS_PORT) = LCMRS_PIN;
	lcdSpiWrite(reg);
	GPIO_BOP(LCMRS_PORT) = LCMRS_PIN;
}

static void lcdWriteData(unsigned char data)
{
	lcdSpiWrite(data);
}

static void lcdWriteData16(unsigned short data)
{
	lcdSpiWrite(data >> 8);
	lcdSpiWrite(data & 0xff);
}

void lcdSetCursor(unsigned short x, unsigned short y)
{
	lcdWriteReg(0x2A);
	lcdWriteData(x >> 8);
	lcdWriteData(x & 0xff);
	
	lcdWriteReg(0x2B);
	lcdWriteData(y >> 8);
	lcdWriteData(y & 0xff);
	
	lcdWriteReg(0x2C);
}

void lcdSetWindow(unsigned int xstart, unsigned int ystart, unsigned int xend, unsigned int yend)
{
	lcdWriteReg(0x2A);
	lcdWriteData(xstart >> 8);
	lcdWriteData(xstart & 0xff);
	lcdWriteData(xend >> 8);
	lcdWriteData(xend & 0xff);
	
	lcdWriteReg(0x2B);
	lcdWriteData(ystart >> 8);
	lcdWriteData(ystart & 0xff);
	lcdWriteData(yend >> 8);
	lcdWriteData(yend & 0xff);
	
	lcdWriteReg(0x2C);
}

void lcdDrawPoint(unsigned short x, unsigned short y, unsigned short color)
{
	lcdSetCursor(x, y);
	lcdWriteData16(color);
}

void lcdDispPic(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, const unsigned char *pic, unsigned short hcolor, unsigned short lcolor)
{
	unsigned short i, len, k;
	len = (ex - sx) * (ey - sy) / 8;
	lcdSetWindow(sx, sy, ex, ey);
	for(i = 0; i < len; i++) {
		for(k = 0; k < 8; k++) {
			if((pic[i] << k) & 0x80) lcdWriteData16(hcolor);
			else lcdWriteData16(lcolor);
		}
	}
}

void lcdFill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color)
{
	unsigned short i, k;
	lcdSetWindow(sx, sy, ex, ey);
	for(i = 0; i < ey - sy; i++) {
		for(k = 0; k < ex - sx; k++) {
			lcdWriteData16(color);
		}
	}	
}

void lcdClear(unsigned short color)
{
	unsigned short i, k;
	lcdSetWindow(0, 0, LCD_WIDTH, LCD_HEIGHT);
	//lcdSetCursor(0, 0);
	for(i = 0; i < LCD_HEIGHT; i++) {
		for(k = 0; k < LCD_WIDTH; k++) {
			lcdWriteData16(color);
		}
	}
}

void lcdDispChar(unsigned short x, unsigned short y, unsigned char num, unsigned char size, unsigned char mode)
{
	unsigned char tmp, k, i;
	unsigned short y0 = y;
	unsigned char csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
	num = num - ' ';
	
	for(i = 0; i < csize; i++) {
		if(size == 40) {
			tmp = asc2_4020[num][i];
		} else if(size == 24) {
			tmp = asc2_2412[num][i];
//		} else if(size == 16) {
//			tmp = asc2_1608[num][i];
//		} else if(size == 12) {
//			tmp = asc2_1206[num][i];
		} else {
			return;
		}
		
		for(k = 0; k < 8; k++) {
#if 1
			if(tmp & 0x80) {
				if(mode) lcdDrawPoint(x, y, BLACK);
				else lcdDrawPoint(x, y, WHITE);
			} else {
				if(mode) lcdDrawPoint(x, y, WHITE);
				else lcdDrawPoint(x, y, BLACK);
			}
#else
			if(tmp & 0x80) {
				lcdDrawPoint(x, y, BLACK);
			} else if(mode == 0) {
				lcdDrawPoint(x, y, WHITE);
			}
#endif
			tmp <<= 1;
			y++;
			if(y >= LCD_HEIGHT) return;
			if((y - y0) == size) {
				y = y0;
				x++;
				if(x >= LCD_WIDTH) return;
				break;
			}
		}
	}
}

void lcdDispString(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, char *p)
{
	unsigned char x0 = x;
	width += x;
	height += y;
	
	while((*p <= '~') && (*p >= ' ')) {
		if(x >= width) {
			x = x0;
			y += size;
		}
		if(y >= height) {
			break;
		}
		lcdDispChar(x, y, (unsigned char)*p, size, 1);
		x += (size / 2);
		p++;
	}
}

void lcdDispString1(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, char *p)
{
	unsigned char x0 = x;
	width += x;
	height += y;
	
	while((*p <= '~') && (*p >= ' ')) {
		if(x >= width) {
			x = x0;
			y += size;
		}
		if(y >= height) {
			break;
		}
		lcdDispChar(x, y, (unsigned char)*p, size, 0);
		x += (size / 2);
		p++;
	}
}

void lcdDrawLine(unsigned short sx, unsigned short sy, unsigned short len)
{
	unsigned short i, k;
	lcdSetWindow(sx, sy, sx + len, sy + 1);
	for(i = 0; i < 2; i++) {
		for(k = 0; k < len; k++) {
			lcdWriteData16(BLACK);
		}
	}
}

void lcdInit(void)
{
	lcdHwInit();

	lcdPower(0);
	
	//Reset
	GPIO_BOP(LCMRST_PORT) = LCMRST_PIN;
	delayMs(200);
	GPIO_BC(LCMRST_PORT) = LCMRST_PIN;
	delayMs(200);
	GPIO_BOP(LCMRST_PORT) = LCMRST_PIN;
	
	GPIO_BC(LCMCS_PORT) = LCMCS_PIN;
	delayMs(200);
	
	
	lcdWriteReg(0xcf);
	lcdWriteData(0x00);
	lcdWriteData(0xC1);
	lcdWriteData(0x30);
		
	lcdWriteReg(0xED);
	lcdWriteData(0x64);
	lcdWriteData(0x03);
	lcdWriteData(0x12);
	lcdWriteData(0x81);
		
	lcdWriteReg(0xE8);
	lcdWriteData(0x85);
	lcdWriteData(0x00);
	lcdWriteData(0x7A); //0x79
	
	lcdWriteReg(0xCB);
	lcdWriteData(0x39);
	lcdWriteData(0x2C);
	lcdWriteData(0x00);
	lcdWriteData(0x34);
	lcdWriteData(0x02);
		
	lcdWriteReg(0xF7);
	lcdWriteData(0x20);
		
	lcdWriteReg(0xEA);
	lcdWriteData(0x00);
	lcdWriteData(0x00);
		
	lcdWriteReg(0xC0);  //power control
	lcdWriteData(0x21); //vrh[5:0]
		
	lcdWriteReg(0xC1);  //power control
	lcdWriteData(0x11); //sap[2:0];bt[3:0]
		
	lcdWriteReg(0xC5);  //vcm control
	lcdWriteData(0x25);
	lcdWriteData(0x32);
		
	lcdWriteReg(0xC7);  //vcm control
	lcdWriteData(0xaa);
	
	lcdWriteReg(0x36);
	lcdWriteData(0xC8);
	
	lcdWriteReg(0xB6);  //display function control
	lcdWriteData(0x0A);
	lcdWriteData(0xA2);
	
	lcdWriteReg(0xB1);
	lcdWriteData(0x00);
	lcdWriteData(0x1B);

	lcdWriteReg(0xF2);  //3gamma function disable
	lcdWriteData(0x00);
		
	lcdWriteReg(0x26);  //gamma curve selected
	lcdWriteData(0x01);
	
	lcdWriteReg(0x3a);
	lcdWriteData(0x55);
	
	lcdWriteReg(0xE0);  //set gamma
	lcdWriteData(0x0F);
	lcdWriteData(0x2D);
	lcdWriteData(0x0E);
	lcdWriteData(0x08);
	lcdWriteData(0x12);
	lcdWriteData(0x0A);
	lcdWriteData(0x3D);
	lcdWriteData(0x95);
	lcdWriteData(0x31);
	lcdWriteData(0x04);
	lcdWriteData(0x10);
	lcdWriteData(0x09);
	lcdWriteData(0x09);
	lcdWriteData(0x0d);
	lcdWriteData(0x00);
	
	lcdWriteReg(0xE1);  //set gamma
	lcdWriteData(0x00);
	lcdWriteData(0x12);
	lcdWriteData(0x17);
	lcdWriteData(0x03);
	lcdWriteData(0x0d);
	lcdWriteData(0x05);
	lcdWriteData(0x2C);
	lcdWriteData(0x44);
	lcdWriteData(0x41);
	lcdWriteData(0x05);
	lcdWriteData(0x0F);
	lcdWriteData(0x0A);
	lcdWriteData(0x30);
	lcdWriteData(0x32);
	lcdWriteData(0x0F);
	
	lcdWriteReg(0x21);
	lcdWriteReg(0x11);

  	lcdWriteReg(0x29);  //display on
	delayMs(120);
			
	//lcdClear(0xf800);
	//lcdSetWindow(0, 0, 100, 100);
	//lcdFill(0, 0, 100, 100, 0xaaaa);
	//lcdDispString(100, 50, 150, 24, 24, "Hello World");
}

void lcdPower(unsigned char state)
{
	if(state) GPIO_BOP(LCMLEDA_PORT) = LCMLEDA_PIN;
	else GPIO_BC(LCMLEDA_PORT) = LCMLEDA_PIN;
}

void lcdUint2Str(unsigned int num, char *str, unsigned char bit)
{
	unsigned char i;
	
	for(i = 0; i < bit; i++) {
		str[bit - 1 - i] = (num % 10) + 0x30;
		num /= 10;
	}
}
