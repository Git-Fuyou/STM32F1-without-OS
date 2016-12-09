/*------------------------------------------------------------------------
文件：font.h  
      
说明：定义字库信息结构体，字号
------------------------------------------------------------------------*/
#ifndef __FONTUPD_H__
#define __FONTUPD_H__

#include <stm32f10x.h>


#define FONT12      12      //字号12
#define FONT16      16      //字号16
#define FONT24      24      //字号24


//字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息=3238700字节,约占791个W25QXX扇区)
#define FONTSECSIZE	 	791
//字库存放起始地址 
#define FONTINFOADDR 	1024*1024*4 //W25Qxx,前4M字节给FATFS用，之后用于存放字库,字库占用3.09M


//字库信息结构体定义，用来保存字库基本信息，地址，大小等
//占33个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
__packed typedef struct 
{
	u8 fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
	u32 ugbkaddr; 			//unigbk的地址
	u32 ugbksize;			//unigbk的大小	 
	u32 f12addr;			//gbk12地址	
	u32 gbk12size;			//gbk12的大小	 
	u32 f16addr;			//gbk16地址
	u32 gbk16size;			//gbk16的大小		 
	u32 f24addr;			//gbk24地址
	u32 gkb24size;			//gbk24的大小 
}_font_info; 


extern _font_info ftinfo;	//字库信息结构体


u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos);	//显示更新进度
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx);	//更新指定字库
u8 update_font(u16 x,u16 y,u8 size,u8* src);			//更新全部字库
u8 font_init(void);										//初始化字库
void Get_HzMat(u8 *code,u8 *mat,u8 size);				//得到汉字的点阵码

#endif

