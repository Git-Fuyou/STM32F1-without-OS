/*!-----------------------------------------------------------------------
* @file    delay.h
* @author  FuYou
* @version V1.0
* @date    2016.09.18
* @brief   延时函数声明
------------------------------------------------------------------------*/
#ifndef __DELAY_H__
#define __DELAY_H__


#include"stm32f10x.h"
//#include "stm32f4xx.h"

#define KEY_SCAN        1
#define USART_PRINT     2


extern u8 timeFlag;      ///< 时间标志
extern volatile uint16_t systick_time; ///< 毫秒级计数值


void DelayInit(void);
void delay_us( uint32_t x );
void delay_ms( uint32_t x );


#endif /*  __DELAY_H__  */

