/*!-----------------------------------------------------------------------
* @file    flash.h
* @author  FuYou
* @version V1.0
* @date    2016-09-21
* @brief   根据配置自动保存数据块到flash
* @note    用法：

    比如现在有2个字节和一个半字的数据需要掉电保存：(u8)a,(u8)b,(u16)c
	用的单片机为STM32F103CB，共128KB的Flash，128个扇区，每个1024字节，存在最后一个扇区
*   - 
    比较节省的办法是：USER_DATA_SIZE    2
	                  STM_SECTOR_SIZE   1024
					  DATA_SECTOR_NUMB  127
                      sysData[0]=(u16)ab;      //ab组合成(u16)
                      sysData[1]=(u16)c; 
                      SaveDataToFlash(sysData);
*   - 
    比较简便的方法是：USER_DATA_SIZE    3
					  STM_SECTOR_SIZE   1024
					  DATA_SECTOR_NUMB  127
                      sysData[0]=(u16)a;
                      sysData[1]=(u16)b;
                      sysData[0]=(u16)c;
                      SaveDataToFlash(sysData);
*   - 
    读取时根据自己定义的写的格式拆分sysData[]
------------------------------------------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H	

#include "stm32f10x.h"

/** @defgroup Flash
  * @{
  */
  
#define USER_DATA_SIZE  	3    			//保存数据需要的空间长度（半字，即16位）
#define STM_SECTOR_SIZE 	1024 			//单片机扇区的大小, >512KB的大容量设备为2048（大于等于最小的擦除单元即可）
#define DATA_SECTOR_NUMB    127  			//数据在哪一个扇区保存（从0开始）

#define USER_DATA_CONT 		STM_SECTOR_SIZE/((USER_DATA_SIZE+1)*2)//一个扇区能保存多少次数据，自动计算
#define USER_SECTOR_BASE 	(FLASH_BASE+(STM_SECTOR_SIZE*DATA_SECTOR_NUMB))//扇区基地址，自动计算

/**
  * @}
  */

extern u16 sysData[USER_DATA_SIZE];			//数据缓冲区，16位的，保存数据时注意类型


void SaveDataToFlash(u16 *pBuffer);
void ReadDataFromFlash(u16 *pBuffer);

#endif

