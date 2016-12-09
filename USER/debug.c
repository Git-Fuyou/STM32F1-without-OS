/*!-----------------------------------------------------------------------
* @file    debug.c
* @author  FuYou
* @version V3.0
* @date    18-September-2016
* @brief   支持调试输出的相关函数

------------------------------------------------------------------------*/
#include "debug.h"
#include "usart.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#ifdef __PRINT__

#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}

/**
  * @brief  重定义fputc函数
  * @param  ch: 字符
  * @param  f:  
  * @retval ch  
  */
int fputc(int ch, FILE *f)
{

    while((UART4->SR & 0X40) == 0)
        ;

    UART4->DR = (u8) ch;

    return ch;
}

#endif  /*  __PRINT__  */


