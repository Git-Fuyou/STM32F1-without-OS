/*------------------------------------------------------------------------
文件：font.c 
      
说明：1.实现字库信息的读取、更新等
	  2.需要FatFs文件系统支持，W25Qxx Flash支持，OLED支持，string.h支持
	    malloc.h内存管理，字库初始化函数用到延时，
------------------------------------------------------------------------*/
#include "ff.h"	
#include "oled.h"
#include "font.h"
#include "delay.h"  
#include "w25qxx.h" 
#include "string.h"
#include "malloc.h"


														
//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//字库出错时，将字库文件以对应名称存放在对应路径
u8* const UNIGBK_PATH="/FONT/UNIGBK.BIN";	//UNIGBK.BIN的存放位置
u8* const GBK12_PATH="/FONT/GBK12.FON";		//GBK12的存放位置
u8* const GBK16_PATH="/FONT/GBK16.FON";		//GBK16的存放位置
u8* const GBK24_PATH="/FONT/GBK24.FON";		//GBK24的存放位置


/*------------------------------------------------------------------------
功能：	从字库中查找出字模
入参：
		code 	GBK码
		mat  	点阵数据存放地址	
		size	字体大小
------------------------------------------------------------------------*/ 	
void Get_HzMat(u8 *code,u8 *mat,u8 size)
{		    
	u8 qh,ql;
	u8 i,csize;					  
	unsigned long foffset; 
//a:得到字体一个字符对应点阵集所占的字节数
	switch(size)
	{
		case FONT12:csize=24;break;
		case FONT16:csize=32;break;
		case FONT24:csize=72;break;
	}
//b:得到GBK码，若非常用汉字，填充空白点后退出
	qh=*code;
	ql=*(++code);	
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;
	    return; //结束访问
	}
//c:计算出内码并读出点阵数据	
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize; 
	
	switch(size)
	{
		case FONT12:
			
			W25QXX_Read(mat,foffset+ftinfo.f12addr,csize);
			break;
		case FONT16:
			
			W25QXX_Read(mat,foffset+ftinfo.f16addr,csize);
			break;
		case FONT24:
			
			W25QXX_Read(mat,foffset+ftinfo.f24addr,csize);
			break;			
	}     												    
} 
/*------------------------------------------------------------------------
功能：	OLED显示当前字体更新进度
入参：	x,y:坐标
		size:字体大小
		fsize:整个文件大小
		pos:当前文件指针位置
------------------------------------------------------------------------*/
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		OLED_ShowString(x+3*(size/2),y,"%",size,1);		
		t=prog;
		if(t>100)t=100;
		OLED_ShowNum(x,y,t,3,size);//显示数值
	}
	return 0;					    
}
/*------------------------------------------------------------------------
功能：	更新某一个字库
入参：	x,y:坐标
		size:字体大小
		fxpath:路径
		fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
返回:	0,成功;其他,失败.
------------------------------------------------------------------------*/
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
	u32 flashaddr=0;								    
	FIL * fftemp;
	u8 	*tempbuf;
 	u8 	res;	
	u16 bread;
	u32 offx=0;
	u8 	rval=0;	
	
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMIN,4096);				//分配4096个字节空间
	if(tempbuf==NULL)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//打开文件失败  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:												//更新UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//信息头之后，紧跟UNIGBK转换码表
				ftinfo.ugbksize=fftemp->fsize;					//UNIGBK大小
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK之后，紧跟GBK12字库
				ftinfo.gbk12size=fftemp->fsize;					//GBK12字库大小
				flashaddr=ftinfo.f12addr;						//GBK12的起始地址
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12之后，紧跟GBK16字库
				ftinfo.gbk16size=fftemp->fsize;					//GBK16字库大小
				flashaddr=ftinfo.f16addr;						//GBK16的起始地址
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16之后，紧跟GBK24字库
				ftinfo.gkb24size=fftemp->fsize;					//GBK24字库大小
				flashaddr=ftinfo.f24addr;						//GBK24的起始地址
				break;
		} 
			
		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//读取数据	 
			if(res!=FR_OK)break;								//执行错误
			W25QXX_Write(tempbuf,offx+flashaddr,bread);			//从0开始写入4096个数据  
	  		offx+=bread;	  
			fupd_prog(x,y,size,fftemp->fsize,offx);	 			//进度显示
			OLED_Refresh_Gram();
			if(bread!=4096)break;								//读完了.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//释放内存
	myfree(SRAMIN,tempbuf);	//释放内存
	return res;
} 
/*------------------------------------------------------------------------
功能：	更新字体文件,UNIGBK,GBK12,GBK16,GBK24一起更新
入参：	x,y:提示信息的显示地址
		size:字体大小
		src:字库来源磁盘."0:",SD卡;"1:",FLASH盘,"2:",U盘.										  
返回：	0,更新成功;其他,错误代码.
说明：	OLED显示更新进度
------------------------------------------------------------------------*/
u8 update_font(u16 x,u16 y,u8 size,u8* src)
{	
	u8 *pname;
	u32 *buf;
	u8 res=0;		   
 	u16 i,j;
	FIL *fftemp;
	u8 rval=0; 
	res=0XFF;		
	ftinfo.fontok=0XFF;
	pname=mymalloc(SRAMIN,100);	//申请100字节内存  
	buf=mymalloc(SRAMIN,4096);	//申请4K字节内存  
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	
	if(buf==NULL||pname==NULL||fftemp==NULL)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return 5;	//内存申请失败
	}
	//先查找文件是否正常 
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)UNIGBK_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<4;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK12_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<5;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK16_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<6;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK24_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<7;//打开文件失败   
	myfree(SRAMIN,fftemp);//释放内存
	if(rval==0)//字库文件都存在.
	{  
		OLED_ShowString(x,y,"Erasing...",size,1);//提示正在擦除扇区	
		for(i=0;i<FONTSECSIZE;i++)	//先擦除字库区域,提高写入速度
		{
			fupd_prog(x+10*size/2,y,size,FONTSECSIZE,i);//进度显示
			OLED_Refresh_Gram();
			W25QXX_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//读出整个扇区的内容
			for(j=0;j<1024;j++)//校验数据
			{
				if(buf[j]!=0XFFFFFFFF)break;//需要擦除  	  
			}
			if(j!=1024)W25QXX_Erase_Sector((FONTINFOADDR/4096)+i);	//需要擦除的扇区
		}
		myfree(SRAMIN,buf);
		OLED_ShowString(x,y,"UNIGBK.BIN",size,1);
		OLED_Refresh_Gram();		
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res=updata_fontx(x+10*size/2,y,size,pname,0);	//更新UNIGBK.BIN
		if(res){myfree(SRAMIN,pname);return 1;}
		OLED_ShowString(x,y,"GBK12.BIN ",size,1);
		OLED_Refresh_Gram();
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK12_PATH); 
		res=updata_fontx(x+10*size/2,y,size,pname,1);	//更新GBK12.FON
		if(res){myfree(SRAMIN,pname);return 2;}
		OLED_ShowString(x,y,"GBK16.BIN ",size,1);
		OLED_Refresh_Gram();
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK16_PATH); 
		res=updata_fontx(x+10*size/2,y,size,pname,2);	//更新GBK16.FON
		if(res){myfree(SRAMIN,pname);return 3;}
		OLED_ShowString(x,y,"GBK24.BIN ",size,1);
		OLED_Refresh_Gram();
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK24_PATH); 
		res=updata_fontx(x+10*size/2,y,size,pname,3);	//更新GBK24.FON
		if(res){myfree(SRAMIN,pname);return 4;}
		//全部更新好了
		ftinfo.fontok=0XAA;
		W25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//保存字库信息
		OLED_ShowString(x,y,"Finish       ",size,1);
		OLED_Refresh_Gram();
	}
	myfree(SRAMIN,pname);//释放内存 
	myfree(SRAMIN,buf);
	return rval;//无错误.			 
} 
/*------------------------------------------------------------------------
功能：	初始化字体

返回:	0,字库完好.其他,字库丢失		 
------------------------------------------------------------------------*/
u8 font_init(void)
{		
	u8 t=0;
//a 先尝试读取一次
	W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//读出ftinfo结构体数据
	if(ftinfo.fontok==0XAA)return 0;
	
//b	不正确, 初始化后重新读取
	else
	{
		W25QXX_Init();
		while(t<10)//连续读取10次,都是错误,说明确实是有问题,得更新字库了
		{
			t++;
			W25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//读出ftinfo结构体数据
			if(ftinfo.fontok==0XAA)break;
			delay_ms(20);
		}
		if(ftinfo.fontok!=0XAA)return 1;
	}
	return 0;		    
}


