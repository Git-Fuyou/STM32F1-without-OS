/*!-----------------------------------------------------------------------
* @file    delay.c
* @author  FuYou
* @version V1.0
* @date    2016.09.18
* @brief   实现微秒和毫秒延时函数
------------------------------------------------------------------------*/
#include "led.h"
#include "delay.h"
#include "keybord.h"

volatile uint16_t systick_time; ///< 毫秒级计数值，不用volatile的话，在leave2优化时会卡死在延时的while里。

/*!
* @brief  初始化 SysTick
* @param  无
* @retval 无
* @note   根据需要配置，如下所示：
    @arg    SystemCoreClock/1000    1ms中断一次
    @arg    SystemCoreClock/10000   100us中断一次
    @arg    SystemCoreClock/100000  10us中断一次
*/
void DelayInit(void)
{

    SysTick_Config( SystemCoreClock / 1000 ); //配置SysTick，1ms

}

/**
* @brief  延时任意个1us
* @param  x：微秒数，最好不要超过1ms，即1000
* @retval 无
*/
void delay_us( uint32_t x )
{

    x *= (SystemCoreClock / 1000000);

    while( x-- );

}

/**
* @brief  延时任意个1ms
* @param  x：毫秒数，最大65535
* @retval 无
*/
void delay_ms( uint32_t x )
{

    x = (systick_time + x) & 0xffff;

    while( x != systick_time);

}


u8 timeFlag = 0; ///< 时间标志
/**
* @brief  滴答定时中断处理函数
* @param  无
* @retval 无
*/
void SysTick_Handler(void)
{

    systick_time ++;    // 16位，满回零，循环记数

    if((systick_time & 0x1F) == 0x1F)   //31ms扫描一次按键
    {
        timeFlag |= KEY_SCAN ;
    }

    if((systick_time & 0x1FF) == 0x1FF) //511ms翻转一次LED
    {
        LED_TRG(LED0);
    }

}


