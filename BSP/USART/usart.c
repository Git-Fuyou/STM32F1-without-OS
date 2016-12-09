/*!-----------------------------------------------------------------------
* @file    usart.c
* @author  FuYou
* @version V3.0
* @date    18-September-2016
* @brief   全部串口支持printf
           全部串口支持连续发送数据块，UartxPutString(char* buff,u16 num)
           两个输出一起使用时，DMA发送可能出现乱码。

------------------------------------------------------------------------*/
#include "usart.h"


u8  usartChannel = 1; ///< 用于重定义的fputc函数内选择输出串口，默认为usart1
u8  Res;              // 临时用,接收


/*------------------------------------------------------------------------
功能：串口总初始化

------------------------------------------------------------------------*/
void uart_init()
{
    Usart1_init(115200);//9600会乱码
    Usart2_init(115200);//9600会乱码
    Usart3_init(115200);//9600会乱码
    Uart4_init(115200); //9600会乱码
    Uart5_init(115200); //9600会乱码

}

/**
  * @brief  串口1中断
  * @param  无
  * @retval 无
  
  * @note   
  */
void USART1_IRQHandler(void)
{

    if(USART1->SR & 0x20)  //接收中断
    {
        Res = (u8)USART1->DR;
    }
}

/**
  * @brief  串口2中断
  * @param  无
  * @retval 无
  
  * @note   
  */
void USART2_IRQHandler(void)
{

    if(USART2->SR & 0x20)  //接收中断
    {
        Res = (u8)USART2->DR;
    }
}

/**
  * @brief  串口3中断
  * @param  无
  * @retval 无
  
  * @note   
  */
void USART3_IRQHandler(void)
{

    if(USART3->SR & 0x20)  //接收中断
    {
        Res = (u8)USART3->DR;
    }
}

/**
  * @brief  串口4中断
  * @param  无
  * @retval 无
  
  * @note   
  */
void UART4_IRQHandler(void)                 //串口4中断服务程序
{
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
    {
        Res = USART_ReceiveData(UART4); //读取接收到的数据

    }
}

/**
  * @brief  串口5中断服务程序
  * @param  无
  * @retval 无
  * @note   通过全局的指针变量和计数变量完成连续发送，不支持DMA
  */
char *uartSendPoint;    //串口发送指针
u16  uartSendNum = 0;   //串口发送剩余字节数
void UART5_IRQHandler(void)
{
    if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)  //发送中断
    {
        if(uartSendNum)
        {
            UART5->DR = (u8) * (uartSendPoint++); //未发送完则写入
            uartSendNum--;
            if(uartSendNum == 0)USART_ITConfig(UART5, USART_IT_TXE, DISABLE); //关闭发送缓冲区空中断
        }
    }
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
    {
    
        Res = USART_ReceiveData(UART5); //读取接收到的数据
    }
}

/**
  * @brief  串口1 DMA中断服务程序
  * @param  无
  * @retval 无
  */
void DMA1_Channel4_IRQHandler(void)
{
    //清除标志位
//  DMA_ClearITPendingBit(DMA1_IT_TC4);
    DMA1->IFCR = DMA1_IT_TC4;   //比上面的函数节约100多ns,无代价
    //发送完成
//    com[COM_1].busy=COM_FINISH;
}

/**
  * @brief  串口2 DMA中断服务程序
  * @param  无
  * @retval 无
  */
void DMA1_Channel7_IRQHandler(void)
{
    //清除标志位
    DMA1->IFCR = DMA1_IT_TC7;
    //发送完成
//    com[COM_2].busy=COM_FINISH;
}

/**
  * @brief  串口3 DMA中断服务程序
  * @param  无
  * @retval 无
  */
void DMA1_Channel2_IRQHandler(void)
{
    //清除标志位
    DMA1->IFCR = DMA1_IT_TC2;
    //发送完成
//    com[COM_HUB].busy=COM_FINISH;
}

/**
  * @brief  串口4 DMA中断服务程序
  * @param  无
  * @retval 无
  */
void DMA2_Channel4_5_IRQHandler(void)
{
    //清除标志位
    DMA_ClearITPendingBit(DMA2_IT_TC5);
    //发送完成
    
}

/*------------------------------------------------------------------------
功能：串口1发送数据块
入参：buff，数据地址；num，数据量（字节为单位）。
说明：使用DMA发送
------------------------------------------------------------------------*/
void Uart1PutString(u8 *buff, u16 num)
{
    //  USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);    //使能串口1 DMA发送
    DMA_Cmd(DMA1_Channel4, DISABLE);    //关闭DMA通道
    while((USART1->SR & 0X80) == 0)
        ;//等待发送寄存器空
    DMA1_Channel4->CMAR = (u32)buff;    //源地址
    DMA_SetCurrDataCounter(DMA1_Channel4, num); //传输数量
    DMA_Cmd(DMA1_Channel4, ENABLE);     //开启DMA通道

}
/*------------------------------------------------------------------------
功能：串口2发送数据块
入参：buff，数据地址；num，数据量（字节为单位）。
说明：使用DMA发送
------------------------------------------------------------------------*/
void Uart2PutString(u8 *buff, u16 num)
{
    //  USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);    //使能串口2 DMA发送
    DMA_Cmd(DMA1_Channel7, DISABLE);    //关闭DMA通道
    while((USART2->SR & 0X80) == 0)
        ;//等待发送寄存器空
    DMA1_Channel7->CMAR = (u32)buff;    //源地址
    DMA_SetCurrDataCounter(DMA1_Channel7, num); //传输数量
    DMA_Cmd(DMA1_Channel7, ENABLE);     //开启DMA通道
}
/*------------------------------------------------------------------------
功能：串口3发送数据块
入参：buff，数据地址；num，数据量（字节为单位）。
说明：使用DMA发送
------------------------------------------------------------------------*/
void Uart3PutString(u8 *buff, u16 num)
{
    //  USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);    //使能串口3 DMA发送
    DMA_Cmd(DMA1_Channel2, DISABLE);    //关闭DMA通道
    while((USART3->SR & 0X80) == 0)
        ;//等待发送寄存器空
    DMA1_Channel2->CMAR = (u32)buff;    //源地址
    DMA_SetCurrDataCounter(DMA1_Channel2, num); //传输数量
    DMA_Cmd(DMA1_Channel2, ENABLE);     //开启DMA通道
}
/*------------------------------------------------------------------------
功能：串口4发送数据块
入参：buff，数据地址；num，数据量（字节为单位）。
说明：使用DMA发送
------------------------------------------------------------------------*/
void Uart4PutString(u8 *buff, u16 num)
{
    //  USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE); //使能串口4 DMA发送
    DMA_Cmd(DMA2_Channel5, DISABLE);    //关闭DMA通道
    while((UART4->SR & 0X80) == 0)
        ;//等待发送寄存器空
    DMA2_Channel5->CMAR = (u32)buff;    //源地址
    DMA_SetCurrDataCounter(DMA2_Channel5, num); //传输数量
    DMA_Cmd(DMA2_Channel5, ENABLE);     //开启DMA通道
}
/*------------------------------------------------------------------------
功能：串口5发送数据块
入参：buff，数据地址；num，数据量（字节为单位）。
说明：使用发送中断
------------------------------------------------------------------------*/
void Uart5PutString(u8 *buff, u16 num)
{
    while((UART5->SR & 0X80) == 0)
        ;//等待发送寄存器空
    UART5->DR = (u8) * buff;

    uartSendNum = num - 1;
    uartSendPoint = (char *)buff + 1;
    USART_ITConfig(UART5, USART_IT_TXE, ENABLE);    //开启发送缓冲区空中断

}
/*------------------------------------------------------------------------
功能：串口1初始化
入参：boundRate，波特率，直接写，例如115200
说明：开启DMA发送
------------------------------------------------------------------------*/
void Usart1_init(u32 boundRate)
{
    //GPIO端口设置
    DMA_InitTypeDef  DMA_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA9

    //USART1_RX   GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART1 初始化设置

    USART_InitStructure.USART_BaudRate = boundRate;         //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART1, &USART_InitStructure);       //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //开启串口接受中断

    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  //使能串口1 DMA发送

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel4);
    //  DMA_InitStruct.DMA_BufferSize=10;                   //传输次数，启动发送时重置
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;     //传输方向，内存到外设
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //关闭内存到内存模式
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;          //正常模式，非循环
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //源数据宽度
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //源地址自增
    //  DMA_InitStruct.DMA_MemoryBaseAddr=(u32)dat;         //源地址，启动发送时重置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&USART1->DR); //外设地址
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址不变
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;  //通道优先级，中等
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    //  DMA_Cmd(DMA1_Channel4, ENABLE);                 //启动发送时使能
    USART_Cmd(USART1, ENABLE);                      //使能串口1

}
/*------------------------------------------------------------------------
功能：串口2初始化
入参：boundRate，波特率，直接写，例如115200
说明：开启DMA发送
------------------------------------------------------------------------*/
void Usart2_init(u32 boundRate)
{
    //GPIO端口设置
    DMA_InitTypeDef  DMA_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //使能USART2时钟

    //USART2_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA2

    //USART2_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA3

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART2 初始化设置

    USART_InitStructure.USART_BaudRate = boundRate;         //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART2, &USART_InitStructure);       //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //开启串口接收中断
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  //使能串口2 DMA发送

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel7);
    //  DMA_InitStruct.DMA_BufferSize=10;                   //传输次数，启动发送时重置
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;     //传输方向，内存到外设
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //关闭内存到内存模式
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;          //正常模式，非循环
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //源数据宽度
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //源地址自增
    //  DMA_InitStruct.DMA_MemoryBaseAddr=(u32)dat;         //源地址，启动发送时重置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&USART2->DR); //外设地址
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址不变
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;  //通道优先级，中等
    DMA_Init(DMA1_Channel7, &DMA_InitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

    //  DMA_Cmd(DMA1_Channel7, ENABLE);                 //启动发送时使能
    USART_Cmd(USART2, ENABLE);                      //使能串口2

}
/*------------------------------------------------------------------------
功能：串口3初始化
入参：boundRate，波特率，直接写，例如115200
说明：开启DMA发送
------------------------------------------------------------------------*/
void Usart3_init(u32 boundRate)
{
    //GPIO端口设置
    DMA_InitTypeDef  DMA_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //使能USART3时钟

    //USART3_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB10

    //USART3_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB11

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART3 初始化设置

    USART_InitStructure.USART_BaudRate = boundRate;         //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART3, &USART_InitStructure);       //初始化串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启串口接收中断
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  //使能串口3 DMA发送

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel2);
    //  DMA_InitStruct.DMA_BufferSize=10;                   //传输次数，启动发送时重置
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;     //传输方向，内存到外设
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //关闭内存到内存模式
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;          //正常模式，非循环
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //源数据宽度
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //源地址自增
    //  DMA_InitStruct.DMA_MemoryBaseAddr=(u32)dat;         //源地址，启动发送时重置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&USART3->DR); //外设地址
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址不变
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;  //通道优先级，中等
    DMA_Init(DMA1_Channel2, &DMA_InitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

    //  DMA_Cmd(DMA1_Channel2, ENABLE);                 //启动发送时使能
    USART_Cmd(USART3, ENABLE);                      //使能串口3

}
/*------------------------------------------------------------------------
功能：串口4初始化
入参：boundRate，波特率，直接写，例如115200
说明：开启DMA发送
------------------------------------------------------------------------*/
void Uart4_init(u32 boundRate)
{
    //GPIO端口设置
    DMA_InitTypeDef  DMA_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //GPIOC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);   //使能UART4时钟

    //UART4_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC10

    //UART4_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC11

    //Uart4 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //UART4 初始化设置

    USART_InitStructure.USART_BaudRate = boundRate;         //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(UART4, &USART_InitStructure);        //初始化串口4
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);   //开启串口接收中断
    USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);   //使能串口4 DMA发送

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    DMA_DeInit(DMA2_Channel5);
    //  DMA_InitStruct.DMA_BufferSize=10;                   //传输次数，启动发送时重置
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;     //传输方向，内存到外设
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //关闭内存到内存模式
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;          //正常模式，非循环
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //源数据宽度
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; //源地址自增
    //  DMA_InitStruct.DMA_MemoryBaseAddr=(u32)dat;         //源地址，启动发送时重置
    DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)(&UART4->DR);  //外设地址
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据宽度
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       //外设地址不变
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;  //通道优先级，中等
    DMA_Init(DMA2_Channel5, &DMA_InitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);

    //  DMA_Cmd(DMA2_Channel5, ENABLE);                 //启动发送时使能
    USART_Cmd(UART4, ENABLE);                       //使能串口4

}
/*------------------------------------------------------------------------
功能：串口5初始化
入参：boundRate，波特率，直接写，例如115200
说明：由于串口5不支持DMA，所以使用中断以连续输出数据块
------------------------------------------------------------------------*/
void Uart5_init(u32 boundRate)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); //GPIOC、D时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);   //使能UART5时钟

    //UART5_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC12

    //UART5_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD2

    //Uart5 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //UART5 初始化设置

    USART_InitStructure.USART_BaudRate = boundRate;         //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;     //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(UART5, &USART_InitStructure);        //初始化串口5
    USART_ClearITPendingBit(UART5, USART_IT_TC);
    USART_ClearFlag(UART5, USART_IT_TC);
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);   //开启串口接收缓冲区非空中断，
    //  USART_ITConfig(UART5, USART_IT_TXE, ENABLE);    //连续发送时打开，开启发送缓冲区空中断
    USART_Cmd(UART5, ENABLE);                       //使能串口5
}


