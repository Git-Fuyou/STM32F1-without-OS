/*!-----------------------------------------------------------------------
* @file    led.h
* @author  FuYou
* @version V1.0
* @date    2016-09-21
* @note    ##用于连接两个字符串为一个完整的宏
* @code    
           //翻转LED0，括号里的参数不能写错:LEDx,x=0,1,2...
           LED_TRG(LED0);
* @endcode
------------------------------------------------------------------------*/
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"

/** @defgroup LED option
  * @{
  */
 
// LED0
#define LED0_RCC        RCC_APB2Periph_GPIOC
#define LED0_GPIO       GPIOC
#define LED0_Pin        GPIO_Pin_0

// LED1
#define LED1_RCC        RCC_APB2Periph_GPIOC
#define LED1_GPIO       GPIOC
#define LED1_Pin        GPIO_Pin_1

//LED时钟
#define LED_RCC         LED0_RCC | LED1_RCC

//LED操作
#define LED_ON(x)       x##_GPIO->BRR  = x##_Pin//LEDx输出0
#define LED_OFF(x)      x##_GPIO->BSRR = x##_Pin//LEDx输出1
#define LED_TRG(x)      x##_GPIO->ODR ^= x##_Pin//LEDx翻转

/**
  * @}
  */

void LED_Init(void);//初始化


#endif /* __LED_H__ */

