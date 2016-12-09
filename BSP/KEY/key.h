/*!-----------------------------------------------------------------------
* @file    key.h
* @author  FuYou
* @version V1.0
* @date    2016-09-21
* @brief   定义按键相关引脚和操作
* @note    保证KEYx_PRESS宏在按键按下时的值为真
* @code
    //按键扫描
    if(timeFlag & KEYSCAN)
    {
        KeyScan();
        timeFlag &= ~KEYSCAN;
    }
    //扫描结果的使用
    if(single_click & KEY_OK)
    {
        按键执行的命令
        single_click &= ~KEY_OK; //清除单击
    }
* @endcode

------------------------------------------------------------------------*/
#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"      //RCC_APB2Periph_GPIOA
//#include "stm32f4xx.h"    //RCC_AHB1Periph_GPIOA
 
#define MAX_KEY_TIME 30     //长按门限，单位：KeyScan函数的调用周期

// 掩码，外部函数用
#define KEY_UP      KEY1
#define KEY_DOWN    KEY2
#define KEY_OK      KEY4
#define KEY_BACK    KEY3


/** @defgroup 按键
  * @{
  */
  
// 掩码，扫描函数用
#define KEY1        0x01
#define KEY2        0x02
#define KEY3        0x04
#define KEY4        0x08

// KEY1
#define KEY1_RCC    RCC_APB2Periph_GPIOB//GPIO时钟
#define KEY1_GPIO   GPIOB               //GPIO组
#define KEY1_Pin    GPIO_Pin_12         //GPIO管脚

// KEY2
#define KEY2_RCC    RCC_APB2Periph_GPIOB//GPIO时钟
#define KEY2_GPIO   GPIOB               //GPIO组
#define KEY2_Pin    GPIO_Pin_13         //GPIO管脚 

// KEY3
#define KEY3_RCC    RCC_APB2Periph_GPIOB//GPIO时钟
#define KEY3_GPIO   GPIOB               //GPIO组
#define KEY3_Pin    GPIO_Pin_14         //GPIO管脚 

// KEY4
#define KEY4_RCC    RCC_APB2Periph_GPIOB//GPIO时钟
#define KEY4_GPIO   GPIOB               //GPIO组
#define KEY4_Pin    GPIO_Pin_15         //GPIO管脚 

//所有用到的I/O的时钟(用"|"叠加多个I/O的时钟)
#define KEY_RCC     KEY1_RCC | KEY2_RCC | KEY3_RCC | KEY4_RCC

// 读按键
#define KEY1_PRESS  (  KEY1_GPIO->IDR & KEY1_Pin )//原子开发板这个键按下是高电平
#define KEY2_PRESS  (!(KEY2_GPIO->IDR & KEY2_Pin))
#define KEY3_PRESS  (!(KEY3_GPIO->IDR & KEY3_Pin))
#define KEY4_PRESS  (!(KEY4_GPIO->IDR & KEY4_Pin))

// 同样的读按键，更加简练（低电平按下）
#define KEY_PRESS(x) (!(x##_GPIO->IDR & x##_Pin))//读取KEYx的状态，按下表达式为1

/**
  * @}
  */
  
extern uint8_t single_click;
extern uint8_t longpress;

void KeyInit(void);     //IO初始化
void KeyScan(void);     //按键扫描函数


#endif  /* __KEY_H__ */

