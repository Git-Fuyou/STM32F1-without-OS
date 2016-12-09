/*!-----------------------------------------------------------------------
* @file    main.c
* @author  FuYou
* @version V1.0
* @date    2016.09.21
* @brief   模板(演示：键盘扫描，LED闪烁，适用于欣世纪开发板)

------------------------------------------------------------------------*/
#include "include.h"


int main(void)
{
    
#ifdef DEBUG_IN_RAM
    SCB->VTOR = NVIC_VectTab_RAM;//ram中调试中断，需要重映射中断向量表
#endif
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
    LED_Init();
    DelayInit();
    KeyInit();
    uart_init();

    while(1)
    {
        if(timeFlag & KEY_SCAN)
        {
            KeyScan();
            timeFlag &= ~KEY_SCAN;
        }
        // 按键单击演示
//        if(single_click & KEY_UP)
//        {
////            printf("key_up\n");
//            LED_TRG(LED1);
//            single_click &= ~KEY_UP;// 用完记得清零
//        }
        if(single_click)
        {
//            printf("key_up\n");
            LED_TRG(LED1);
            single_click = 0;// 用完记得清零
        }
    }
}

