/*!-----------------------------------------------------------------------
* @file    IWDG.h
* @author  FuYou
* @version V1.0
* @date    2016.09.18
* @brief   独立看门狗，初始化函数只要以需要的溢出时间作为入参即可（单位ms）

------------------------------------------------------------------------*/
#ifndef __IWDG_H__
#define __IWDG_H__

#include "stm32f10x.h"



void IWDG_init(u32 time_ms);


#endif /* __IWDG_H__ */
