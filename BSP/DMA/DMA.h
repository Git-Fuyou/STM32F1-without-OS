/*------------------------------------------------------------------------
文件：dma.h  
      
说明：
------------------------------------------------------------------------*/
#ifndef __DMA_h
#define __DMA_h

#include "stm32f10x.h"
 
void dma_init(DMA_Channel_TypeDef* DMAy_Channelx,uint32_t srcdir,uint32_t aimdir,uint16_t datanum);

void dma_enable(DMA_Channel_TypeDef* DMAy_Channelx);

#endif
