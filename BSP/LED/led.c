/*!-----------------------------------------------------------------------
* @file    led.c
* @author  FuYou
* @version V1.0
* @date    18-September-2016
* @brief   初始化 LED 端口
------------------------------------------------------------------------*/
#include "led.h"



/*!
* @brief  初始化 LED IO
* @param  无
* @retval 无
* @note   几乎无需修改
*/
void LED_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( LED_RCC, ENABLE);//使能端口时钟

    GPIO_InitStructure.GPIO_Pin = LED0_Pin;             //LED0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz
    GPIO_Init(LED0_GPIO, &GPIO_InitStructure);          //根据设定参数初始化GPIO

    GPIO_InitStructure.GPIO_Pin = LED1_Pin;             //LED1
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO口速度为50MHz
    GPIO_Init(LED1_GPIO, &GPIO_InitStructure);          //根据设定参数初始化GPIO
}


