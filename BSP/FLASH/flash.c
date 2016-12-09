/*!-----------------------------------------------------------------------
* @file    flash.c
* @author  FuYou
* @version V1.0
* @date    2016-09-21
* @brief   实现小块数据的循环存储，提高flash寿命

------------------------------------------------------------------------*/

#include "OLED.h"
#include "usart.h"
#include "flash.h"


u16 sysData[USER_DATA_SIZE]; ///< 数据缓冲区，读写flash都通过它

/**
 * @brief   读取指定地址的半字(16位数据)
 * @param   faddr:读地址(此地址必须为2的倍数!!)
 * @retval  对应地址的数据
 */
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}

/**
 * @brief   不检查的写入
 * @param   WriteAddr: 起始地址
 * @param   pBuffer:   数据指针
 * @param   NumToWrite:半字(16位)数
 * @retval  无
 */
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
    u16 i;
    for(i=0; i<NumToWrite; i++)
    {
		//写入2个字节.
        FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
		//地址增加2.
        WriteAddr+=2;
    }
}

/**
 * @brief   从指定地址开始读出指定长度的数据
 * @param   WriteAddr: 起始地址
 * @param   pBuffer:   数据指针
 * @param   NumToWrite:半字(16位)数
 * @retval  无
 */
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
    u16 i;
    for(i=0; i<NumToRead; i++)
    {
		//读取2个字节.
        pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);
		//偏移2个字节.
        ReadAddr+=2;
    }
}

/**
 * @brief   保存数据到flash
 * @param   pBuffer:数据缓存
 * @retval  无
 */
void SaveDataToFlash(u16 *pBuffer)
{
    u8  succeed=0;
    u16 i,j,tmp;
    u32 location;

    location=USER_SECTOR_BASE;
//  printf("Write\n");
//  printf("i=%d\n",i);

    //查找一块为被写过的块
    for(i=0; i<USER_DATA_CONT; i++) 
    {
        tmp=STMFLASH_ReadHalfWord(location);

        if(tmp==0x5aa5)//被使用过的
        {
            location += (USER_DATA_SIZE+1)*2;//检查下一块数据空间
        }
        else          //未使用过，或数据头不对的
        {
            if(tmp==0xffff)//未使用
            {
                for(j=0; j<USER_DATA_SIZE; j++)
                    if(STMFLASH_ReadHalfWord(location+2+j*2)!=0xffff)break;
                if(j==USER_DATA_SIZE)
                {
                    FLASH_Unlock(); //解锁
                    FLASH_ProgramHalfWord(location,0x5aa5);//写标志
//                  printf("succeed location=%x    ",location);
                    STMFLASH_Write_NoCheck(location+2,pBuffer,USER_DATA_SIZE);//写入数据
                    succeed=1;
                    FLASH_Lock();   //上锁
//                  DisplayString(1,32,0,"保存成功");
//                  printf("i=%d\n",i);
                    break;

                }
            }
            else//头不对，继续寻找下一个
            {
                location += (USER_DATA_SIZE+1)*2;//检查下一块数据空间
//              printf("flg is 0X%x\n",STMFLASH_ReadHalfWord(location));
            }
        }

    }
	
	//写满了
    if(i==USER_DATA_CONT)
    {
        if(!succeed)
        {
            location=USER_SECTOR_BASE;//在扇区开始处写入数据
            FLASH_Unlock(); //解锁
//          printf("ErasePage  ");
            FLASH_ErasePage(USER_SECTOR_BASE);//擦除扇区
            FLASH_ProgramHalfWord(USER_SECTOR_BASE,0x5aa5);//写标志
//          printf("location=%x    ",location);
            STMFLASH_Write_NoCheck(USER_SECTOR_BASE+2,pBuffer,USER_DATA_SIZE);//写入数据
            FLASH_Lock();   //上锁
//          DisplayString(1,32,0,"保存成功");
//          printf("i=%d\n",i);
        }
    }
}

/**
 * @brief   从flash读取数据
 * @param   pBuffer:数据缓存
 * @retval  无
 */
void ReadDataFromFlash(u16 *pBuffer)
{
    u8  found=0;
    u16 i,tmp;
    u32 location,read;

    location=USER_SECTOR_BASE;
//  printf("Read\n");
    for(i=0; i<USER_DATA_CONT; i++)
    {
        tmp=STMFLASH_ReadHalfWord(location);

        if(tmp==0x5aa5)
        {
            found = 1;//找到未使用，它的前一个就是最新的数据
            read=location;//标记地址
        }
        else
            break;
        location += (USER_DATA_SIZE+1)*2;

    }
	
    if(found)//找到
    {
		//读出
        STMFLASH_Read(read+2,pBuffer,USER_DATA_SIZE);
//      DisplayString(1,16,0,"found");
//      printf("read =%x  ",read);
//      printf("i =%d\n",i);
    }
    else//没找到
    {
//      DisplayString(1,16,0,"NOT found");
    }
}





