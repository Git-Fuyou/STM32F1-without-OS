/*!-----------------------------------------------------------------------
* @file    keybord.c
* @author  FuYou
* @version V1.0
* @date    2016-10-21
* @note    
    @arg   实现按键IO初始化。按键扫描，输出键值在下面的两个全局变量里。
    @arg   矩阵按键扫描,输出有效时对应位置一，16个按键，每个对应一个位
    @arg   注意一个问题，如果行线是推挽输出的话，同一列中同时按下多个按键会导致
           这些按键对应的行线短路，比较危险，而且不能正常读出按键。所以行线最好
           用开漏输出，自己加一个外部上拉电阻。列线自然是必须加上拉电阻了。

------------------------------------------------------------------------*/
#include "keybord.h"

/*全局变量，保存按键值*/
u16 keybord_click; ///< 单击键值
u16 keybord_press; ///< 长按键值

/**
 * @brief   按键I/O初始化
 * @param   无
 * @retval  无
 * @note    F1系列不加外部电阻时只能用推挽输出
 */
void KeyBordInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( KEY_RCC , ENABLE);          //使能PORT 时钟

    //列
    GPIO_InitStructure.GPIO_Pin  = COL1_Pin;            //列1管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       //设置成上拉输入
    GPIO_Init(COL1_GPIO, &GPIO_InitStructure);          //初始化COL1
    
    GPIO_InitStructure.GPIO_Pin  = COL2_Pin;            //列2管脚
    GPIO_Init(COL2_GPIO, &GPIO_InitStructure);          //初始化COL2
    
    GPIO_InitStructure.GPIO_Pin  = COL3_Pin;            //列3管脚
    GPIO_Init(COL3_GPIO, &GPIO_InitStructure);          //初始化COL3
    
    GPIO_InitStructure.GPIO_Pin  = COL4_Pin;            //列4管脚
    GPIO_Init(COL4_GPIO, &GPIO_InitStructure);          //初始化COL4
    
    //行
    GPIO_InitStructure.GPIO_Pin  = ROW1_Pin;            //行1管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //IO口速度为50MHz(开到最高，不要怜惜那一点点功耗而拖累了I/O扫描)
    GPIO_Init(ROW1_GPIO, &GPIO_InitStructure);          //初始化ROW1

    GPIO_InitStructure.GPIO_Pin  = ROW2_Pin;            //行2管脚
    GPIO_Init(ROW2_GPIO, &GPIO_InitStructure);          //初始化ROW2
    
    GPIO_InitStructure.GPIO_Pin  = ROW3_Pin;            //行3管脚
    GPIO_Init(ROW3_GPIO, &GPIO_InitStructure);          //初始化ROW3
    
    GPIO_InitStructure.GPIO_Pin  = ROW4_Pin;            //行4管脚
    GPIO_Init(ROW4_GPIO, &GPIO_InitStructure);          //初始化ROW4
    
    keybord_click = 0;
    keybord_press = 0;
}

/**
 * @brief   按键扫描
 * @param   无
 * @retval  无
 * @note    优化级别太高可能会影响I/O正常操作（太快了反应不过来）
    @arg    全局变量keybord_press保存长按的键值，keybord_click保存单击键值。
            （它们是16位的变量，一个位保存一个按键，按下时相应位置一）

    @arg    先后出现多个按键，新的会用tmp处理，原本在tmp的会转移到tmp2中继续处理，tmp2的内容丢失。
            （也就是说，总是保存最新的两个按键，旧的自动丢弃）

    @arg    同时按下的按键，会一起保存到tmp里，松一个按键就会从tmp里删掉一个，并输出单击；
            持续到门限的会全部输出长按，然后将tmp清零，结束处理。
            （也就是说，同时按下的按键，不管数目多少，长按、单击或者长按单击混合，都能正常处理）

    @arg    函数靠周期性地调用实现消抖（并不会主动延时消抖），被调用的时间间隔稍大于按键抖动时间为佳

 * @code
        //按键扫描
        if(timeFlag & KEYSCAN)
        {
            KeyBordScan();
            timeFlag &= ~KEYSCAN;
        }
        //扫描结果的使用
        if(keybord_click & KEY_0)
        {
            //处理代码
            keybord_click &= ~KEY_0;//清零单击标志（不清除则会一直保留，直到被手动清除）
        }
        if(keybord_press & KEY_0)
        {
            //处理代码（可以在这累计时间，得到更长的长按）
            //长按无需清零，松开时自动清零（清零后不能再进入，不清则在按键松开前都能进入）
        }
* @endcode

*/
void KeyBordScan(void)
{
    volatile static u8  i = 0, j = 0;
    volatile static u16 trg = 0, cont = 0;
    volatile static u16 tmp = 0, tmp2 = 0;
    u16 read = 0;

    //a:读取按键，检测到对应按键处于按下状态，则把对应的位置一  （ read |= KEY_xx ）

    ROW_ENABLE(ROW1);//当前行再输出0（必须保证先关闭上一行，再打开下一行的顺序，否则 COL1 这列会出错）
    if(COL_PRESS(COL1))read |= KEY_1;
    if(COL_PRESS(COL2))read |= KEY_2;
    if(COL_PRESS(COL3))read |= KEY_3;
    if(COL_PRESS(COL4))read |= KEY_4;  // 下面的讨论是在假设KEY_1按下的情况。
    ROW_DISABLE(ROW1);
    ROW_ENABLE(ROW2);                  // 之前是对BSRR寄存器一次性写入四个ROW的全部I/O状态，
    if(COL_PRESS(COL1))read |= KEY_5;  // “ROW1置一”和“ROW2置零”同时发生，I/O的翻转是有个过程的。
    if(COL_PRESS(COL2))read |= KEY_6;  // 默认优化（leave2）时，在三条汇编指令后就读IDR判断KEY_5，
    if(COL_PRESS(COL3))read |= KEY_7;  // 此时ROW1还未被完全置一，读出来的是ROW1、COL1，也就是KEY_1。
    if(COL_PRESS(COL4))read |= KEY_8;  // 所以出现了按下KEY_1时KEY_5也出现的情况。
    ROW_DISABLE(ROW2);                 // 处理办法就是，先把所有行拉高，再拉低当前行（不加延时的办法）。
    ROW_ENABLE(ROW3);
    if(COL_PRESS(COL1))read |= KEY_9;  // 还有个办法，优化改成leave0，此时六条汇编指令后才读，不会出错。
    if(COL_PRESS(COL2))read |= KEY_10; // 虽然也有可能是BSRR是间接操作I/O导致的输出延迟，
    if(COL_PRESS(COL3))read |= KEY_11; // 加上读取间隔太短，才导致出错。但把“ROW1置一”和“ROW2置零”
    if(COL_PRESS(COL4))read |= KEY_12; // 同时进行本身就不严谨，所以不去探究BSRR的延迟有多长了。
    ROW_DISABLE(ROW3);
    ROW_ENABLE(ROW4);
    if(COL_PRESS(COL1))read |= KEY_13;
    if(COL_PRESS(COL2))read |= KEY_14;
    if(COL_PRESS(COL3))read |= KEY_15;
    if(COL_PRESS(COL4))read |= KEY_16;
    ROW_DISABLE(ROW4);
    
    //b:得到新按下的按键

    trg = read & (read ^ cont);     // 按键触发值，只在按下按键后第一次执行keyred时对应位为1，之后自动清零

    if(trg)
    {
        if(tmp)
        {
            tmp2 = tmp;             // tmp总是保存最新的按键
            j = i;                  // 当tmp非0的时候，tmp2被丢弃，tmp转移到tmp2
            tmp = trg;
            i = 0;
        }
        else
        {
            tmp = trg;
            i = 0;
        }
    }
    //c:单击长按处理
    if(tmp)                         // 按键处理被触发
    {
        if(i == 30)                 // 到达长按门限
        {
            keybord_press |= tmp & read; // 达到长按门限，为长按（此时依然按下的才算）

            tmp = 0;
            i = 0;          // 处理完成，清除变量
        }
        if(tmp & (tmp ^ read))      // 有按键被松开，能进来的必然是单击
        {
            keybord_click |= tmp & (tmp ^ read);

            tmp &= ~keybord_click;  // 把单击的按键值从按键处理中删除

            if(!tmp)i = 0;          // 没有按键了，清除变量(单击长按同时发生时，单击结束了并不能退出)
        }
        i++;                        // 按键处理时间计数
    }
    if(tmp2)
    {
        if(j == 30)                 // 到达长按门限
        {
            keybord_press |= tmp2 & read; // 达到长按门限，为长按（此时依然按下的才算）

            tmp2 = 0;
            j = 0;         // 处理完成，清除变量
        }
        if(tmp2 & (tmp2 ^ read))    // 有按键被松开，能进来的必然是单击
        {
            keybord_click |= tmp2 & (tmp2 ^ read);

            tmp2 &= ~keybord_click; // 把单击的按键值从按键处理中删除

            if(!tmp2) j = 0;        // 没有按键了，清除变量
        }
        j++;                        // 按键处理时间计数
    }

    //d:后续处理
    keybord_press &= read;          // 按键松开，长按清零
    cont = read;                    // 保存按键值，长按的按键对应位一直为1
}

