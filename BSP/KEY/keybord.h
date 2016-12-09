/*!-----------------------------------------------------------------------
* @file    keybord.h
* @author  FuYou
* @version V1.0
* @date    2016-10-21
* @note    可以用任意引脚，比如ROW1_EN用PA1，ROW2_EN用PC5，COL1_IN用PB15等等，
           只要自己改下面的宏就可以了。

------------------------------------------------------------------------*/
#ifndef __KEYBODE_H__
#define __KEYBODE_H__

#include "stm32f10x.h"

//所有用到的I/O的时钟(用"|"叠加多个I/O的时钟)
#define KEY_RCC         RCC_APB2Periph_GPIOA


/** @defgroup 按键引脚定义
  * @{
  */

#define ROW1_GPIO       GPIOA
#define ROW1_Pin        GPIO_Pin_0

#define ROW2_GPIO       GPIOA
#define ROW2_Pin        GPIO_Pin_1

#define ROW3_GPIO       GPIOA
#define ROW3_Pin        GPIO_Pin_2

#define ROW4_GPIO       GPIOA
#define ROW4_Pin        GPIO_Pin_3

#define COL1_GPIO       GPIOA
#define COL1_Pin        GPIO_Pin_4

#define COL2_GPIO       GPIOA
#define COL2_Pin        GPIO_Pin_5

#define COL3_GPIO       GPIOA
#define COL3_Pin        GPIO_Pin_6

#define COL4_GPIO       GPIOA
#define COL4_Pin        GPIO_Pin_7

/**
  * @}
  */


/** @defgroup 行、列操作定义
  * @{
  */
  
//行操作定义
#define ROW_DISABLE(x)  x##_GPIO->BSRR = x##_Pin    //ROWx无效,输出1
#define ROW_ENABLE(x)   x##_GPIO->BRR  = x##_Pin    //ROWx有效,输出0

//列操作定义(当按键按下时，COL_IN(x)为真)
#define COL_PRESS(x)    (!(x##_GPIO->IDR & x##_Pin))//读取COLx的状态，按下表达式为1

/**
  * @}
  */


/** @defgroup 每个按键的掩码
  * @{
  */

#define KEY_1           0x0001
#define KEY_2           0x0002
#define KEY_3           0x0004
#define KEY_4           0x0008
#define KEY_5           0x0010
#define KEY_6           0x0020
#define KEY_7           0x0040
#define KEY_8           0x0080
#define KEY_9           0x0100
#define KEY_10          0x0200
#define KEY_11          0x0400
#define KEY_12          0x0800
#define KEY_13          0x1000
#define KEY_14          0x2000
#define KEY_15          0x4000
#define KEY_16          0x8000

/**
  * @}
  */

/** @defgroup 按键相关变量及函数的外部声明
  * @{
  */

extern u16 keybord_click;
extern u16 keybord_press;


void KeyBordInit(void);     //IO初始化
void KeyBordScan(void);     //按键扫描函数

/**
  * @}
  */

#endif /* __KEYBODE_H__ */

