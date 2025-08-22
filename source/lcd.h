#ifndef __LCD_H
#define __LCD_H

#define LCD_COL                                 320
#define LCD_ROW                                 240    


/* color value */
#define WHITE         	                  	    0xFFFF
#define BLACK         	                  	    0x0000
#define BLUE         	                   		0x001F
#define BRED                             		0xF81F
#define GRED 			                        0xFFE0
#define GBLUE			                        0x07FF
#define RED           	                  	    0xF800
#define TOUCH_RED      	                 		0x00F8
#define MAGENTA       	                  	    0xF81F
#define GREEN         	                  	    0x07E0
#define CYAN          	                  	    0x7FFF
#define YELLOW        	                  	    0xFFE0
#define BROWN 			                        0xBC40
#define BRRED 			                        0xFC07
#define GRAY  			                        0x8430

#define DARKBLUE      	                  	    0x01CF
#define LIGHTBLUE      	                 		0x7D7C
#define GRAYBLUE       	                 		0x5458

#define LIGHTGREEN     	                 		0x841F
#define LIGHTGRAY                        		0xEF5B
#define LGRAY 			                        0xC618

#define LGRAYBLUE                        		0xA651
#define LBBLUE                           		0x2B12


void lcdInit(void);
void lcdPower(unsigned char state);
void lcdClear(unsigned short color);
void lcdDrawLine(unsigned short sx, unsigned short sy, unsigned short len);
void lcdFill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color);
void lcdDispString(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, char *p);

void lcdDispPic(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, const unsigned char *pic, unsigned short hcolor, unsigned short lcolor);
void lcdUint2Str(unsigned int num, char *str, unsigned char bit);

#endif
