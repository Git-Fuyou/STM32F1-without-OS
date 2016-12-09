/*------------------------------------------------------------------------
文件：spi.h
	  
说明：片选引脚定义在此文件
------------------------------------------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

#include "sys.h"


#define W25QXX_CS           ((uint16_t)0x1000)
#define W25QXX_CS_H         GPIOB->ODR |=  W25QXX_CS
#define W25QXX_CS_L         GPIOB->ODR &= ~W25QXX_CS

#define SD_CS               ((uint16_t)0x0010)
#define SD_CS_H             GPIOA->ODR |=  SD_CS
#define SD_CS_L             GPIOA->ODR &= ~SD_CS

// SPI总线速度设置 
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7 	

void SPI1_Init(void);            //初始化SPI1口
void SPI2_Init(void);			 //初始化SPI2口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI2速度
u8   SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节   
u8   SPI2_ReadWriteByte(u8 TxData);//SPI2总线读写一个字节
		 
#endif

