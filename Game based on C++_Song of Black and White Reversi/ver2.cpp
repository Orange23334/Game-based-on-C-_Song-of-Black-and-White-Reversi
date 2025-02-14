// Code2.cpp : Defines the entry point for the console application.
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



IMAGE img_game1_beginner;     //背景图片--四种难度背景图（入门级，简单，中等，困难）

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
	Class = 1;                                       //对战等级初始化
	WINNER = 0;                                      //得胜方未知

	ComAI_xy = (int *)malloc(2 * sizeof(int));       //指针--分配内存---电脑判断下一步棋返回值--位置坐标
	if (ComAI_xy == NULL)
	{
		printf("内存不足");                           //验证--内存不足时：强行退出
		exit(0);
	}

	//图片导入
	loadimage(&img_game1_beginner, _T("G:\\PicturesMATERIAL\\img_game1_beginner.jpg"), X, Y);           //“入门级”背景

	BeginBatchDraw();                                //初始化批量绘图
}

//可执行点清理-----------------------------边缘函数
//输入：无
//功能：清理当前所有可执行点（（‘2’||‘-2’->->->‘0’））
//输出：无
void Valid_Clear()
{
	for (int i = 0; i < SIZE; i++)                      //8*8--64宫格循环查找
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (map[i][j] == 2 || map[i][j] == -2)                         //有效可落子点清理
				map[i][j] = 0;
		}
	}
}

//棋子数量统计-----------------------------边缘函数
//输入：当前用户（电脑或者玩家）执子类型Order_B_W(->Chess_W_B)
//用处：统计Order_B_W棋子数量
//输出：棋子数量
int Statistics_Number(int Chess_W_B)
{
	int Chess_Number = 0;                                //棋子数目

	for (int i = 0; i < SIZE; i++)                       //8*8==64宫格循环查找
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (map[i][j] == Chess_W_B)                 //找到这一类型棋子
				Chess_Number++;                         //数目统计统计
		}
	}

	return Chess_Number;
}

//有效位置--即可落子点--查找----------------------------------重要函数
//输入：当前用户（电脑或者玩家）执子类型Order_B_W(->Chess_W_B)
//用处：查找当前用户有效可落子点
//输出：无
void Valid_Location(int Chess_W_B)
{
	for (int i = 0; i < SIZE; i++)                     //8*8--64宫格循环查找
	{
		for (int j = 0; j < SIZE; j++)
		{
			//*****************************************（i,j）处为（Chess_W_B）--已落（Chess_W_B）--判断周围是否有（-Chess_W_B）
			if (map[i][j] == Chess_W_B)
			{//*****************************************************************************
				if ((map[i - 1][j - 1] == -Chess_W_B) && ((i - 1) != 0) && ((j - 1) != 0))        //******左上--有（-Chess_W_B）&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j - k] != 2 * Chess_W_B)                                    //判断左上角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j - k] == -Chess_W_B) && ((i - k) != 0) && ((j - k) != 0))//如果下一个左上角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个左上角循环
						else
						{
							if ((map[i - k][j - k] == 0) || (map[i - k][j - k] == -2 * Chess_W_B))//如果当前左上角位置为0或(-2*Chess_W_B)
								map[i - k][j - k] = 2 * Chess_W_B;                                //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向左上角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i][j - 1] == -Chess_W_B) && ((j - 1) != 0))                              //******正上--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i][j - k] != 2 * Chess_W_B)                                        //判断正上方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i][j - k] == -Chess_W_B) && ((j - k) != 0))                      //如果下一个正上方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正上方循环
						else
						{
							if ((map[i][j - k] == 0) || (map[i][j - k] == -2 * Chess_W_B))        //如果当前正上方位置为0或(-2*Chess_W_B)
								map[i][j - k] = 2 * Chess_W_B;                                    //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向正上方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j - 1] == -Chess_W_B) && ((i + 1) != 7) && ((j - 1) != 0))        //******右上--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j - k] != 2 * Chess_W_B)                                    //判断右上角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j - k] == -Chess_W_B) && ((i + k) != 7) && ((j - k) != 0))//如果下一个右上角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个右上角循环
						else
						{
							if ((map[i + k][j - k] == 0) || (map[i + k][j - k] == -2 * Chess_W_B))//如果当前右上角位置为0或(-2*Chess_W_B)
								map[i + k][j - k] = 2 * Chess_W_B;                                //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向右上角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i - 1][j] == -Chess_W_B) && ((i - 1) != 0))                              //******正左--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j] != 2 * Chess_W_B)                                        //判断正左方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j] == -Chess_W_B) && ((i - k) != 0))                      //如果下一个正左方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正左方循环
						else
						{
							if ((map[i - k][j] == 0) || (map[i - k][j] == -2 * Chess_W_B))        //如果当前正左方位置为0或(-2*Chess_W_B)
								map[i - k][j] = 2 * Chess_W_B;                                    //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向正左方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j] == -Chess_W_B) && ((i + 1) != 7))                              //******正右--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j] != 2 * Chess_W_B)                                        //判断正右方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j] == -Chess_W_B) && ((i + k) != 7))                      //如果下一个正右方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正右方循环
						else
						{
							if ((map[i + k][j] == 0) || (map[i - k][j] == -2 * Chess_W_B))        //如果当前正右方位置为0或(-2*Chess_W_B)
								map[i + k][j] = 2 * Chess_W_B;                                    //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向正右方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i - 1][j + 1] == -Chess_W_B) && ((i - 1) != 0) && ((j + 1) != 7))        //******左下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j + k] != 2 * Chess_W_B)                                    //判断左下角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j + k] == -Chess_W_B) && ((i - k) != 0) && ((j + k) != 7))//如果下一个左下角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个左下角循环
						else
						{
							if ((map[i - k][j + k] == 0) || (map[i - k][j + k] == -2 * Chess_W_B))//如果当前左下角位置为0或(-2*Chess_W_B)
								map[i - k][j + k] = 2 * Chess_W_B;                                //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向左下角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i][j + 1] == -Chess_W_B) && ((j + 1) != 7))                              //******正下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i][j + k] != 2 * Chess_W_B)                                        //判断正下方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i][j + k] == -Chess_W_B) && ((j + k) != 7))                      //如果下一个正下方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正下方循环
						else
						{
							if ((map[i][j + k] == 0) || (map[i][j + k] == -2 * Chess_W_B))        //如果当前正下方位置为0或(-2*Chess_W_B)
								map[i][j + k] = 2 * Chess_W_B;                                    //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向正下方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j + 1] == -Chess_W_B) && ((i + 1) != 7) && ((j + 1) != 7))        //******右下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j + k] != 2 * Chess_W_B)                                    //判断右下角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j + k] == -Chess_W_B) && ((i + k) != 7) && ((j + k) != 7))//如果下一个右下角循环值有白（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个右下角循环
						else
						{
							if ((map[i + k][j + k] == 0) || (map[i + k][j + k] == -2 * Chess_W_B))//如果当前右下角位置为0或(-2*Chess_W_B)
								map[i + k][j + k] = 2 * Chess_W_B;                                //放置（Chess_W_B）子有效可落子点
							break;                                                                //只要only--向右下角的循环条件不满足时--退出while循环
						}
					}
				}
			}
		}
	}
}

//有效位置--即可落子点数--统计--------------------------------重要函数
//输入：当前用户（电脑或者玩家）执子类型Order_B_W(->Chess_W_B)
//用处：当前用户有效可落子点数目统计
//输出：当前用户有效可落子点数目
int Statistics_Valid_Location(int Chess_W_B)
{
	int Valid_Chess_Num = 0;                                                                      //定义有效可落子点数目

	for (int i = 0; i < SIZE; i++)                     //8*8--64宫格循环查找
	{
		for (int j = 0; j < SIZE; j++)
		{
			//*****************************************（i,j）处为（Chess_W_B）--已落（Chess_W_B）--判断周围是否有（-Chess_W_B）
			if (map[i][j] == Chess_W_B)
			{//*****************************************************************************
				if ((map[i - 1][j - 1] == -Chess_W_B) && ((i - 1) != 0) && ((j - 1) != 0))        //******左上--有（-Chess_W_B）&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j - k] != 2 * Chess_W_B)                                    //判断左上角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j - k] == -Chess_W_B) && ((i - k) != 0) && ((j - k) != 0))//如果下一个左上角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个左上角循环
						else
						{
							if ((map[i - k][j - k] == 0) || (map[i - k][j - k] == -2 * Chess_W_B))//如果当前左上角位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向左上角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i][j - 1] == -Chess_W_B) && ((j - 1) != 0))                              //******正上--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i][j - k] != 2 * Chess_W_B)                                        //判断正上方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i][j - k] == -Chess_W_B) && ((j - k) != 0))                      //如果下一个正上方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正上方循环
						else
						{
							if ((map[i][j - k] == 0) || (map[i][j - k] == -2 * Chess_W_B))        //如果当前正上方位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向正上方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j - 1] == -Chess_W_B) && ((i + 1) != 7) && ((j - 1) != 0))        //******右上--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j - k] != 2 * Chess_W_B)                                    //判断右上角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j - k] == -Chess_W_B) && ((i + k) != 7) && ((j - k) != 0))//如果下一个右上角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个右上角循环
						else
						{
							if ((map[i + k][j - k] == 0) || (map[i + k][j - k] == -2 * Chess_W_B))//如果当前右上角位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向右上角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i - 1][j] == -Chess_W_B) && ((i - 1) != 0))                              //******正左--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j] != 2 * Chess_W_B)                                        //判断正左方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j] == -Chess_W_B) && ((i - k) != 0))                      //如果下一个正左方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正左方循环
						else
						{
							if ((map[i - k][j] == 0) || (map[i - k][j] == -2 * Chess_W_B))        //如果当前正左方位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向正左方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j] == -Chess_W_B) && ((i + 1) != 7))                              //******正右--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j] != 2 * Chess_W_B)                                        //判断正右方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j] == -Chess_W_B) && ((i + k) != 7))                      //如果下一个正右方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正右方循环
						else
						{
							if ((map[i + k][j] == 0) || (map[i - k][j] == -2 * Chess_W_B))        //如果当前正右方位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向正右方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i - 1][j + 1] == -Chess_W_B) && ((i - 1) != 0) && ((j + 1) != 7))        //******左下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i - k][j + k] != 2 * Chess_W_B)                                    //判断左下角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i - k][j + k] == -Chess_W_B) && ((i - k) != 0) && ((j + k) != 7))//如果下一个左下角循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个左下角循环
						else
						{
							if ((map[i - k][j + k] == 0) || (map[i - k][j + k] == -2 * Chess_W_B))//如果当前左下角位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向左下角的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i][j + 1] == -Chess_W_B) && ((j + 1) != 7))                              //******正下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i][j + k] != 2 * Chess_W_B)                                        //判断正下方是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i][j + k] == -Chess_W_B) && ((j + k) != 7))                      //如果下一个正下方循环值有（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个正下方循环
						else
						{
							if ((map[i][j + k] == 0) || (map[i][j + k] == -2 * Chess_W_B))        //如果当前正下方位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向正下方的循环条件不满足时--退出while循环
						}
					}
				}
				if ((map[i + 1][j + 1] == -Chess_W_B) && ((i + 1) != 7) && ((j + 1) != 7))        //******右下--有（-Chess_W_B）子&&且不挨着边界
				{
					int k = 2;
					while (map[i + k][j + k] != 2 * Chess_W_B)                                    //判断右下角是否有（Chess_W_B）棋有效可落子点---如果有-跳出||如果没有-循环
					{
						if ((map[i + k][j + k] == -Chess_W_B) && ((i + k) != 7) && ((j + k) != 7))//如果下一个右下角循环值有白（-Chess_W_B）子且不挨着边界
							k++;                                                                  //向下一个右下角循环
						else
						{
							if ((map[i + k][j + k] == 0) || (map[i + k][j + k] == -2 * Chess_W_B))//如果当前右下角位置为0或(-2*Chess_W_B)
								Valid_Chess_Num++;                                                //当前棋子的有效可落子点数加一
							break;                                                                //只要only--向右下角的循环条件不满足时--退出while循环
						}
					}
				}
			}
		}
	}

	return Valid_Chess_Num;
}

//棋子转换---------------------------------------------------重要函数
//输入：当前用户（电脑或玩家）的落子点的横纵坐标(Move_x,Move_y)&&当前用户（电脑或玩家）执子类型Order_B_W->Chess_W_B
//用处：棋子类型转化---从落子点出发--八个方向查找进行类型转化
//输出：无
void Conversion_Chess(int Move_x, int Move_y, int Chess_W_B)
{
	int i = Move_x, j = Move_y;                                                  //把(Move_x,Move_y)赋值给(i,j)---从(i,j)出发开始想八个方向查找

	int Flag_CVS = 0;                                                            //定义可转化标志Flag_CVS--（0）不可转化||（1）可转化
	int k = 2;                                                                   //下标循环辅助数

																				 //*****************************************左上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j - 1] == -Chess_W_B) && (i - 1 != 0) && (j - 1 != 0))
	{
		if ((map[i - k][j - k] == -Chess_W_B) && (i - k != 0) && (j - k != 0))   //如果下一个左上角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个左上角循环
		else                                                                     //下一个左上角的循环值不符合条件
		{
			if (map[i - k][j - k] == Chess_W_B)                                  //如果下一个左上角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //左上角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向左上角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志Flag_CVS为（1）--可转化
	{
		k = 1;
		while (map[i - k][j - k] == -Chess_W_B)                                  //---左上角——有（-Chess_W_B）子&&向左上角循环转化
		{
			map[i - k][j - k] = Chess_W_B;                                       //左上角每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //****************************************正上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i][j - 1] == -Chess_W_B) && (j - 1 != 0))
	{
		if ((map[i][j - k] == -Chess_W_B) && (j - k != 0))                       //如果下一个正上方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正上方循环
		else                                                                     //下一个正上方的循环值不符合条件
		{
			if (map[i][j - k] == Chess_W_B)                                      //如果下一个正上方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正上方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正上方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i][j - k] == -Chess_W_B)                                      //---正上方——有（-Chess_W_B）子&&向正上方循环转化
		{
			map[i][j - k] = Chess_W_B;                                           //正上方每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //*****************************************右上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j - 1] == -Chess_W_B) && (i + 1 != 7) && (j - 1 != 0))
	{
		if ((map[i + k][j - k] == -Chess_W_B) && (i + k != 7) && (j - k != 0))   //如果下一个右上角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个右上角循环
		else                                                                     //下一个右上角的循环值不符合条件
		{
			if (map[i + k][j - k] == Chess_W_B)                                  //如果下一个右上角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //右上角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向右上角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j - k] == -Chess_W_B)                                  //---右上角——有（-Chess_W_B）子&&向右上角循环转化
		{
			map[i + k][j - k] = Chess_W_B;                                       //右上角每一个有（-Chess_W_B）子转化为（-Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //****************************************正左方--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j] == -Chess_W_B) && (i - 1 != 0))
	{
		if ((map[i - k][j] == -Chess_W_B) && (i - k != 0))                       //如果下一个正左方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正左方循环
		else                                                                     //下一个正左方的循环值不符合条件
		{
			if (map[i - k][j] == Chess_W_B)                                      //如果下一个正左方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正左方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正左方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i - k][j] == -Chess_W_B)                                      //---正左方——有（-Chess_W_B）子&&向正左方循环转化
		{
			map[i - k][j] = Chess_W_B;                                           //正左方每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //****************************************正右方--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j] == -Chess_W_B) && (i + 1 != 7))
	{
		if ((map[i + k][j] == -Chess_W_B) && (i + k != 7))                       //如果下一个正右方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正右方循环
		else                                                                     //下一个正右方的循环值不符合条件
		{
			if (map[i + k][j] == Chess_W_B)                                      //如果下一个正右方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正右方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正右方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j] == -Chess_W_B)                                      //---正右方——有（-Chess_W_B）子&&向正右方循环转化
		{
			map[i + k][j] = Chess_W_B;                                           //正右方每一个有（-Chess_W_B）子转化为（-Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //******************************************左下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j + 1] == -Chess_W_B) && (i - 1 != 0) && (j + 1 != 7))
	{
		if ((map[i - k][j + k] == -Chess_W_B) && (i - k != 0) && (j + k != 7))   //如果下一个左下角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个左下角循环
		else                                                                     //下一个左下角的循环值不符合条件
		{
			if (map[i - k][j + k] == Chess_W_B)                                  //如果下一个左下角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //左下角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向左下角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i - k][j + k] == -Chess_W_B)                                  //---左下角——有（-Chess_W_B）子&&向左下角循环转化
		{
			map[i - k][j + k] = Chess_W_B;                                       //左下角每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //******************************************正下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i][j + 1] == -Chess_W_B) && (j + 1 != 7))
	{
		if ((map[i][j + k] == -Chess_W_B) && (j + k != 7))                       //如果下一个正下方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正下方循环
		else                                                                     //下一个正下方的循环值不符合条件
		{
			if (map[i][j + k] == Chess_W_B)                                      //如果下一个正下方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正下方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正下方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i][j + k] == -Chess_W_B)                                      //---正下方——有（-Chess_W_B）子&&向正下方循环转化
		{
			map[i][j + k] = Chess_W_B;                                           //正下方每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //******************************************右下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j + 1] == -Chess_W_B) && (i + 1 != 7) && (j + 1 != 7))
	{
		if ((map[i + k][j + k] == -Chess_W_B) && (i + k != 7) && (j + k != 7))   //如果下一个右下角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个右下角循环
		else                                                                     //下一个右下角的循环值不符合条件
		{
			if (map[i + k][j + k] == Chess_W_B)                                  //如果下一个右下角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //右下角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向右下角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j + k] == -Chess_W_B)                                  //---右下角——有（-Chess_W_B）子&&向右下角循环转化
		{
			map[i + k][j + k] = Chess_W_B;                                       //右下角每一个有（-Chess_W_B）子转化为（Chess_W_B）子
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
}

//棋子可转换““““最大数目””””统计------------------------边缘函数
//输入：当前用户（电脑或玩家）的落子点的横纵坐标(Move_x,Move_y)&&当前用户（电脑或玩家）执子类型Order_B_W->Chess_W_B
//用处：可以转化的棋子数目统计---从落子点出发--八个方向查找进行类型转化查找
//输出：可以转化的““““最大棋子数目””””
int Statistics_Conversion_Chess(int Move_x, int Move_y, int Chess_W_B)
{
	int i = Move_x, j = Move_y;                                                  //把(Move_x,Move_y)赋值给(i,j)---从(i,j)出发开始想八个方向查找

	int Flag_CVS = 0;                                                            //定义可转化标志Flag_CVS--（0）不可转化||（1）可转化
	int k = 2;                                                                   //下标循环辅助数
	int Sta_CVS_Num = 0;                                                         //在此落子点（Move_x,Move_y）当前““方向””可以转换的棋子数目

																				 //*****************************************左上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j - 1] == -Chess_W_B) && (i - 1 != 0) && (j - 1 != 0))
	{
		if ((map[i - k][j - k] == -Chess_W_B) && (i - k != 0) && (j - k != 0))   //如果下一个左上角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个左上角循环
		else                                                                     //下一个左上角的循环值不符合条件
		{
			if (map[i - k][j - k] == Chess_W_B)                                  //如果下一个左上角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //左上角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向左上角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志Flag_CVS为（1）--可转化
	{
		k = 1;
		while (map[i - k][j - k] == -Chess_W_B)                                  //---左上角——有（-Chess_W_B）子&&向左上角循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //****************************************正上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i][j - 1] == -Chess_W_B) && (j - 1 != 0))
	{
		if ((map[i][j - k] == -Chess_W_B) && (j - k != 0))                       //如果下一个正上方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正上方循环
		else                                                                     //下一个正上方的循环值不符合条件
		{
			if (map[i][j - k] == Chess_W_B)                                      //如果下一个正上方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正上方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正上方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i][j - k] == -Chess_W_B)                                      //---正上方——有（-Chess_W_B）子&&向正上方循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;                                                                       //下表返回初始值

																				 //*****************************************右上--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j - 1] == -Chess_W_B) && (i + 1 != 7) && (j - 1 != 0))
	{
		if ((map[i + k][j - k] == -Chess_W_B) && (i + k != 7) && (j - k != 0))   //如果下一个右上角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个右上角循环
		else                                                                     //下一个右上角的循环值不符合条件
		{
			if (map[i + k][j - k] == Chess_W_B)                                  //如果下一个右上角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //右上角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向右上角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j - k] == -Chess_W_B)                                  //---右上角——有（-Chess_W_B）子&&向右上角循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //****************************************正左方--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j] == -Chess_W_B) && (i - 1 != 0))
	{
		if ((map[i - k][j] == -Chess_W_B) && (i - k != 0))                       //如果下一个正左方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正左方循环
		else                                                                     //下一个正左方的循环值不符合条件
		{
			if (map[i - k][j] == Chess_W_B)                                      //如果下一个正左方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正左方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正左方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i - k][j] == -Chess_W_B)                                      //---正左方——有（-Chess_W_B）子&&向正左方循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //****************************************正右方--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j] == -Chess_W_B) && (i + 1 != 7))
	{
		if ((map[i + k][j] == -Chess_W_B) && (i + k != 7))                       //如果下一个正右方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正右方循环
		else                                                                     //下一个正右方的循环值不符合条件
		{
			if (map[i + k][j] == Chess_W_B)                                      //如果下一个正右方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正右方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正右方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j] == -Chess_W_B)                                      //---正右方——有（-Chess_W_B）子&&向正右方循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //******************************************左下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i - 1][j + 1] == -Chess_W_B) && (i - 1 != 0) && (j + 1 != 7))
	{
		if ((map[i - k][j + k] == -Chess_W_B) && (i - k != 0) && (j + k != 7))   //如果下一个左下角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个左下角循环
		else                                                                     //下一个左下角的循环值不符合条件
		{
			if (map[i - k][j + k] == Chess_W_B)                                  //如果下一个左下角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //左下角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向左下角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i - k][j + k] == -Chess_W_B)                                  //---左下角——有（-Chess_W_B）子&&向左下角循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //******************************************正下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i][j + 1] == -Chess_W_B) && (j + 1 != 7))
	{
		if ((map[i][j + k] == -Chess_W_B) && (j + k != 7))                       //如果下一个正下方循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个正下方循环
		else                                                                     //下一个正下方的循环值不符合条件
		{
			if (map[i][j + k] == Chess_W_B)                                      //如果下一个正下方的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //正下方的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向正下方的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i][j + k] == -Chess_W_B)                                      //---正下方——有（-Chess_W_B）子&&向正下方循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}
	k = 2;																	     //下表返回初始值

																				 //******************************************右下--有（-Chess_W_B）&&且不挨着边界
	while ((map[i + 1][j + 1] == -Chess_W_B) && (i + 1 != 7) && (j + 1 != 7))
	{
		if ((map[i + k][j + k] == -Chess_W_B) && (i + k != 7) && (j + k != 7))   //如果下一个右下角循环值有（-Chess_W_B）子且不挨着边界
			k++;                                                                 //向下一个右下角循环
		else                                                                     //下一个右下角的循环值不符合条件
		{
			if (map[i + k][j + k] == Chess_W_B)                                  //如果下一个右下角的循环值为（Chess_W_B）
				Flag_CVS = 1;                                                    //右下角的循环转化可以开始--循环转化标志为（1）
			break;                                                               //只要only--向右下角的循环条件不满足时--退出while循环
		}
	}
	if (Flag_CVS)                                                                //如果转化标志为（1）--可转化
	{
		k = 1;
		while (map[i + k][j + k] == -Chess_W_B)                                  //---右下角——有（-Chess_W_B）子&&向右下角循环转化
		{
			Sta_CVS_Num++;                                                       //左上角每一个有（-Chess_W_B）子可以转化为（Chess_W_B）子---Sta_CVS_Num统计数据加一
			k++;
		}
		Flag_CVS = 0;                                                            //转化标志返回初始值
	}

	return Sta_CVS_Num;
}

//胜负判断---------------------------------边缘函数
//输入：无
//用处：判断谁胜谁负
//输出：无
void Victory_Or_Defeat()
{
	int reminder_chess = Statistics_Number(0);                      //定义剩余空棋盘数量并求出

	if (reminder_chess == 0)                                        //如果--未剩余棋盘格为0----判断哪一方棋子数多
	{
		Boundary = -2;                                              //跳转到胜利页面Win

		if (Statistics_Number(1) >= Statistics_Number(-1))          //如果--黑气数目多于白棋
			WINNER = 1;                                             //胜利者为黑棋
		else                                                        //否则
			WINNER = -1;                                            //胜利者为白棋
	}
	else                                                            //如果剩余空棋盘数目不为0--有效位置可落子点查找&&判断有效可落子点数量
	{
		if (Statistics_Valid_Location(1) == 0)                      //如果黑棋没有效可落子点--无棋可下
		{
			Boundary = -2;                                          //跳转到胜利页面Win
			WINNER = -1;                                            //胜利者为白棋 
		}
		if (Statistics_Valid_Location(-1) == 0)                     //如果白棋没有效可落子点--无棋可下
		{
			Boundary = -2;                                          //跳转到胜利页面Win
			WINNER = 1;                                             //胜利者为黑棋
		}
	}
}

//电脑-人工智能-对战等级（1）入门级（2）简单（3）中等（4）困难-AI重要函数
//输入：难度等级Class->cl，，当前执子类型：Order_W_B->Chess_W_B
//用处：指针传递——ComAI_xy[0]与ComAI_xy[1]分别作为电脑落子点的横纵坐标
//输出：无
void Com_AI_Work_xy(int cl, int Chess_W_B)
{
	int i, j;                                               //定义i,j作为落子点的横纵坐标(i,j)

	if (cl == 1)                                            //输入等级Class为1时，电脑新手村程序智商
	{
		srand((int)time(0));                                //随机种子
		i = rand() % 8, j = rand() % 8;                     //i与j取0~7内的任意数
		for (; i < SIZE; i++)
		{
			for (; j < SIZE; j++)
			{
				if (map[i][j] == 2 * Chess_W_B)             //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
					break;
				Sleep(50);
			}

			if (map[i][j] == 2 * Chess_W_B)                 //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
				break;
			Sleep(50);

			if (i >= 7)                                     //数组下标i的循环赋值
				i = -1;
			if (j >= 7)                                     //数组下标j的循环赋值
				j = 0;
		}


		ComAI_xy[0] = i;                                        //i,j,分别赋值给ComAI_xy---传递指针，作为电脑落子的坐标
		ComAI_xy[1] = j;
	}

	if (cl == 2)											      		       //输入等级Class为2时，电脑简单AI_easy程序智商
	{
	}

	if (cl == 3)                                                               //输入等级Class为3时，电脑中等AI_middle程序智商
	{
	}

	if (cl == 4)                                         //输入等级Class为4时，电脑困难AI_hard程序智商
	{
	}

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
						if (map[i][j] == 2 * Order_B_W)                         //如果当下位置为可落子点（2*Order_B_W）
						{
							Player_move = 1;                                    //确认玩家已落子
							map[i][j] = Order_B_W;                              //鼠标区域放置当前棋子（Order_B_W）
							Conversion_Chess(i, j, Order_B_W);                  //棋子转换
							Order_B_W = -Order_B_W;                             //轮换为电脑执子
						}
					}
				}
			}

			Valid_Clear();                                                      //过去有效值清理
			Victory_Or_Defeat();                                                //胜负判断
																				//Valid_Clear();                                                    //过去有效值清理
			Valid_Location(Order_B_W);                                          //电脑有效可落子点查找

			BlackNum = Statistics_Number(1);                                    //黑子数量统计
			WhiteNum = Statistics_Number(-1);                                   //白字数量统计


		}

	}

}

//与用户输入无关-------------------------------------------------------主体函数
void updatewithoutinput()
{
	BlackNum = Statistics_Number(1);                      //黑子数量统计
	WhiteNum = Statistics_Number(-1);                     //白字数量统计

	if (Player_move)                                      //如果玩家已落子
	{
		Com_AI_Work_xy(Class, Order_B_W);                 //对战电脑人工智能AI等级-----AI判断

		Player_move = 0;                                  //重新设置为鼠标玩家未落子
		map[ComAI_xy[0]][ComAI_xy[1]] = Order_B_W;        //放置当前类型的棋子

		Conversion_Chess(ComAI_xy[0], ComAI_xy[1], Order_B_W);//棋子转换
		Order_B_W = -Order_B_W;                           //轮换为玩家执子

		Valid_Clear();                                        //过去有效值清理
		Victory_Or_Defeat();                                  //胜负判断
															  //Valid_Clear();                                      //过去有效值清理
		Valid_Location(Order_B_W);                            //玩家有效可落子点查找
	}
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
	Valid_Location(Order_B_W);           //有效位置可落子点查找
	while (1)                            //无限次无条件循环
	{
		updatewithoutinput();            //与用户输入无关
		updatewithinput();               //与用户输入有关
		show();                          //显示

	}
	BOWCGOMOover();                      //游戏结束---后续处理
	return 0;
}

