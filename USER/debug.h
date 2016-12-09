/*!-----------------------------------------------------------------------
* @file    debug.h
* @author  FuYou
* @version V3.0
* @date    17-September-2016
* @brief   包含了所有DEBUG相关的组件

------------------------------------------------------------------------*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "stdio.h"
#include "stm32f10x.h"



/*!
 @brief 调试输出
*/
#if defined (USE_DEBUG)

#define DEBUG(format,...)   printf("File: "__FILE__", Line: %d : "format"\n", __LINE__, ##__VA_ARGS__)

#ifndef __PRINT__   //定义了调试输出，则必须定义print打印
#define __PRINT__
#endif

#else

#define DEBUG(format,...)   ((void)0)

#endif



/*!
 @brief print打印初始化
*/
#ifdef __PRINT__

#define PRINT_INIT()    Uart4_init(230400)

#else

#define PRINT_INIT()    ((void)0)

#endif


#endif /* __DEBUG_H__ */

