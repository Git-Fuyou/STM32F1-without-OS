/*------------------------------------------------------------------------
文件：dma.c  
      
说明：1.实现DMA相关
	  2.需要stm32f10x_dma.h、misc.h支持，在stm32f10x_conf.h中打开
------------------------------------------------------------------------*/
#include "DMA.h"

/*------------------------------------------------------------------------
DMA中断入口
------------------------------------------------------------------------*/
//void DMA1_Channel1_IRQHandler(void)
//{
//	dma_flg=1;
//	DMA_ClearFlag(DMA1_FLAG_TC1);
//}

uint16_t DMA1_DAT_LEN;//传输数据长度，内部全局变量
/*------------------------------------------------------------------------
功能：ADC相关DMA初始化
入参：DMAy_Channelx 通道号
      srcdir        源地址
	  aimdir        目标地址
	  datanum       传输数目
------------------------------------------------------------------------*/
void dma_init(DMA_Channel_TypeDef* DMAy_Channelx,uint32_t srcdir,uint32_t aimdir,uint16_t datanum)
{
	DMA_InitTypeDef  DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA1_DAT_LEN=datanum;
	DMA_DeInit(DMAy_Channelx);
	
	DMA_InitStruct.DMA_BufferSize=datanum;
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr=aimdir;
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr=srcdir;
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_Priority=DMA_Priority_Medium;
	
	DMA_Init(DMAy_Channelx, &DMA_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=DMA1_Channel1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x02;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x02;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	DMA_ITConfig(DMAy_Channelx,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(DMAy_Channelx, ENABLE);
}
/*------------------------------------------------------------------------
功能：DMA传输使能
入参：DMAy_Channelx  通道号
说明：在不使用循环传输时需要调用此函数重新设置传输数据量
------------------------------------------------------------------------*/
void dma_enable(DMA_Channel_TypeDef* DMAy_Channelx)
{
	DMA_Cmd(DMAy_Channelx, DISABLE);
	DMA_SetCurrDataCounter(DMAy_Channelx,DMA1_DAT_LEN); 
	DMA_Cmd(DMAy_Channelx, ENABLE);
}
