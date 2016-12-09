/*-----------------------------------------------------------------------------------------------
文件：menuio.c  
      
说明：1.实现显示屏驱动不能提供的函数，如画特定图片 等
	  2.
/----------------------------------------------------------------------------------------------*/

#include "oled.h"
#include "MenuIO.h"
#include "DisplayMenu.h"

/*-----------------------------------------------------------------------------------------------
功能：	清除一行
入参：	x		 横坐标
		y		 纵坐标
    	fontSize 高度

说明：	本函数用于窗口绘制函数 Menu_Draw()的a处，用于清除一行
----------------------------------------------------------------------------------------------*/
void ClearLine(u8 x,u8 y,u8 fontSize)
{
	OLED_ShowString(x,y,"                ",fontSize,OLED_NORMAL);
}
/*-----------------------------------------------------------------------------------------------
功能：	清除菜单项所占的全部列
入参：	x		 横坐标
		y		 纵坐标
    	fontSize 高度

说明：	本函数用于窗口绘制函数 Menu_Draw()的b处，用于清除一行
----------------------------------------------------------------------------------------------*/
void ClearRow(void)
{
	OLED_Clear();
}
/*----------------------------------------------------------------------------------------------
功能：	显示字符串，中英文
入参：	x		 横坐标
		y		 纵坐标
		interval 显示宽度（像素数）
    	str 	 显示的字符串指针
		fontSize 字体大小
		rever 	 反显及方向

说明：	本函数使用字库完成字符串显示，以支持内部MenuDraw()函数
----------------------------------------------------------------------------------------------*/
void ShowString(u16 x,u16 y,u16 interval,u8 const * str,u8 fontSize,u8 rever)
{
	u16 strlenth=0;
	
//a:处理坐标方向等，一般无需改动		
	if( rever & 0x2 )
	{
		//竖直的列表，外部处理好了，不需要处理
	}
	else
	{
		//水平的列表，计算开始显示的x坐标
		StringLength(str,fontSize,&strlenth);
		//字符串小于显示宽度则居中，否则起始位置不变
		if(strlenth < interval) x += (interval-strlenth)/2;
	}
	
	if(rever&0x1)rever=0;
	else rever=1;
	
//b:将下面的显示中英文字符串实现	
	OLED_ShowString(x,y,(u8 *)str,fontSize,rever);
}
/*显示一个中文汉字函数-----------------------------------------------------------------------------
//功能：显示一个中文汉字
//入参：x		 横坐标
//		y		 纵坐标
//    	dp 		 显示的字符串指针
//      fontSize 字体大小
//		rever 	 是否反显
//
//说明：以查表方式支持下方ShowString()函数
//----------------------------------------------------------------------------------------------*/
/*void ShowFont(u16 x,u16 y,u8 const *str,u8 fontSize,u8 rever)
{      			    
	u8 temp,t,t1,t2;
	u8 fh,fl;
	
	fh = *str;
    fl = *(++str);
	t1=sizeof(Font)/sizeof(Chinese);
	for(t=0;t<t1;t++)
	{
		str=Font[t].text;
		if(fh == *str)
		{
			str = (u8 const*)str +1;
			if(fl == *str) 
			{
				str=Font[t].data;	
				break;
			}	
		}
		
	}
	
	if(t==t1)
	{
		str=Font[6].data;//没找到显示 “ 品 ”
	}
	
	for(t=1;t<2;t--)
	{
		for(t1=0;t1<16;t1++)
		{   
			temp=*str;
			for(t2=7;t2<8;t2--)
			{
				if(temp&0x80)OLED_DrawPoint((x+t1),(y+t*8+t2),rever);
				else OLED_DrawPoint((x+t1),(y+t*8+t2),!rever);
				temp<<=1;
			} 
			str++;
		}  
	}
}*/	
/*显示字符串，中英文函数---------------------------------------------------------------------------
//功能：显示字符串，中英文
//入参：x		 横坐标
//		y		 纵坐标
//		interval 显示宽度（像素数）
//    	str 	 显示的字符串指针
//      fontSize 字体大小
//		rever 	 反显及方向
//
//说明：本函数以查表方式寻找点阵数据，以支持内部MenuDraw()函数
//----------------------------------------------------------------------------------------------*/
/*void ShowString(u16 x,u16 y,u16 interval,u8 const * str,u8 fontSize,u8 rever)
{							  	  
    u8 bHz=0;     	//字符或者中文  
	u16 strlenth=0;
   		
	if( rever & 0x2 )
	{
		//竖直的列表，外部处理好了，不需要处理
	}
	else
	{
		//水平的列表，计算开始显示的x坐标
		StringLength(str,fontSize,&strlenth);
		//字符串小于显示宽度则居中，否则起始位置不变
		if(strlenth < interval) x += (interval-strlenth)/2;
	}
	
	if(rever&0x1)rever=0;
	else rever=1;
	
    while( *str != 0)	//数据未结束
    { 
        if(!bHz)
        {
	        if(*str> 0x80)bHz=1;//中文 
	        else              //字符
	        {    
                OLED_ShowChar(x,y,*str,fontSize,rever);
				str++; 
				x+=8; //字符,为全字的一半 
	        }
        }else//中文 
        {     
            bHz=0;//有汉字库
	        ShowFont(x,y,str,fontSize,rever); //显示汉字
	        str += 2; 
	        x += fontSize;//下一个汉字偏移	    
        }					 
    } 
}*/

/*----------------------------------------------------------------------------------------------
功能：	在指定位置显示32*32图片
入参：	x		 横坐标
		y		 纵坐标
    	p 		 显示的字符串指针

说明：	本函数用于处理函数的 case WD_PAINT，即窗口初始化。根据需要自行添加
----------------------------------------------------------------------------------------------*/
void OLED_ShowPic(u8 x,u8 y,u8 const * p)
{      			    
	u8 temp,t,t1,t2;
	
	for(t=3;t<4;t--)
	{
		for(t1=0;t1<32;t1++)
		{   
			temp=*p;
			for(t2=7;t2<8;t2--)
			{
				if(temp&0x80)OLED_DrawPoint((x+t1),(y+t*8+t2),1);
				else OLED_DrawPoint((x+t1),(y+t*8+t2),0);
				temp<<=1;
			} 
			p++;
		}  
	}
	OLED_Refresh_Gram();//更新显示	
}

