/*!-----------------------------------------------------------------------
* @file    usart.h
* @author  FuYou
* @version V3.0
* @date    18-September-2016
* @brief   printf函数支持全部串口，通过usartChannel选择使用的串口

------------------------------------------------------------------------*/
#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "stm32f10x.h"

extern u8  usartChannel;    //用于重定义的fputc函数内选择输出串口，默认为usart1

void uart_init(void);       //串口初始化


void Usart1_init(u32 bound);
void Usart2_init(u32 bound);
void Usart3_init(u32 bound);
void Uart4_init(u32 bound);
void Uart5_init(u32 bound);

void Uart1PutString(u8 *buff, u16 num);
void Uart2PutString(u8 *buff, u16 num);
void Uart3PutString(u8 *buff, u16 num);
void Uart4PutString(u8 *buff, u16 num);
void Uart5PutString(u8 *buff, u16 num);


#endif

