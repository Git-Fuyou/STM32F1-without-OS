/*-----------------------------------------------------------------------------------------------
文件：DisplayMenu.h 

说明：菜单相关结构体及命令定义
/----------------------------------------------------------------------------------------------*/
#ifndef __DisplayMenu_H
#define __DisplayMenu_H	

#include "stm32f10x.h"

#define MSG_CLR     0       //清除消息
#define WD_CREATE 	1		//窗口创建消息
#define WD_PAINT  	2		//窗口绘制消息
#define WD_CLOSE  	3		//窗口注销消息
#define WD_TIMER  	4		//窗口定时器消息
#define WD_KEY1		5		//按键1消息
#define WD_KEY2		6		//按键2消息
#define WD_KEY3		7		//按键3消息
#define WD_KEY4		8		//按键4消息
#define WD_USER		0x80	//用户消息1

#define MENU_CREAT  1		//菜单创建
#define MENU_UP     2		//菜单上翻
#define MENU_DOWN   3		//菜单下翻

#define HORIZONTAL  0 		//菜单水平显示
#define VERTICAL    2		//菜单竖直显示
#define H_REVER		1		//菜单水平反显
#define V_REVER		3		//菜单竖直反显

#ifndef FONT12
#define FONT12      12      //字号12
#endif
#ifndef FONT16
#define FONT16      16      //字号16
#endif
#ifndef FONT24
#define FONT24      24      //字号24
#endif

typedef void (*FUNC_POINT)(void);//函数指针

typedef struct
{
	u8   const* text;		//显示的菜单名称，用于菜单选择时的文本，窗口标题使能时，可作为对应子窗口标题
	u8 	param;				//代表菜单项对应的入口索引
	//u8   const* icon;		//显示的菜单图标
	//u8   iconWidth;		//图标宽度
	//u8   iconHeight;		//图标高度
	
}MenuItem;

typedef struct//每级菜单一个，入栈保存
{
	u8 	hwndParent;			//父菜单索引
	u8 	sel;				//当前选中菜单项
	u8 	head;  				//当前显示菜单列表的首项
	
}MENU;

typedef struct//消息变量，以数组的形式做成消息队列，数组的[0]总是提供给最前端的窗口，并且使用完后清零
{
	u8 	msg;				//消息类型
	u8 	wParam;				//消息参数1
	u8 	iParam;				//消息参数2
	
}MSG;

typedef struct//窗体变量，仅一个，窗口切换时更新
{
	u8    	wndIndex;			//当前窗口索引
	u16   	timer;				//窗体定时器
	void  	  (* FuncPoint)(void);  //窗体函数指针
	
}WINDOW;

typedef struct
{
	u8 	indexState;			//当前菜单索引号
	MenuItem 	const* caption;		//菜单标题（显示的文字等）
	void 	(* CurOperate)(void);	//当前菜单处理函数指针	
	u8 	listMenuNum;		//包含子菜单个数
	u8  	listShowNum;		//列表最多显示子菜单个数
	u8  	direction ;			//菜单显示方向
	u16 	rowOrColumn;		//水平方向表示菜单所在行，竖直方向表示菜单所在列
	u16 	start;				//菜单列表起始点坐标
	u16 	end;				//菜单列表终点坐标
	
}TabStruct;

//内部函数
void  MsgQProcess(void);
u8 WindowCreat(void);
u8 WindowDestory(void);
void  MenuDraw(u8 cmd,u8 fontSize);
void  StringLength(u8 const * str,u8 fontSize,u16 * len);
void  ShowString(u16 x,u16 y,u16 interval,u8 const * str,u8 fontSize,u8 rever);
//外部接口
void  MenuInit(void);	//多级菜单初始化
void  PostMessage(u8 cmd,u8 dat1);//发送窗口消息
void  MenuProcess(void);//消息引擎，周期性调用以提供窗口时钟

#endif

