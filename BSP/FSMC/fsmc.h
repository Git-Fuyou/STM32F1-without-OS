/*------------------------------------------------------------------------
文件：fsmc.h  
      
说明：定义按键掩码，使用时如：if(single_click & KEY_OK)
------------------------------------------------------------------------*/
#ifndef __FSMC_H
#define __FSMC_H	 

#include "stm32f10x.h"
 
//LCD地址结构体
typedef struct
{
	vu16 LCD_REG;
	vu16 LCD_RAM;
} LCD_TypeDef;
//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A10作为数据命令区分线 
//注意设置时STM32内部会右移一位对其! 			    
#define LCD_BASE        ((u32)(0x6C000000 | 0x000003FE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


void fsmc_init(void);

#endif





