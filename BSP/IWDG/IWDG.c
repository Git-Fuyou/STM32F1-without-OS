/*!-----------------------------------------------------------------------
* @file    IWDG.c
* @author  FuYou
* @version V1.0
* @date    2016.09.18
* @brief   实现独立看门狗初始化函数

------------------------------------------------------------------------*/
#include "IWDG.h"

/*!
* @brief  独立看门狗初始化
* @param  time_ms：重装载值，12位寄存器，0~4096
* @retval 无

* @note   预分频值：     \n
    IWDG_Prescaler_4     \t   ((uint8_t)0x00)  \t   0.1 ~ 409.6ms   \n
    IWDG_Prescaler_8     \t   ((uint8_t)0x01)  \t   0.2 ~ 819.2ms   \n
    IWDG_Prescaler_16    \t   ((uint8_t)0x02)  \t   0.4 ~ 1638.4ms  \n
    IWDG_Prescaler_32    \t   ((uint8_t)0x03)  \t   0.8 ~ 3276.8ms  \n
    IWDG_Prescaler_64    \t   ((uint8_t)0x04)  \t   1.6 ~ 6553.5ms  \n
    IWDG_Prescaler_128   \t   ((uint8_t)0x05)  \t   3.2 ~ 13107.2ms \n
    IWDG_Prescaler_256   \t   ((uint8_t)0x06)  \t   6.4 ~ 26214.4ms
*/
void IWDG_init(u32 time_ms)
{
    u8 i;
    u32 tmp;

    time_ms *=10;
    tmp=4096;
    //计算预分频值
    for(i=0; i<7; i++)
    {
        tmp<<=i;
        if(time_ms<(tmp-100))break;
    }
    //计算重装载值
    tmp=time_ms/(2<<i);

    //使能寄存器IWDG_PR和IWDG_RLR写操作
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    //设置IWDG预分频值
    IWDG_SetPrescaler(i);

    //设置IWDG重装载值
    IWDG_SetReload(tmp);

    //重装载IWDG计数器
    IWDG_ReloadCounter();

    //使能IWDG
    IWDG_Enable();

}
