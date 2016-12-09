/*-----------------------------------------------------------------------------------------------
文件：DisplayMenu.c           

说明：点阵液晶多级菜单（TFT，LCD等）                 刘福有，2016.06
-----------------------------------------------------------------------------------------------/
移植步骤：
	1. 配置菜单标题：移植时,首先用MenuItem这个结构体为每一个菜单定义个数组，编写标题“MenuItem.text”，
	   入口索引先写0或空着
	2. 编写处理函数：编写所有菜单界面的处理函数，先写个空函数，并在本文件开头（菜单列表之前）声明。
	3. 配置菜单列表：TabStruct const MenuTab[]。
		3.1 第一项：索引，是每个菜单或界面的索引号，保证唯一。0固定为启动界面，1为主菜单，按顺序为每一个定
		  义索引号。需要注意的是，索引号和数组对应的编号一致，这是为了逻辑清晰，及节省空间（中间无空项）。
		3.2 第二项：标题，对应写上第一步编写的MenuItem的数组名。
		3.3 第三项：处理函数，填写对应的处理函数名。
		3.4 第四项：子菜单数，菜单为对应MenuItem结构体数组的元素数，界面统一为0。系统将在窗口切换时为此项
				  为0 的窗口自动启动窗口定时器，若在对应事件内有重设定时器命令，则定时刷新。
		3.5 第五项：显示菜单数，屏幕能同时显示的菜单项数目。对于界面，写0则不会出现标题，写1则把菜单入口的
				  标题显示在对应的位置（相当于只有一个菜单项的菜单，总是被反显），可作为窗口标题用。
		3.6 第六项：菜单方向，菜单是水平排列还是竖直排列。
		3.7 第七项：行列，当菜单水平排列时，表示菜单顶部对其所在行，当竖直时，表示菜单左对齐所在列。
		3.8 第八项：起点，菜单在所在行或列的起点。
		3.9 第九项：终点，菜单在所在行或列的终点，菜单会通过这两个点，计算分布位置。起点小于等于终点将不显
			示菜单。
	4. 根据菜单间的关系，填好对应MenuItem数组元素的入口参数。没有子窗口的统一写NULL_MENU，系统发现此项
	   为0将不再创建新的子窗口。设置下方的MENU_DEPTH，屏幕参数等宏定义。 
	5. 编写对应的屏幕驱动函数，按键驱动函数。
	5. 根据下方提供的窗口处理函数模板，填充对应的处理函数。
	6. 将菜单出现的汉字取模，放到icon.h的 Chinese const Font[]。
-----------------------------------------------------------------------------------------------/
注意：
	1. 0号索引固定为启动界面，1号为主菜单。MenuInit()函数从0号索引启动，并触发定时器，定时结束自动切换到
	   主菜单。若不需要启动界面，将MenuInit()函数中的定时器赋值为2，即可马上切换到主菜单。
	2. icon.h只能由此文件唯一调用，其它文件调用将出错。
-----------------------------------------------------------------------------------------------/
待改进：
	1. 按键解释权的分配。现在是把按键解释权给了菜单引擎，但有时候界面需要有完全的按键解释权，与菜单不同。
	2. 菜单项的显示位置现在比较粗糙，需要有自适应能力。
	3. 窗口区域没有实现，菜单项的显示会超出范围。
	4. 图标
----------------------------------------------------------------------------------------------*/

#include "key.h"
#include "icon.h"
#include "oled.h"
#include "MenuIO.h"
#include "DisplayMenu.h"


//配置参数，用户设置------------------------------------------------------------------------------/
#define MENU_DEPTH      6	//用于设置栈深,随菜单级数而定
#define MSGQ_DEPTH		5	//消息队列深度，同时出现消息的源可能有，窗口定时器，按键，菜单系统，用户函数，中断
#define SCREEN_WIDTH    128 //屏宽x:0~127
#define SCREEN_HEIGHT   64  //屏高y:0~63

u16  displayTest=0;		//用于测试自动刷新功能

//此处用户无需更改--------------------------------------------------------------------------------/
#define MAIN_MENU       1   //主菜单的索引号
#define NULL_MENU       0   //系统提供的终点界面，防止出错（系统发现为0时不切换窗口）
#define SetTimer(time)  window.timer=time  //设置窗口定时器函数
WINDOW 	window;				//窗口结构体变量
MSG    	message[MSGQ_DEPTH];//消息队列
MENU   	curMenu;        	//菜单信息结构体变量
MENU   	menu[MENU_DEPTH];	//菜单信息栈，用于保存父窗口菜单信息
u8 	menuSP=0;			//菜单信息栈指针

//菜单标题，用户设置------------------------------------------------------------------------------/
MenuItem const StartPic[]={"Start",MAIN_MENU};
MenuItem const NullMenu[]={"without menu",NULL_MENU};
MenuItem const Main[]={

	{"定速",	8 },
	{"定量",	7 },
	{"设置",	2 }
};

MenuItem const SetMain[]={

	{"药品",	3 },
	{"报警",	4 },
	{"屏幕",	5 },
	{"其它",	6 }
};

MenuItem const SetScreen[]={

	{"亮度",	10 	},
	{"时间",	10 	},
	{"对比",	10 	}
};

MenuItem const SetAlarm[]={

	{"声音" 	,10 },
	{"震动"		,10 },
	{"闪光"		,10 }
};

MenuItem const SetOther[]={

	{"充电" 	,11 },
	{"电量"		,11 },
	{"存储"		,11 }
};

MenuItem const DrugMenu[]={

	{"1.天麻"		,9 },
	{"2.葡萄糖"		,9 },
	{"3.枸杞"		,9 },
	{"4.红枣"		,9 },
	{"5.生理盐水"	,9 },
	{"6.人参"		,9 },
	{"7.青霉素"		,9 },
	{"8.苹果汁"		,9 }
};
//处理函数声明，用户设置--------------------------------------------------------------------------/
void StartMenu(void);
void MainMenu(void);
void SetMainMenu(void);
void DrugSet(void);
void AlarmSet(void);
void ScreenSet(void);
void OtherSet(void);
void FixQuaility(void);
void FixSpeed(void);
void SaveData(void);
void SlidersSet(void);
void ErrorMsg(void);
//菜单列表，用户设置------------------------------------------------------------------------------/
TabStruct const MenuTab[]={
//								    显示菜单数
//  索引  标题          处理函数  子菜单数     菜单方向 行列 起点 终点
	{0, StartPic,	(* StartMenu),	0,	1,	HORIZONTAL,	0,	0,	127},
	{1, Main,		(* MainMenu),	3,	3,  HORIZONTAL,	47,	0,	127}, 	//主       菜单  定量  定速  设置
	
	{2,	SetMain,	(* SetMainMenu),4,	3,	HORIZONTAL,	47,	0,	127},	//设置主   菜单  药品  报警  屏幕  其它
	
	{3,	DrugMenu,	(* DrugSet)	,	8,	4,	VERTICAL , 	10,	0,	63},	//药品设置 菜单  无
	{4,	SetAlarm,	(* AlarmSet),	3,	3,	HORIZONTAL,	47,	0,	127},	//报警设置 菜单  声音  震动  闪光
	{5,	SetScreen,	(* ScreenSet),	3,	3,	HORIZONTAL,	47,	0,	127},	//屏幕设置 菜单  亮度  时间  对比
	{6,	SetOther,	(* OtherSet),	3,	3,	HORIZONTAL,	47,	0,	127},	//其它设置 菜单  无
		
	{7,	NullMenu,	(* FixQuaility),0,	0,	HORIZONTAL,	0,	0,	100},	//定量模式 界面  
	{8,	NullMenu,	(* FixSpeed),	0,	1,	HORIZONTAL,	0,	0,	20}, 	//定速模式 界面  
	{9,	NullMenu,	(* SaveData),	0,	1,	HORIZONTAL,	0,	0,	80},	//提示保存 界面
	{10,NullMenu,	(* SlidersSet),	0,	1,	HORIZONTAL,	0,	0,	80},	//滑条设置 界面
	{11,NullMenu,	(* ErrorMsg),	0,	1,	HORIZONTAL,	0,	0,	80}  	//提示错误 界面
	
};
//所有菜单配置到此结束----------------------------------------------------------------------------/

/*窗口函数模板----------------------------------------------------------------------------------*/
/*
void FunctionName(void)
{
	switch(message[0].msg)
	{			
		case WD_PAINT://窗口初始化
			
			//添加代码，绘制界面，背景等，在初始化窗口时调用唯一一次
		
			break;		
		
		case WD_TIMER://定时消息，刷新数据，需要实时显示的东西写在这，并设置好自动刷新时间
			
			//添加代码，需要刷新的数据等
		
			//SetTimer(50);						//若重新设置定时器，可周期性定时刷新，界面添加，菜单一般不添加			
			break;				
	}
}*/

/*启动界面函数-------------------------------------------------------------------------------------
//
//MenuInit函数将启动窗设为顶层窗口，将指针指向本函数，设置窗口定时器，并发送绘制窗口消息，创建本窗口
//
//当定时器终止时，将触发定时消息命令。定时消息发送窗口创建命令，通过本窗口自动创建主菜单窗口
//
//用户需要做的是补充绘制本窗口命令，绘制用户自己的启动界面
//----------------------------------------------------------------------------------------------*/
void StartMenu(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制本窗口
		
			//绘制图片等，用户添加

			break;
		
		case WD_TIMER://定时消息，启动界面结束，跳转到主菜单
			
			PostMessage(WD_CREATE,0);			//发送窗口创建消息
			break;			
	}
}

/*主菜单处理函数---------------------------------------------------------------------------------*/
void MainMenu(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowPic(0,16,bottle_icon);		//绘制界面，背景等（图片）
			OLED_ShowPic(50,16,drop_icon);
			OLED_ShowPic(98,16,set_icon);
			
			break;						
	}
}

/*设置主菜单处理函数-----------------------------------------------------------------------------*/
void SetMainMenu(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowPic(0,16,durgs_icon);		//绘制界面，背景等（图片）
			OLED_ShowPic(50,16,alarm_icon);
			OLED_ShowPic(98,16,others_icon);
			
			break;					
	}
}

/*药品设置处理函数-------------------------------------------------------------------------------*/
void DrugSet(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			//绘制界面，背景等（图片）
		
			break;						
	}
}

/*报警设置处理函数-------------------------------------------------------------------------------*/
void AlarmSet(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowPic(0,16,speaker_icon);	//绘制界面，背景等（图片）
			OLED_ShowPic(50,16,shock_icon);
			OLED_ShowPic(98,16,lamp_icon);
			
			break;							
	}
}

/*屏幕设置处理函数-------------------------------------------------------------------------------*/
void ScreenSet(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowString(30,20,"ScreenSet",FONT16,OLED_NORMAL);
			
			break;						
	}
}

/*其它设置处理函数-------------------------------------------------------------------------------*/
void OtherSet(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowString(30,20,"OtherSet",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;				
	}
}

/*定量报警处理函数-------------------------------------------------------------------------------*/
void FixQuaility(void)
{
	switch(message[0].msg)
	{			
		case WD_PAINT://绘制窗口
			
			OLED_ShowString(30,16,"FixQuaility",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;
		
		case WD_TIMER://定时消息，刷新数据
			
			OLED_ShowNum(30,32,displayTest,5,FONT16);//添加代码，需要刷新的数据等
			OLED_Refresh_Gram();				//更新显示
			SetTimer(10);						//若重新设置定时器，可周期性定时刷新，界面添加，菜单一般不添加			
			break;			
	}
}

/*定速报警处理函数-------------------------------------------------------------------------------*/
void FixSpeed(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口
			
			OLED_ShowString(30,16,"FixSpeed",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;
		
		case WD_TIMER://定时消息，刷新数据
			
			OLED_ShowNum(30,32,displayTest,5,FONT16);//添加代码，需要刷新的数据等
			OLED_Refresh_Gram();				//更新显示
			SetTimer(20);						//若重新设置定时器，可周期性定时刷新，界面添加，菜单一般不添加			
			break;					
	}
}

/*保存数据处理函数-------------------------------------------------------------------------------*/
void SaveData(void)
{
	switch(message[0].msg)
	{
		case WD_PAINT://绘制窗口				
			
			OLED_ShowString(30,0,"SaveData",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;
	}
}

/*滑块设置处理函数-------------------------------------------------------------------------------*/
void SlidersSet(void)
{
	switch(message[0].msg)
	{		
		case WD_PAINT://绘制窗口			
			
			OLED_ShowString(30,20,"SlidersSet",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;				
	}
}

/*错误提示处理函数-------------------------------------------------------------------------------*/
void ErrorMsg(void)
{
	switch(message[0].msg)
	{			
		case WD_PAINT://窗口初始化
			
			OLED_ShowString(30,20,"ErrorMsg",FONT16,OLED_NORMAL);//绘制界面，背景等（图片）
		
			break;		
		
		case WD_TIMER://定时消息，刷新数据
			
			//添加代码，需要刷新的数据等
		
			//SetTimer(50);						//若重新设置定时器，可周期性定时刷新，界面添加，菜单一般不添加			
			break;				
	}
}

/*绘制菜单项函数---------------------------用户需要对a、b、c三处做修改------------------------------
//功能：绘制菜单项
//入参：cmd		 菜单动作，上移MENU_UP，下移MENU_DOWN，创建刷新MENU_CREAT
//		fontSize 字体大小
//
//备注：a和b两处是为了防止菜单项字数不同时出现叠加显示，故加了清除语句，(未完善)
//      c处是因为这里为OLED开辟了显存，需要刷新显示才能显示出变化。显存很占内存，没用到时删去
//----------------------------------------------------------------------------------------------*/
void MenuDraw(u8 cmd,u8 fontSize)
{
	u8 i,rever;
	u16 interval;	//菜单宽度
	u16 tmp;		//
	
	//参数有效则计算每个菜单项分配的宽度
	i=MenuTab[window.wndIndex].listShowNum;
	if((MenuTab[window.wndIndex].end > MenuTab[window.wndIndex].start) && i)
	{
		tmp=MenuTab[window.wndIndex].end - MenuTab[window.wndIndex].start + 1;
		interval = tmp / i;
	}
	else return;//宽度为0 或非法，显示0个菜单项，直接退出	
		
	switch(cmd)
	{
		case MENU_CREAT:	//创建或更新菜单，直接退出，菜单不变
						
			break;
		
		case MENU_UP:		//菜单上移
			
			if(curMenu.sel > 0)
			{
				curMenu.sel--;
				if(curMenu.sel<curMenu.head)
					curMenu.head=curMenu.sel;
			}
			else return;	//菜单已经在最开头，不再刷新
			break;
		
		case MENU_DOWN:		//菜单下移
			
			if(curMenu.sel < MenuTab[window.wndIndex].listMenuNum-1)
			{
				curMenu.sel++;
				if(curMenu.sel>curMenu.head+MenuTab[window.wndIndex].listShowNum-1)
					curMenu.head=curMenu.sel-MenuTab[window.wndIndex].listShowNum+1;
			}
			else return;	//菜单已经在最末尾，不再刷新
			break;
	}
	
	//更新菜单
	switch(MenuTab[window.wndIndex].direction)
	{
		case HORIZONTAL://水平方向，选中项以所在点为中心展开并反显
			
			
/* a:	*/	ClearLine(0,MenuTab[window.wndIndex].rowOrColumn,fontSize);//（未完善，现在直接写空格）
		
			for(i=0;i< MenuTab[window.wndIndex].listShowNum ;i++)
			{
				tmp=MenuTab[window.wndIndex].start + interval * i;
				if(i==curMenu.sel-curMenu.head) cmd=i;//保存选中项
				ShowString(tmp,MenuTab[window.wndIndex].rowOrColumn,interval,MenuTab[window.wndIndex].caption[curMenu.head+i].text,fontSize,HORIZONTAL);			
			}
			//把选中项挑出来，显示在最前端
//			if(0 == cmd)tmp=MenuTab[window.wndIndex].start;			
//			else 
//			{
//				StringLength(MenuTab[window.wndIndex].caption[curMenu.head+i].text,fontSize,&tmp);
//				if(MenuTab[window.wndIndex].listShowNum-1 == cmd)
//					 tmp=MenuTab[window.wndIndex].end - tmp;//选中的是最后一项，则从边界往前推，防止超出范围无法显示
//				else tmp=MenuTab[window.wndIndex].start + interval * cmd + interval / 2 - tmp;
//			}
			tmp=MenuTab[window.wndIndex].start + interval * cmd;
			ShowString(tmp,MenuTab[window.wndIndex].rowOrColumn,interval,MenuTab[window.wndIndex].caption[curMenu.head+cmd].text,fontSize,H_REVER);
			break;
			
		case VERTICAL:	//竖直方向，空间较大，全部是展开状态
			
/* b:	*/	ClearRow();//（未完善，现在直接清除全屏）
		
			for(i=0;i< MenuTab[window.wndIndex].listShowNum ;i++)
			{
				tmp=MenuTab[window.wndIndex].start + interval * i;
				if(i==curMenu.sel-curMenu.head) rever=V_REVER;
				else rever=VERTICAL;
				ShowString(MenuTab[window.wndIndex].rowOrColumn,tmp,interval,MenuTab[window.wndIndex].caption[curMenu.head+i].text,fontSize,rever);
				
			}
			break;
	}
	
// c:
	OLED_Refresh_Gram();				//更新显示
}

/*菜单引擎---------------------------------用户需要对a、b两处做修改------不用显存则删除d-----------*/
void MenuProcess(void)
{
	u8 tmp;
// a:	
	//测试用，变化的数字，查看自动更新的效果,实际用则删除
	displayTest++;
	if(displayTest>65534)displayTest=0;
// b:
	//处理按键，触发相应消息，做好按键处理函数
	if(single_click)
	{
		switch(single_click)
		{
			case KEY_UP:
				PostMessage(WD_KEY1,0);
				break;
			case KEY_DOWN:
				PostMessage(WD_KEY2,0);
				break;
			case KEY_OK:
				PostMessage(WD_KEY3,0);
				break;
			case KEY_BACK:
				PostMessage(WD_KEY4,0);
				break;
		}
		
		single_click=0;
	}
// c:	
	//消息执行，由于数据往显存里写，所以加了d处的更新显示
	if(message[0].msg)
	{
		switch(message[0].msg)
		{
			case WD_CREATE://创建窗口
				
				tmp=WindowCreat();					//创建子窗口，父窗口的信息入栈，更新curMenu
				if( tmp )break;						//创建失败则不重新绘制当前窗口，防止闪烁
											
				PostMessage(WD_PAINT,0);			//创建成功，发送窗口绘制消息（发送前子窗口已切换到前端）
				break;
			
			case WD_CLOSE://注销窗口
				
				tmp=WindowDestory();				//注销本窗口，父窗口信息出栈，更新curMenu（主菜单窗口禁止注销）
				if( tmp )break;						//注销失败则不重新绘制当前窗口，防止闪烁
										
				PostMessage(WD_PAINT,0);			//注销成功，发送窗口绘制消息（发送前父窗口已切换到前端）
				break;
			
			case WD_PAINT://绘制窗口
				
				if(message[0].wParam==0)OLED_Clear();//是否清除旧窗口，PostMessage(WD_PAINT,0)的第二个入参
				MenuDraw(MENU_CREAT,FONT16);		//绘制菜单项（MenuItem.text携带的标题，菜单必选，界面可选）
				//无break
			case WD_TIMER://定时消息
				
				(* window.FuncPoint)();				//执行顶层窗口更新函数
/* d */			OLED_Refresh_Gram();				//更新显示
				break;
			
			case WD_KEY1://按键1消息（一般用于向上翻菜单）
				
				MenuDraw(MENU_UP,FONT16);			//向上翻菜单
				break;
			
			case WD_KEY2://按键2消息（一般用于向下翻菜单）
				
				MenuDraw(MENU_DOWN,FONT16);		//向下翻菜单
				break;
			
			case WD_KEY3://按键3消息（一般用于确定、进入）
				
				PostMessage(WD_CREATE,0);			//发送窗口创建消息
				break;
			
			case WD_KEY4://按键4消息（一般用于取消、退出）
				
				PostMessage(WD_CLOSE,0);			//发送窗口注销消息
				break;		
		}
		PostMessage(MSG_CLR,0);	
	}

	//处理消息队列，第一条消息为空，后面的消息向前移动一个位置
	MsgQProcess();
		
	//处理窗口定时器，放在函数之后，以抵消window.timer==1时发送消息的误差
	if(window.timer)window.timer--;
	if(window.timer==1)PostMessage(WD_TIMER,0);//为0时，窗口定时器关闭，所以在减到1时发送，所以放在最后，下一周期生效	
}

//下面的函数不会出现其它模块的函数，无需更改

/*菜单初始化函数---------------------------------------------------------------------------------*/
void MenuInit(void)
{
	menuSP=0;
	
	window.wndIndex=NULL_MENU;	//启动窗为当前窗口
	window.timer=100;			//启动窗持续时间
	window.FuncPoint = MenuTab[NULL_MENU].CurOperate;	//指向StartMenu
	
	curMenu.hwndParent=0;		//父窗口为本身
	curMenu.head=0;
	curMenu.sel=0;
	
	PostMessage(WD_PAINT,0);	//发送窗口绘制消息,0 清除显示
}

/*消息队列处理函数-------------------------------------------------------------------------------*/
void MsgQProcess(void)
{
	u8 i;
	
	if(message[0].msg==0)
	{
		for(i=0;i<MSGQ_DEPTH-1;i++)
		{
			
			message[i].wParam=message[i+1].wParam;
			message[i].msg=message[i+1].msg;
			message[i].iParam=message[i+1].iParam;
			
		}
		message[i].wParam=0;
		message[i].msg=0;
		message[i].iParam=0;
	}
}

/*创建窗口函数-----------------------------------------------------------------------------------*/
u8 WindowCreat(void)
{
	u8 tmp;
	
	if(menuSP>=MENU_DEPTH)return 1;	//满栈则返回错误
	
	tmp=MenuTab[window.wndIndex].caption[curMenu.sel].param;//查找入口
	if(tmp == NULL_MENU) return 2;	//无子窗口，不创建，退出
	if(tmp == MAIN_MENU) 			//创建主菜单，要么是初始化后自动执行，要么是用户强制执行，按退出键不可能创建窗口，
	{								//按确定键，创建窗口时由系统自动完成，除非MenuItem入口参数写错
		menuSP=0;
		window.wndIndex=MAIN_MENU;	//主菜单为当前窗口
		window.timer=0;
		window.FuncPoint = MenuTab[MAIN_MENU].CurOperate;	//指向主菜单
		curMenu.hwndParent=MAIN_MENU;	//主菜单的父窗口为本身
		curMenu.head=0;
		curMenu.sel=0;
		return 0;
	}
	window.wndIndex=tmp;				//将窗口切换到对应子菜单
	if(MenuTab[window.wndIndex].listMenuNum) window.timer=0;//设置刷新时间，菜单不刷新，（依据是否有子菜单，0是界面，刷新）
	else window.timer=2;				//界面则自动刷新，具体时间随系统时间和REFRESH_TIME而定
	window.FuncPoint = MenuTab[window.wndIndex].CurOperate;	//指向新窗口的处理函数
	
	menu[menuSP].hwndParent = curMenu.hwndParent;
	menu[menuSP].head = curMenu.head;
	menu[menuSP].sel = curMenu.sel;
	
	curMenu.hwndParent=message[0].wParam;				//消息结构体参数1保存着父窗口索引
	curMenu.head=0;
	curMenu.sel=0;
		
	menuSP++;//指针加一
	return 0;
}

/*注销窗口函数-----------------------------------------------------------------------------------*/
u8 WindowDestory(void)
{
	if(menuSP==0)return 1;//空栈则返回错误
	
	menuSP--;//指针减一
	
	window.wndIndex=curMenu.hwndParent;				//切换到父窗口
	if(MenuTab[window.wndIndex].listMenuNum)window.timer=0;	//设置刷新时间，菜单不刷新，（依据是否有子菜单而设置）
	else window.timer=2;							//界面则自动刷新，具体时间随系统时间和REFRESH_TIME而定
	window.FuncPoint = MenuTab[window.wndIndex].CurOperate;	//指向新窗口的处理函数
	
	curMenu.hwndParent = menu[menuSP].hwndParent;	//更新父窗口
	curMenu.head = menu[menuSP].head;				//菜单首项出栈
	curMenu.sel = menu[menuSP].sel;					//当前选中菜单出栈
		
	return 0;
}

/*发送窗口消息函数--------------------------------------------------------------------------------
//功能：发送窗口消息
//入参：cmd		 命令（见DisplayMenu.h）
//		dat1	 参数
/-----------------------------------------------------------------------------------------------*/
void PostMessage(u8 cmd,u8 dat1)
{
	u8 i;
	
	if(MSG_CLR==cmd)//清除命令，清除队列第一个元素，退出
	{
		message[0].msg=0;					
		message[0].wParam=0;
		message[0].iParam=0;
		return;
	}
	//查找队列后的第一个空元素
	for(i=0;i<MSGQ_DEPTH;i++)
	{
		if(message[i].msg)continue;
		else
		{
			if(i<MSGQ_DEPTH-1)
			{
				if(!message[i+1].msg)break;
			}
			else break;
		}
	}
	if(MSGQ_DEPTH==i)return;//队列满，申请无效
	
	//把消息插入到队列后第一个空元素
	switch(cmd)
	{
		case WD_CREATE:
			message[i].msg=WD_CREATE;			//创建窗口命令
			message[i].wParam=window.wndIndex;	//保存当前窗作为进入子菜单后的父窗
			message[i].iParam=0;
		break;
		case WD_PAINT:
			message[i].msg=WD_PAINT;			//绘制窗口命令
			message[i].wParam=dat1;			//0清除父窗口，1叠加在父窗口上显示
			message[i].iParam=0;
		break;	
		case WD_CLOSE:
			message[i].msg=WD_CLOSE;			//注销窗口命令
			message[i].wParam=0;	
			message[i].iParam=0;
		break;
		case WD_TIMER:
			message[i].msg=WD_TIMER;			//窗口定时器命令
			message[i].wParam=0;
			message[i].iParam=0;
		break;
		case WD_KEY1:
			message[i].msg=WD_KEY1;			//按键命令
			message[i].wParam=0;
			message[i].iParam=0;
		break;
		case WD_KEY2:
			message[i].msg=WD_KEY2;			//按键命令
			message[i].wParam=0;
			message[i].iParam=0;
		break;
		case WD_KEY3:
			message[i].msg=WD_KEY3;			//按键命令
			message[i].wParam=0;
			message[i].iParam=0;			
		break;
		case WD_KEY4:
			message[i].msg=WD_KEY4;			//按键命令
			message[i].wParam=0;
			message[i].iParam=0;
		break;
	}
}

/*计算字符串长度函数------------------------------------------------------------------------------
//功能：计算字符串长度，中英文，以像素为单位
//入参：str		 参数指针
//		fontSize 字体大小
//		len      外部保存宽度的变量
//----------------------------------------------------------------------------------------------*/
void StringLength(u8 const * str,u8 fontSize,u16 * len)
{
	u8  bHz=0;     	//字符或者中文  
	u16 strlenth=0;
	
	while( *str != 0)	//数据未结束
	{
		if(!bHz)
		{
			if(*str> 0x80)bHz=1;
			else 
			{
				str++;
				strlenth += fontSize/2;
			}
		}
		else 
		{
			bHz=0;
			str+=2;
			strlenth += fontSize;
		}
	}
	*len = strlenth;
}

