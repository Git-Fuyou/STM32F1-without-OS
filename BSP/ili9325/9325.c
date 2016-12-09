/*-----------------------------------------------------------------------
ili9325液晶驱动，320x240

文件：LCD9325.c  
      
说明：1.实现ili9325的驱动函数
	  2.需要w25qxx.h、fontupd.h、text.h文件支持，中文字库在w25qxx中
------------------------------------------------------------------------*/
#include "9325.h"
#include "font.h"
#include "fsmc.h"
#include "usart.h" 
#include "delay.h"
#include "w25qxx.h"

extern const unsigned char asc2_1206[95][12];//英文（半角）字库
extern const unsigned char asc2_1608[95][16];//GBK字库里的是全角，不方便
extern const unsigned char asc2_2412[95][36];//同时也可以防止字库损坏时无法显示					
						




/*------------------------------------LCD 画点-----------------------------------------------------	    
//入参：x  		横坐标
//		y		纵坐标
//		color	颜色
/-----------------------------------------------------------------------------------------------*/
void LcdDrawPoint(unsigned short x, unsigned short y, unsigned int color)
{
	unsigned int n;
	
	n=color>>8;//大小端转换
	color<<=8;
	color+=n;
	
	LcdSetPos(x,x,y,y);
	
	LCD->LCD_RAM = color;
}
/*------------------------------------LCD 读点-----------------------------------------------------	    
入参：	x  		横坐标
		y		纵坐标
返回：	color	颜色
说明：  读不出来
/-----------------------------------------------------------------------------------------------*/
u16 LcdReadPoint(u16 x, u16 y)
{
//	unsigned int n;
	vu16 color;
	vu8 tmp;
	
//	n=color>>8;//大小端转换
//	color<<=8;
//	color+=n;

//	Write_Cmd_Data(20,x);//x坐标
//	
//	Write_Cmd_Data(21,y);//y坐标
//	
//	LCD->LCD_REG = 0x2200;
//	delay_us(2);
	LcdSetPos(x,y,x,y);
	delay_us(2);
	color = LCD->LCD_RAM;//空读
	printf("%x\n ",color);
	delay_us(2);
	color = LCD->LCD_RAM;
	printf("%x\n ",color);
	return color;
}
/*------------------------------------LCD 写单个字符-----------------------------------------------	    
//入参：x  		横坐标
//		y		纵坐标
//		chr		字符
//  	size	字号 	FONT12、FONT16、FONT24
//		fColor	字符颜色
//		bColor	背景颜色
/-----------------------------------------------------------------------------------------------*/
void LcdPutChar(unsigned short x, unsigned short y, char chr,unsigned char size, unsigned int fColor, unsigned int bColor) 
{
	unsigned int i,j,n;
	unsigned char temp;
	u8 csize=(size/8+((size%8)?1:0));//高度占多少个字节
	
	LcdSetPos(x,x+size/2-1,y,y+csize*8-1);
	csize=csize * size /2;//得到字体一个字符对应点阵集所占的字节数
	chr -= ' ';	
	
	n=fColor>>8;//大小端转换
	fColor<<=8;
	fColor+=n;
	n=bColor>>8;//大小端转换
	bColor<<=8;
	bColor+=n;
	
	for(i=0;i<csize;i++)
    {   
		if(size==12)temp=asc2_1206[chr][i]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][i];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][i];	//调用2412字体
		else return;								//没有的字库
        for(j=0;j<8;j++)
		{
			if(temp&0x80)LCD->LCD_RAM = fColor;
			else LCD->LCD_RAM = bColor;
			temp<<=1;
		}  	 
    } 
}		

/*------------------------------------LCD 写单个汉字-----------------------------------------------	    
//入参：x  		横坐标
//		y		纵坐标
//		c[2]	汉字编码
//  	size	字号 	FONT12、FONT16、FONT24
//		fColor	字符颜色
//		bColor	背景颜色
/-----------------------------------------------------------------------------------------------*/	
void LcdPutFont(unsigned short x, unsigned short  y, unsigned char c[2],unsigned char size, unsigned int fColor,unsigned int bColor)
{
	unsigned int i,j,n;
	unsigned char temp;
	u8 dzk[72];
	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数
	
	LcdSetPos(x,  x+size-1,y, y+size-1);
	
	Get_HzMat(c,dzk,size);	//得到相应大小的点阵数据
	
	n=fColor>>8;//大小端转换
	fColor<<=8;
	fColor+=n;
	n=bColor>>8;//大小端转换
	bColor<<=8;
	bColor+=n;
	
	for(i=0;i<csize;i++)
	{   												   
		temp=dzk[i];		//得到点阵数据                          
		for(j=0;j<8;j++)
		{
			if(temp&0x80)LCD->LCD_RAM = fColor;
			else LCD->LCD_RAM = bColor;
;
			temp<<=1;
		}  	 
	} 
}

/*------------------------------------LCD 写字符串，中英文----------------------------------------    
//入参：x  		横坐标
//		y		纵坐标
//		s		字符串指针
//  	size	字号 	FONT12、FONT16、FONT24
//		fColor	字符颜色
//		bColor	背景颜色
/-----------------------------------------------------------------------------------------------*/
unsigned short LcdPutString(unsigned short x, unsigned short y, unsigned char *s,unsigned char size, unsigned int fColor, unsigned int bColor) 
{	
	unsigned short x0=x,bw=0;
	while(*s) 
	{
		if( *s < 0x80) 
		{
			if(x>(LCD_WIDTH-size/2))
			{
				y+=size;
				x=x0;
			}	
			if(y>(LCD_HIGHT-size))break;
			if(*s==13)//换行符号
			{
				y+=size;
				x=x0;
				s+=2;// 再跳过回车
				bw+=2;
			}
			else 
			{
				LcdPutChar(x,y,*s,size,fColor,bColor);		
				s++;x += size/2;
				bw++;
			}						
		}
		else
		{
			if(x>(LCD_WIDTH-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
			if(y>(LCD_HIGHT-size))break;//越界返回  
			LcdPutFont(x,y,(unsigned char*)s,size,fColor,bColor);
			s+=2;x += size;
			bw+=2;
		}
	}
	return bw;
}

/*-----------------------------------------------------------------------
                         显示RGB颜色
-----------------------------------------------------------------------*/
void ShowRGB (unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,unsigned int Color)
{
	unsigned int i,j,n;
	
	n=Color>>8;//大小端转换
	Color<<=8;
	Color+=n;	
	LcdSetPos(x0,x1,y0,y1);
	
	for (i=y0;i<=y1;i++)
	{
	   for (j=x0;j<=x1;j++)
	       LCD->LCD_RAM = Color;

	}

}

/*-----------------------------------------------------------------------
                            显示图片
-----------------------------------------------------------------------*/
void ShowPhoto(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1,const unsigned char *pic)
{
	unsigned int j;
	unsigned int i;
	//unsigned long s=0;

	LcdSetPos(x0,x1-1,y0,y1-1);//定义坐标和显示范围

	for (i=0;i<y1-y0;i++)
	{
    	for (j=0;j<x1-x0;j++)
		LCD->LCD_RAM =*(pic++ -1);
		LCD->LCD_RAM =*(pic++ -1);
	}

}

/*-----------------------------------------------------------------------
                            写指令和数据	  8位总线
-----------------------------------------------------------------------*/
void  Write_Cmd_Data (unsigned char x,unsigned int y)
{	
	unsigned int n;
	
	n=x;
	n<<=8;
	LCD->LCD_REG = n;
	n=(y>>8);//大小端转换
	y<<=8;
	y+=n;
	LCD->LCD_RAM = y;
}

/*-----------------------------------------------------------------------
                            写16位数据
-----------------------------------------------------------------------*/
void  Write_Data_U16(unsigned int y)
{	

	unsigned int n;
	n=y>>8;//大小端转换
	y<<=8;
	y+=n;		
	LCD->LCD_RAM = y;

}
					     
/*-----------------------------------------------------------------------
                             延时程序
-----------------------------------------------------------------------*/
void delayms(unsigned int count)
{
    int i,j;                                                                                
    for(i=0;i<count;i++)                                                                    
       {
	     for(j=0;j<26;j++);
       }                                                                                     
}

/*-----------------------------------------------------------------------
                          液晶初始化
-----------------------------------------------------------------------*/
void ILI9325Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);	 //使能PG端口时钟

	GPIO_InitStructure.GPIO_Pin = TFT_RST;				 	 //RST引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOG, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
//	//液晶接口初始化   
//	RCC->APB2ENR|=0X00000014;//先使能外设IO PORTA,C时钟

//	GPIOA->CRL=0X33333333; 	//PA0-7 推挽输出
//	GPIOC->CRL&=0Xff0000ff;
//	GPIOC->CRL|=0X00333300; //PC2-5 推挽输出

	TFT_RST_SET_L;
	delayms(10);
	TFT_RST_SET_H;

	Write_Cmd_Data(0x0001,0x0100); 
	Write_Cmd_Data(0x0002,0x0700); 
	Write_Cmd_Data(0x0003,0x1038);//0x1038是正的 
	Write_Cmd_Data(0x0004,0x0000); 
	Write_Cmd_Data(0x0008,0x0207);  
	Write_Cmd_Data(0x0009,0x0000);
	Write_Cmd_Data(0x000A,0x0000); 
	Write_Cmd_Data(0x000C,0x0000); 
	Write_Cmd_Data(0x000D,0x0000);
	Write_Cmd_Data(0x000F,0x0000);
	//power on sequence VGHVGL
	Write_Cmd_Data(0x0010,0x0000);   
	Write_Cmd_Data(0x0011,0x0007);  
	Write_Cmd_Data(0x0012,0x0000);  
	Write_Cmd_Data(0x0013,0x0000); 
	//vgh 
	Write_Cmd_Data(0x0010,0x1290);   
	Write_Cmd_Data(0x0011,0x0227);
	//delayms(100);
	//vregiout 
	Write_Cmd_Data(0x0012,0x001d); //0x001b
	//delayms(100); 
	//vom amplitude
	Write_Cmd_Data(0x0013,0x1500);
	//delayms(100); 
	//vom H
	Write_Cmd_Data(0x0029,0x0018); 
	Write_Cmd_Data(0x002B,0x000D); 

	//gamma
	Write_Cmd_Data(0x0030,0x0004);
	Write_Cmd_Data(0x0031,0x0307);
	Write_Cmd_Data(0x0032,0x0002);// 0006
	Write_Cmd_Data(0x0035,0x0206);
	Write_Cmd_Data(0x0036,0x0408);
	Write_Cmd_Data(0x0037,0x0507); 
	Write_Cmd_Data(0x0038,0x0204);//0200
	Write_Cmd_Data(0x0039,0x0707); 
	Write_Cmd_Data(0x003C,0x0405);// 0504
	Write_Cmd_Data(0x003D,0x0F02); 
	//ram
	Write_Cmd_Data(0x0050,0x0000); 
	Write_Cmd_Data(0x0051,0x00EF);
	Write_Cmd_Data(0x0052,0x0000); 
	Write_Cmd_Data(0x0053,0x013F);  
	Write_Cmd_Data(0x0060,0xA700); 
	Write_Cmd_Data(0x0061,0x0001); 
	Write_Cmd_Data(0x006A,0x0000); 
	//
	Write_Cmd_Data(0x0080,0x0000); 
	Write_Cmd_Data(0x0081,0x0000); 
	Write_Cmd_Data(0x0082,0x0000); 
	Write_Cmd_Data(0x0083,0x0000); 
	Write_Cmd_Data(0x0084,0x0000); 
	Write_Cmd_Data(0x0085,0x0000); 
	//
	Write_Cmd_Data(0x0090,0x0010); 
	Write_Cmd_Data(0x0092,0x0600); 
	Write_Cmd_Data(0x0093,0x0003); 
	Write_Cmd_Data(0x0095,0x0110); 
	Write_Cmd_Data(0x0097,0x0000); 
	Write_Cmd_Data(0x0098,0x0000);
	Write_Cmd_Data(0x0007,0x0133);
		
}

/*-----------------------------------------------------------------------
                         清屏函数
-----------------------------------------------------------------------*/
void ClearScreen(unsigned int bColor)
{
	unsigned int i,j,n;
	
	n=(bColor>>8);//大小端转换
	bColor<<=8;
	bColor+=n;
	
	LcdSetPos(0,240,0,320);//320x240
	for (i=0;i<322;i++)
	{	
	   for (j=0;j<240;j++)
	       LCD->LCD_RAM = bColor;
	}
}

/*-----------------------------------------------------------------------
                        设置坐标点
-----------------------------------------------------------------------*/
void LcdSetPos(unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1)
{
  Write_Cmd_Data(WINDOW_XADDR_START,x0);
  Write_Cmd_Data(WINDOW_XADDR_END,x1);
  Write_Cmd_Data(WINDOW_YADDR_START,y0);
  Write_Cmd_Data(WINDOW_YADDR_END,y1);
  Write_Cmd_Data(GRAM_XADDR,x0);
  Write_Cmd_Data(GRAM_YADDR,y0);
  LCD->LCD_REG = 0x2200;
}


