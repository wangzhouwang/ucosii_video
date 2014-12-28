

#ifndef __LCD_H__
#define __LCD_H__	1
#include "stm32f4xx.h"







#define LCD_RAM    *(__IO uint16_t *) (0x6C000002)    	//µØÖ·¼Ä´æÆ÷
#define LCD_REG    *(__IO uint16_t *) (0x6C000000)	 	//Ö¸Áî¼Ä´æÆ÷

#define rw_data_prepare()               write_cmd(34)


#define LCD_WIDTH   240
#define LCD_HEIGHT  320

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0


void lcd_init(void);
void lcd_clear(unsigned short Color);




#endif
