/*------------------------------------------------------------------------
文件：menuio.h  
      
说明：无
------------------------------------------------------------------------*/
#ifndef __MenuIO_H
#define __MenuIO_H	

#include "stm32f10x.h"

void ClearRow(void);
void ClearLine(u8 x,u8 y,u8 fontSize);
void OLED_ShowPic(u8 x,u8 y,u8 const * p);

#endif
