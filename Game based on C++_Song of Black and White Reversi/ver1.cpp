// Ver1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"                                     //C语言函数库
#include <graphics.h>                                   //Easyx辅助函数库
#include <conio.h>                                      //Easyx辅助函数库
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>                                    //睡眠函数所在库---Sleep()|||电脑键盘感应
#include <time.h>                                       //时间函数所在库---用于srand()随机种子获取
#include <math.h>                                       //数学函数库-------绝对值引用-确定鼠标位置
#include <string.h>                                     //关于调用字符串有关函数
#include <tchar.h>                                      //添加对TCHAR的支持
#include <mmsystem.h>                                   //有关获取鼠标位置的函数库
#pragma comment(lib,"Winmm.lib")                        //引用 Windows Multimedia API-鼠标位置

#define X 1200                                          //定义界面长X,宽Y
#define Y 800
#define PX 280			                                //棋盘偏移量X
#define PY 80		        	                        //棋盘偏移量Y
#define BBLACK 80		                                //空格大小
#define CHESS_SIZE 25	                                //棋子尺寸
#define SIZE 8			                                //棋盘格数
#define THICKNESS  3                                    //棋盘线的粗细

int map[SIZE][SIZE];                                    //定义“棋盘格地图”--“1”放置黑子--“-1”放置白子--“2”显示黑子可落点--“-2”显示白子可落点--“3”当前黑子落子点--“-3”当前白子落子点
int BlackNum;                                           //黑子数量
int BlackValid;                                         //黑子可落点数量-----------------------------------not be used 
int WhiteNum;                                           //白子数量
int WhiteVaild;                                         //白子可落点数量-----------------------------------not be used 
int Order_B_W;                                          //当前用户的执子为“黑”---赋值为"1"|||“白”---赋值为"-1"----(输入到函数中，作为"Chess_W_B"在函数中起作用)
int Player_move;                                        //鼠标玩家是否落子----“0”玩家未落子----“2”玩家已落子
int Boundary;                                           //界面选择---（0）开始start页面||（-1）难度choice选择页面||（-2）胜利win页面||（-3）故事story页面||（-4）结束over页面||（-5）规则介绍||（1）对战页面
int Class;                                              //难度等级选择-------------------（1）新手村beginner页面||（2）简单easy页面||（3）中等middle页面||（4）困难hard页面
int *ComAI_xy;                                          //指针定义----电脑判断下一步棋返回值--位置坐标
int WINNER;                                             //胜利一方----（-1）电脑胜利||（1）玩家胜利


IMAGE img_game1_beginner;     //背景图片

							  //初始化---------------------------------------------------------------主体函数
void startup()
{
	initgraph(X, Y);                                 //初始化绘图环境

	for (int i = 0; i < SIZE; i++)                   //8*8--64宫格循环查找||所有棋盘格初始化为0
		for (int j = 0; j < SIZE; j++)
			map[i][j] = 0;
	map[3][3] = 1;                                   //中间4个棋盘格棋子初始化
	map[4][4] = 1;
	map[3][4] = -1;
	map[4][3] = -1;

	BlackNum = 0;                                    //黑子数量初始化为0
	WhiteNum = 0;                                    //白字数量初始化为0
	BlackValid = 0;                                  //黑子有效位置数--即可落子点数--为0
	WhiteVaild = 0;                                  //白子有效位置数--即可落子点数--为0
	Order_B_W = 1;                                   //初始化当前鼠标用户执子为"0"无人执子
	Player_move = 0;                                 //初始化为“0”玩家尚未落子
	Boundary = 0;                                    //显示页面初始化
	Class = 0;                                       //对战等级初始化
	WINNER = 0;                                      //得胜方未知

	ComAI_xy = (int *)malloc(2 * sizeof(int));       //指针--分配内存---电脑判断下一步棋返回值--位置坐标
	if (ComAI_xy == NULL)
	{
		printf("内存不足");                           //验证--内存不足时：强行退出
		exit(0);
	}

	//图片导入

	loadimage(&img_game1_beginner, _T("D:\\STUDY\\computer\\C\\Procedure\\VisualStudio2017\\HOMEWORK\\BattleOfWitsChessGame\\PicturesOfOWCGOMO\\img_game1_beginner.jpg"), X, Y);           //“新手村”背景

	BeginBatchDraw();                                //初始化批量绘图
}

//显示----------------------------------------------------------------主体函数
void show()
{
	putimage(0, 0, &img_game1_beginner);                 //放置**新手村**背景图片

	setlinecolor(YELLOW);                                                      //棋盘线显示黄色
	for (int i = 0; i <= SIZE; i++)                                            //绘制棋盘
	{
		for (int j = 0; j <= THICKNESS; j++)                                   //循环绘制加粗棋盘横格
			line(PX, PY + i * BBLACK + j, PX + 8 * BBLACK, PY + i * BBLACK + j);
		for (int j = 0; j <= THICKNESS; j++)                                   //循环绘制加粗棋盘竖格
			line(PX + i * BBLACK + j, PY, PX + i * BBLACK + j, PY + 8 * BBLACK);
	}

	for (int i = 0; i < SIZE; i++)                                             //8*8--64宫格w循环查找棋盘格---放置棋子“黑&&白”|可落子点“红圈”
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (map[i][j] == 1)                                                //如果棋盘格为1---放置黑子
			{
				setfillcolor(BLACK);                                           //黑色填充--绘制圆形
				solidcircle(PX + BBLACK / 2 + i * BBLACK, PY + BBLACK / 2 + j * BBLACK, CHESS_SIZE);
			}
			else if (map[i][j] == -1)                                          //如果棋盘格为-1---放置白子
			{
				setfillcolor(RGB(255, 255, 255));                              //白色填充--绘制圆形
				solidcircle(PX + BBLACK / 2 + i * BBLACK, PY + BBLACK / 2 + j * BBLACK, CHESS_SIZE);
			}
			else if (map[i][j] == 2 || map[i][j] == -2)                        //如果棋盘格为（2*Chess_W_B）---放置红圈
			{
				setlinecolor(RED);                                             //红边空心圆绘制--显示“（Chess_W_B）子”可落点
				circle(PX + BBLACK / 2 + i * BBLACK, PY + BBLACK / 2 + j * BBLACK, (int)(CHESS_SIZE * 0.28));
			}
		}
	}

	FlushBatchDraw();                                    //批量绘图
	Sleep(1);                                            //睡眠
}

//与用户输入有关-------------------------------------------------------主体函数
void updatewithinput()
{
	MOUSEMSG m;		                                                           //定义鼠标消息
	while (MouseHit())                                                          //检测当前鼠标消息
	{
		m = GetMouseMsg();

		if (m.uMsg == WM_LBUTTONDOWN)                                           //如果按下鼠标左键-----双击左键
		{
			for (int i = 0; i < SIZE; i++)                                      //64宫格循环查找
			{
				for (int j = 0; j < SIZE; j++)
				{
					if (abs(m.x - (PX + BBLACK / 2 + i * BBLACK)) < BBLACK / 2 && abs(m.y - (PY + BBLACK / 2 + j * BBLACK)) < BBLACK / 2)        //范围确认
					{
						Player_move = 1;                                    //确认玩家已落子
						map[i][j] = Order_B_W;                              //鼠标区域放置当前棋子（Order_B_W）
						Order_B_W = -Order_B_W;                             //轮换为电脑执子
					}
				}
			}
		}
	}
}

//与用户输入无关-------------------------------------------------------主体函数
void updatewithoutinput()
{

}

//游戏结束--后续处理---------------------------------------------------主体函数
void BOWCGOMOover()
{
	free(ComAI_xy);                      //释放内存
	EndBatchDraw();                      //结束批量绘图
	_getch();
	closegraph();                        //关闭绘图环境
}

//主函数
int main()
{
	char input;                                                                 //定义字符

	startup();                           //数据初始化
	while (1)                            //无限次无条件循环
	{
		updatewithoutinput();            //与用户输入无关
		updatewithinput();               //与用户输入有关
		show();                          //显示
	}
	BOWCGOMOover();                      //游戏结束---后续处理
	return 0;
}




