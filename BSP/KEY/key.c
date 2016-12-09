/*!-----------------------------------------------------------------------
* @file    key.c
* @author  FuYou
* @version V1.0
* @date    2016-09-21
* @brief   实现按键IO初始化，按键扫描
* @note
*   - 按键动作分别保存在longpress，single_click全局变量中
*   - 例如P0接了八个独立按键，允许按键接任意I/O，使用连续的I/O可以简化a这步
        单击P0^0 ，则  single_click=0x01,使用后软件清零
        长按P0^0 ，则  longpress =0x01,放开按键自动清零
*   - 函数靠周期性地调用实现消抖（并不会主动延时消抖）
        函数被调用的时间间隔稍大于按键抖动时间为佳

------------------------------------------------------------------------*/
#include "key.h"

/** @brief 全局变量，保存按键值,u8类型最大支持8个独立按键*/
uint8_t single_click;   ///< 单击
uint8_t longpress;      ///< 长按

/**
 * @brief   按键I/O初始化
 * @param   无
 * @retval  无
 */
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( KEY_RCC, ENABLE);       //使能时钟

    GPIO_InitStructure.GPIO_Pin  = KEY1_Pin;        //原子的开发板比较特殊，按下时高电平
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
    GPIO_Init(KEY1_GPIO, &GPIO_InitStructure);      //初始化GPIO

    GPIO_InitStructure.GPIO_Pin  = KEY2_Pin;        //
    GPIO_Init(KEY2_GPIO, &GPIO_InitStructure);      //初始化GPIO

    GPIO_InitStructure.GPIO_Pin  = KEY3_Pin;        //
    GPIO_Init(KEY3_GPIO, &GPIO_InitStructure);      //初始化GPIO

    GPIO_InitStructure.GPIO_Pin  = KEY4_Pin;        //
    GPIO_Init(KEY4_GPIO, &GPIO_InitStructure);      //初始化GPIO


    single_click = 0;
    longpress = 0;
}

/**
 * @brief   按键扫描
 * @param   无
 * @retval  无
 * @note    硬件无关
 */
void KeyScan(void)
{
    static unsigned char j = 0, tmp = 0;
    static unsigned char trg = 0;
    static unsigned char cont = 0;

    unsigned char read;

    //a:读取按键，检测到对应按键处于按下状态，则把对应的位置一  （ read |= KEY_xx ）
    read = 0;
    if(KEY_PRESS(KEY1))  read |= KEY1;//原子开发板这个键按下是高电平
    if(KEY_PRESS(KEY2))  read |= KEY2;
    if(KEY_PRESS(KEY3))  read |= KEY3;
    if(KEY_PRESS(KEY4))  read |= KEY4;

    //b:得到新按下的按键

    trg = read & (read ^ cont);     // 按键触发值，只在按下按键后第一次执行keyred时对应位为1，之后自动清零

    if(trg)                         //
    {
        if(tmp && (j < MAX_KEY_TIME)) // 如果还有按键未处理完就来了新按键，强制未处理完的按键为单击
            single_click |= tmp;
        tmp = trg;                  // 临时保存触发按键值   触发 步骤c
        j = 0;
    }

    //c:单击长按处理
    if(tmp)                         // 按键处理被触发
    {
        j++;                        // 按键处理时间计数
        if(j == MAX_KEY_TIME)       // 到达长按门限
        {
            longpress = tmp;        // 长按
            tmp = 0;
            j = 0;
        }
        if(tmp & (tmp ^ read))      // 有按键被释放
        {
            single_click |= tmp & (tmp ^ read);
            tmp &= ~single_click;   // 把单击的按键值从按键处理中删除
            if(!tmp)j = 0;          // 清除变量
        }
    }

    //d:后续处理
    longpress &= read;              // 按键松开，长按清零
    cont = read;                    // 保存按键值，长按的按键对应位一直为1
}

