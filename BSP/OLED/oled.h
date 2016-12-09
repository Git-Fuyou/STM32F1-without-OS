/*------------------------------------------------------------------------
文件：oled.h  
      
说明：OLED相关引脚及参数定义
------------------------------------------------------------------------*/
#ifndef __OLED_H
#define __OLED_H	

#include "stm32f10x.h"   

//OLED模式设置
//0: 4线串行模式  （模块的BS1，BS2均接GND）
//1: 并行8080模式 （模块的BS1，BS2均接VCC）
#define OLED_MODE 	1 

#define OLED_HIGHT  64	//宽
#define OLED_WIDTH  128 //高

#define OLED_CMD  	0	//写命令
#define OLED_DATA 	1	//写数据
#define OLED_NORMAL 1 	//正常显示
#define OLED_REVER  0 	//  反显		    						  
//-----------------OLED端口定义----------------  					   
//使用4线串行接口时使用
#define OLED_CS 	GPIO_Pin_2
#define OLED_RS		GPIO_Pin_3
#define OLED_SDIN	GPIO_Pin_4
#define OLED_SCLK	GPIO_Pin_5

#define OLED_CS_Clr()  	GPIOE->BRR  = OLED_CS//输出0
#define OLED_CS_Set()  	GPIOE->BSRR = OLED_CS//输出1

#define OLED_RS_Clr() 	GPIOE->BRR  = OLED_RS//输出0
#define OLED_RS_Set() 	GPIOE->BSRR = OLED_RS//输出1

#define OLED_SCLK_Clr() GPIOE->BRR  = OLED_SCLK//输出0
#define OLED_SCLK_Set() GPIOE->BSRR = OLED_SCLK//输出1

#define OLED_SDIN_Clr() GPIOE->BRR  = OLED_SDIN//输出0
#define OLED_SDIN_Set() GPIOE->BSRR = OLED_SDIN//输出1
		     

//-----------------OLED函数声明---------------- 
void OLED_Init(void);
void OLED_Clear(void);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);  		    
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowFont(u8 x,u8 y,u8 *font,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8*str,u8 size,u8 mode);


#endif  
	 

