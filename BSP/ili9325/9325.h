/*-----------------------------------------------------------------------
文件：LCD9325.h  
      
说明：1.ili9325复位引脚，可以硬件连接到单片机复位脚
------------------------------------------------------------------------*/
#ifndef __9325_H
#define __9325_H

#include "stm32f10x.h"

   
#define LCD_HIGHT  320	//宽
#define LCD_WIDTH  240 	//高

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END	0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

/*-----------------------------------------------------------------------
                           液晶端口定义
-----------------------------------------------------------------------*/

#define	TFT_RST	GPIO_Pin_1  //硬复位				PG1


#define TFT_RST_SET_L	GPIOG->BRR  = TFT_RST
#define TFT_RST_SET_H	GPIOG->BSRR = TFT_RST
 	

/*-----------------------------------------------------------------------
                             函数声明
-----------------------------------------------------------------------*/

void ILI9325Init(void);
void Write_Cmd_Data(unsigned char x, unsigned int y);
void Write_Data_U16(unsigned int y);
void LcdSetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1);
void LcdDrawPoint(unsigned short x, unsigned short y, unsigned int color);
u16  LcdReadPoint(u16 x, u16 y);
void ShowPhoto(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,const unsigned char *pic)  ;
void ShowRGB(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,unsigned int Color);
void ClearScreen(unsigned int bColor);
void LcdPutChar(unsigned short x, unsigned short y, char chr,unsigned char size, unsigned int fColor, unsigned int bColor);
void LcdPutFont(unsigned short x, unsigned short  y, unsigned char c[2],unsigned char size, unsigned int fColor,unsigned int bColor);
unsigned short LcdPutString(unsigned short x, unsigned short y, unsigned char *s,unsigned char size, unsigned int fColor, unsigned int bColor);

#endif
