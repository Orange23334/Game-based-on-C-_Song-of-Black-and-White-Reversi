//Ver5最终版本
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
#define ESC 27                                          //把电脑键盘“Esc”键宏定义为ESC

int map[SIZE][SIZE];                                    //定义“棋盘格地图”--“1”放置黑子--“-1”放置白子--“2”显示黑子可落点--“-2”显示白子可落点--“3”当前黑子落子点--“-3”当前白子落子点
int BlackNum;                                           //黑子数量
int BlackValid;                                         //黑子可落点数量 
int WhiteNum;                                           //白子数量
int WhiteVaild;                                         //白子可落点数量 
int Order_B_W;                                          //当前用户的执子为“黑”---赋值为"1"；“白”---赋值为"-1"----(输入到函数中，作为"Chess_W_B"在函数中起作用)
int Player_move;                                        //鼠标玩家是否落子----“0”玩家未落子----“2”玩家已落子
int Boundary;                                           //界面选择---（0）开始start页面；（-1）难度choice选择页面；（-2）胜利win页面；（-3）故事story页面；（-4）结束over页面；（-5）规则介绍；（1）对战页面
int Class;                                              //难度等级选择-------------------（1）入门级beginner页面；（2）简单easy页面；（3）中等middle页面；（4）困难hard页面
int* ComAI_xy;                                          //指针定义----电脑判断下一步棋返回值--位置坐标
int WINNER;                                             //胜利一方----（-1）电脑胜利；（1）玩家胜利

														//====================================================估值表-----------------------//棋盘各点权值估值
int mappointcount[SIZE][SIZE] =
{
{ 90, -60, 10, 10, 10, 10, -60, 90 },
{ -60, -80, 5, 5, 5, 5, -80, -60 },
{ 10, 5, 1, 1, 1, 1, 5, 10 },
{ 10, 5, 1, 1, 1, 1, 5, 10 },
{ 10, 5, 1, 1, 1, 1, 5, 10 },
{ 10, 5, 1, 1, 1, 1, 5, 10 },
{ -60, -80, 5, 5, 5, 5, -80, -60 },
{ 90, -60, 10, 10, 10, 10, -60, 90 }
};

IMAGE img_bk1_start, img_bk2_choice;                    //背景图片--开始图片|选关图片
IMAGE img_game1_beginner, img_game2_easy, img_game3_middle, img_game4_hard;     //背景图片--四种难度背景图（入门级，简单，中等，困难）
IMAGE img_Regulation;                                   //背景图片--规则介绍
IMAGE img_story1, img_story2;                           //背景图片--背景故事
IMAGE img_win, img_over1, img_over2;                    //背景图片--胜利图片|结束图片

														//初始化---------------------------------------------------------------主体函数
void startup()
{
	mciSendString(_T("open G:\\MusicMATERIAL\\backgroundmusic.mp3 alias bkmusic"), NULL, 0, NULL);
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);  // 循环播放

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

	ComAI_xy = (int*)malloc(2 * sizeof(int));       //指针--分配内存---电脑判断下一步棋返回值--位置坐标
	if (ComAI_xy == NULL)
	{
		printf("内存不足");                           //验证--内存不足时：强行退出
		exit(0);
	}

	//图片导入
	loadimage(&img_bk1_start, _T("G:\\PicturesMATERIAL\\img_bk1_start.jpg"), X, Y);                     //页面初始背景
	loadimage(&img_bk2_choice, _T("G:\\PicturesMATERIAL\\img_bk2_choice.jpg"), X, Y);                   //难度选择背景

	loadimage(&img_game1_beginner, _T("G:\\PicturesMATERIAL\\img_game1_beginner.jpg"), X, Y);           //“入门级”背景
	loadimage(&img_game2_easy, _T("G:\\PicturesMATERIAL\\img_game2_easy.jpg"), X, Y);                   //“简单”背景
	loadimage(&img_game3_middle, _T("G:\\PicturesMATERIAL\\img_game3_middle.jpg"), X, Y);               //“中等”背景
	loadimage(&img_game4_hard, _T("G:\\PicturesMATERIAL\\img_game4_hard.jpg"), X, Y);                   //“困难”背景

	loadimage(&img_Regulation, _T("G:\\PicturesMATERIAL\\img_bk3.jpg"), X, Y);                          //规则介绍背景

	loadimage(&img_story1, _T("G:\\PicturesMATERIAL\\img_story1.jpg"), X, Y);                           //故事背景1
	loadimage(&img_story2, _T("G:\\PicturesMATERIAL\\img_story2.jpg"), X, Y);                           //故事背景2

	loadimage(&img_win, _T("G:\\PicturesMATERIAL\\img_win.jpg"), X, Y);                                 //“胜利|失败”背景
	loadimage(&img_over1, _T("G:\\PicturesMATERIAL\\img_over1.jpg"), X, Y);                             //结束背景1
	loadimage(&img_over2, _T("G:\\PicturesMATERIAL\\img_over2.jpg"), X, Y);                             //结束背景2

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
		int Valid_Number_AI2 = Statistics_Number(2 * Chess_W_B);			   //定义当前难度等级下，此时电脑的有效可落子点数;
		int MAX_WORTH = 0, MAX_i = 0, MAX_j = 0;            			       //定义电脑可转换的最有价值棋盘格及其对应下标

		srand((int)time(0));							                	   //随机种子
		int i = rand() % 8, j = rand() % 8;				                	   //i与j取0~7内的任意数

		for (int k = 0; k < Valid_Number_AI2; k++)                      	   //当前有几个有效可落子点，就进行几次循环----进行几次查找比较
		{
			Sleep(20);

			for (; i < SIZE; i++)                                       	   //*********寻找下一个可执行落点
			{
				for (; j < SIZE; j++)
				{
					if (map[i][j] == 2 * Chess_W_B)                     	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
						break;
					Sleep(20);
				}
				if (map[i][j] == 2 * Chess_W_B)                         	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
					break;
				Sleep(20);

				if (i >= 7)                                             	   //数组下标i的循环赋值
					i = -1;
				if (j >= 7)                                             	   //数组下标j的循环赋值
					j = 0;
			}

			if (MAX_WORTH < Statistics_Conversion_Chess(i, j, Chess_W_B))          //如果最大可转换数目小于当前可执行落点可以转换的数目     
			{
				MAX_WORTH = Statistics_Conversion_Chess(i, j, Chess_W_B);           //赋值
				MAX_i = i;
				MAX_j = j;
			}
			Sleep(20);

			if (i == 7)
				i = 0;
			else
				i++;

			if (j == 7)
				j = 0;
			else
				j++;
		}

		ComAI_xy[0] = MAX_i;                                        //i,j,分别赋值给ComAI_xy---传递指针，作为电脑落子的坐标
		ComAI_xy[1] = MAX_j;
	}

	if (cl == 3)                                                               //输入等级Class为3时，电脑中等AI_middle程序智商
	{
		int Valid_Number_AI3 = Statistics_Number(2 * Chess_W_B);			   //定义当前难度等级下，此时电脑的有效可落子点数;
		int MAX_WORTH = 0, MAX_i = 0, MAX_j = 0;            			       //定义电脑可转换的最有价值棋盘格及其对应下标

		srand((int)time(0));							                	   //随机种子
		int i = rand() % 8, j = rand() % 8;				                	   //i与j取0~7内的任意数

		for (int k = 1; k < Valid_Number_AI3; k++)                      	   //当前有几个有效可落子点，就进行几次循环----进行几次查找比较
		{
			Sleep(20);

			for (; i < SIZE; i++)                                       	   //*********寻找下一个可执行落点
			{
				for (; j < SIZE; j++)
				{
					if (map[i][j] == 2 * Chess_W_B)                     	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
						break;
					Sleep(20);
				}
				if (map[i][j] == 2 * Chess_W_B)                         	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
					break;
				Sleep(20);

				if (i >= 7)                                             	   //数组下标i的循环赋值
					i = -1;
				if (j >= 7)                                             	   //数组下标j的循环赋值
					j = 0;
			}

			if (MAX_WORTH < 2 * Statistics_Conversion_Chess(i, j, Chess_W_B) + mappointcount[i][j])          //如果最大可转换数目小于当前可执行落点可以转换的数目     
			{
				MAX_WORTH = 2 * Statistics_Conversion_Chess(i, j, Chess_W_B) + mappointcount[i][j];           //赋值
				MAX_i = i;
				MAX_j = j;
			}
			Sleep(20);

			if (i == 7)
				i = 0;
			else
				i++;

			if (j == 7)
				j = 0;
			else
				j++;
		}

		ComAI_xy[0] = MAX_i;                                        //i,j,分别赋值给ComAI_xy---传递指针，作为电脑落子的坐标
		ComAI_xy[1] = MAX_j;
	}

	if (cl == 4)                                         //输入等级Class为4时，电脑困难AI_hard程序智商
	{
		int map_copy[SIZE][SIZE];

		for (int i = 0; i < SIZE; i++)
			for (int j = 0; j < SIZE; j++)
				map_copy[i][j] = map[i][j];


		int Valid_Number_AI4 = Statistics_Number(2 * Chess_W_B);			   //定义当前难度等级下，此时电脑的有效可落子点数;
		int Valid_Number_AI4_NEXT;
		int MAX_WORTH = 0, MAX_i = 0, MAX_j = 0;            			       //定义电脑可转换的最有价值棋盘格及其对应下标
		int MAX_WORTH_NEXT, MAX_i_NEXT, MAX_j_NEXT;

		int MAX_WORTH_RESERVE = 0;

		srand((int)time(0));							                	   //随机种子
		int i = rand() % 8, j = rand() % 8;				                	   //i与j取0~7内的任意数
		int i_next, j_next;                         				                   //i_next与j_next取0~7内的任意数

		for (int k = 0; k < Valid_Number_AI4; k++)                      	   //当前有几个有效可落子点，就进行几次循环----进行几次查找比较
		{
			Sleep(2);

			for (; i < SIZE; i++)                                       	   //*********寻找下一个可执行落点
			{
				for (; j < SIZE; j++)
				{
					if (map[i][j] == 2 * Chess_W_B)                     	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
						break;
					Sleep(2);
				}
				if (map[i][j] == 2 * Chess_W_B)                         	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
					break;
				Sleep(2);

				if (i >= 7)                                             	   //数组下标i的循环赋值
					i = -1;
				if (j >= 7)                                             	   //数组下标j的循环赋值
					j = 0;
			}

			MAX_WORTH_RESERVE += Statistics_Conversion_Chess(i, j, Chess_W_B) * 2 + mappointcount[i][j];

			map[i][j] = Chess_W_B;
			Conversion_Chess(i, j, Chess_W_B);
			Valid_Clear();
			Valid_Location(-Chess_W_B);


			Valid_Number_AI4_NEXT = Statistics_Number(-2 * Chess_W_B);			   //定义当前难度等级下，此时电脑的有效可落子点数;

			srand((int)time(0));							                	   //随机种子
			i_next = rand() % 8, j_next = rand() % 8;				                	   //i_next与j_next取0~7内的任意数
			MAX_WORTH_NEXT = 0, MAX_i_NEXT = 0, MAX_j_NEXT = 0;

			for (int k_next = 0; k_next < Valid_Number_AI4_NEXT; k_next++)                      	   //当前有几个有效可落子点，就进行几次循环----进行几次查找比较
			{
				Sleep(2);

				for (; i_next < SIZE; i_next++)                                       	   //*********寻找下一个可执行落点
				{
					for (; j_next < SIZE; j_next++)
					{
						if (map[i_next][j_next] == -2 * Chess_W_B)                     	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
							break;
						Sleep(2);
					}
					if (map[i_next][j_next] == -2 * Chess_W_B)                         	   //如果(i,j)为可落子点(2*Chess_W_B)---持续跳出
						break;
					Sleep(2);

					if (i_next >= 7)                                             	   //数组下标i的循环赋值
						i_next = -1;
					if (j_next >= 7)                                             	   //数组下标j的循环赋值
						j_next = 0;
				}

				if (MAX_WORTH_NEXT < Statistics_Conversion_Chess(i_next, j_next, -Chess_W_B))          //如果最大可转换数目小于当前可执行落点可以转换的数目     
				{
					MAX_WORTH_NEXT = Statistics_Conversion_Chess(i_next, j_next, -Chess_W_B);           //赋值
					MAX_i_NEXT = i_next;
					MAX_j_NEXT = j_next;
				}
				Sleep(2);

				if (i_next == 7)
					i_next = 0;
				else
					i_next++;

				if (j_next == 7)
					j_next = 0;
				else
					j_next++;
			}

			MAX_WORTH_RESERVE -= MAX_WORTH_NEXT * 2;

			map[i_next][j_next] = -Chess_W_B;
			Conversion_Chess(i_next, j_next, -Chess_W_B);

			MAX_WORTH_RESERVE += Statistics_Valid_Location(Chess_W_B) * 2;


			if (MAX_WORTH < MAX_WORTH_RESERVE)          //如果最大可转换数目小于当前可执行落点可以转换的数目     
			{
				MAX_WORTH = MAX_WORTH_RESERVE;           //赋值
				MAX_i = i;
				MAX_j = j;
			}
			Sleep(2);

			if (i == 7)
				i = 0;
			else
				i++;

			if (j == 7)
				j = 0;
			else
				j++;


			for (int i = 0; i < SIZE; i++)
				for (int j = 0; j < SIZE; j++)
					map[i][j] = map_copy[i][j];
		}

		ComAI_xy[0] = MAX_i;                                        //i,j,分别赋值给ComAI_xy---传递指针，作为电脑落子的坐标
		ComAI_xy[1] = MAX_j;
	}

}

//开始Start菜单页面显示函数-------（0）----边缘函数！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
void Start()
{
	putimage(0, 0, &img_bk1_start);                                                 //放置**开始**背景图片

	setlinecolor(BLACK);                                                            //设置按键方块边线颜色
	setfillcolor(RGB(200, 200, 200));                                               //设置案件方块颜色 
	for (int i = 0; i <= 3; i++)
		fillrectangle(PX * 1.5, PY * 1.2 + PY * i * 1.5, PX * 3, PY * 2 + PY * i * 1.5);//放置**选择点击项**方块

	setbkmode(TRANSPARENT);                                                         //设置文字背景色为“透明”
	settextcolor(RGB(155, 120, 120));	                                            //设置文字颜色
																					//settextstyle(42,32,_T("隶书"), 0, 0, FW_BOLD, true,false, true);     //设置文字字体
	settextstyle(42, 32, _T("隶书"), 0, 0, FW_BLACK, false, false, false);

	TCHAR s1_regulation[] = _T("游戏规则"), s2_fight[] = _T("人机对战"), s3_story[] = _T("背景故事"), s4_exit[] = _T("结束语");//字符串定义

	outtextxy(PX * 1.8, PY * 1.4, s1_regulation);                                       //分别输出字体字符
	outtextxy(PX * 1.8, PY * 1.4 + PY * 1.5, s2_fight);
	outtextxy(PX * 1.8, PY * 1.4 + PY * 2 * 1.5, s3_story);
	outtextxy(PX * 1.9, PY * 1.4 + PY * 3 * 1.5, s4_exit);
}

//选择Choice页面显示函数---------（-1）---边缘函数
void Choice()
{
	putimage(0, 0, &img_bk2_choice);                                               //放置**选择难度等级**背景图片

	setlinecolor(WHITE);                                                           //设置边框颜色为白色
	setfillcolor(RGB(180, 180, 180));                                              //设置填充颜色为亮灰色
	for (int i = 0; i <= 3; i++)
		fillrectangle(PX * 1.3, PY * 4.2 + PY * i * 1.5, PX * 2.8, PY * 4.8 + PY * i * 1.5);//放置**选择点击项**方块

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(255, 255, 0));                                                //设置文字颜色
	settextstyle(42, 0, _T("李旭科书法 v1.4"), 0, 0, FW_NORMAL, true, true, false); //设置文字字体格式

	TCHAR s_beginner[] = _T("入门级"), s_easy[] = _T("简单模式"), s_middle[] = _T("中等模式"), s_hard[] = _T("困难模式");//定义字符

	outtextxy(PX * 1.85, PY * 4.2, s_beginner);                                    //输出字符
	outtextxy(PX * 1.8, PY * 4.2 + PY * 1.5, s_easy);
	outtextxy(PX * 1.8, PY * 4.2 + PY * 3, s_middle);
	outtextxy(PX * 1.8, PY * 4.2 + PY * 4.5, s_hard);
}

//胜利Win页面显示函数------------（-2）---边缘函数
void Win()
{
	putimage(0, 0, &img_win);                                                      //放置**获胜**背景图片

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(120, 10, 10));                                                //设置文字颜色
	settextstyle(88, 0, _T("隶书"), 66, 66, 0, true, false, true);//设置文字格式

	TCHAR s_loser1[] = _T("很遗憾，您输了！"), s_loser2[] = _T("再接再厉哦！"), s_winner1[] = _T("棋艺高超，佩服！佩服！");//定义字符

	if (WINNER)                                                                    //如果玩家赢得比赛
	{
		outtextxy(PX, PY, s_winner1);                                              //输出字符
		outtextxy(PX * 2, PY * 1.5, s_winner1);
		outtextxy(PX * 3.6, PY * 2, s_winner1);
	}
	else                                                                           //如果电脑赢得比赛
	{
		outtextxy(PX, PY * 3.5, s_loser1);                                         //输出字符
		outtextxy(PX * 2, PY * 6, s_loser2);
	}
}

//故事Story页面显示函数----------（-3）---边缘函数
void Story()
{
	putimage(0, 0, &img_story1);                                                   //放置**故事**背景图片1号
	putimage(0, 0, &img_story2);                                                   //放置**故事**背景图片2号

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(155, 120, 120));                                                       //设置文字颜色
	settextstyle(28, 0, _T("华文行楷"), 0, 0, FW_HEAVY, true, false, false);  //设置文字格式

	TCHAR s_story1[] = _T("黑白谁能用入玄，千回生死体方圆。");
	TCHAR s_story2[] = _T("空门说得恒沙劫，应笑终年为一先。");                                                                 //断行
	TCHAR s_story3[] = _T("闲对弈楸倾一壶，黄羊枰上几成都。");
	TCHAR s_story4[] = _T("他时谒帝铜池晓，便赌宣城太守无。");
	TCHAR s_story5[] = _T("宫棋布局不依经，黑白分明子数停。");
	TCHAR s_story6[] = _T("巡拾玉沙天汉晓，犹残织女两三星。");
	TCHAR s_story7[] = _T("  ");
	TCHAR s_story8[] = _T("何处仙翁爱手谈。时闻剥啄竹林间。");
	TCHAR s_story9[] = _T("一枰子玉敲云碎，几度午窗惊梦残。");				                //断行
	TCHAR s_story10[] = _T("缓着应知心路远，急围不放耳根闲。");
	TCHAR s_story11[] = _T("烂柯人去收残局。寂寂认亭石几寒。");
	TCHAR s_story12[] = _T("  ");
	TCHAR s_story13[] = _T("寂寞枯秤响泬寥，案淮秋老咽寒潮。");
	TCHAR s_story14[] = _T("白头灯影凉宵里，一局残横见六朝。");                                                                            //断行
	TCHAR s_story15[] = _T("七雄虎斗着难下，三季蚖蟠石不磨。");
	TCHAR s_story16[] = _T("蛮触纷纷无了日，青身白水弄鐮柯。");
	TCHAR s_story17[] = _T("  ");
	TCHAR s_story18[] = _T("弈边忘日月，况复遇神仙。");
	TCHAR s_story19[] = _T("石上无多著，人间几百年。");
	TCHAR s_story20[] = _T("指枰如料敌，落子欲争先。");
	TCHAR s_story21[] = _T("想尔腰柯烂，回头亦骇然。");                               //断行
	TCHAR s_story22[] = _T("  ");
	TCHAR s_story23[] = _T("弈仙何处石枰空，细细松阴婉婉风。");
	TCHAR s_story24[] = _T("岂为商山难固蒂，共呼风雨上飞龙。");
	

	outtextxy(PX / 10, PY * 0.5, s_story1);                          //输出字符串
	outtextxy(PX / 10, PY * 0.8, s_story2);
	outtextxy(PX / 10, PY * 1.3, s_story3);
	outtextxy(PX / 10, PY * 1.6, s_story4);
	outtextxy(PX / 10, PY * 1.9, s_story5);
	outtextxy(PX / 10, PY * 2.2, s_story6);
	outtextxy(PX / 10, PY * 2.5, s_story7);
	outtextxy(PX / 10, PY * 2.8, s_story8);
	outtextxy(PX / 10, PY * 3.3, s_story9);
	outtextxy(PX / 10, PY * 3.6, s_story10);
	outtextxy(PX / 10, PY * 3.9, s_story11);
	outtextxy(PX / 10, PY * 4.2, s_story12);
	outtextxy(PX / 10, PY * 4.5, s_story13);
	outtextxy(PX / 10, PY * 5.0, s_story14);
	outtextxy(PX / 10, PY * 5.3, s_story15);
	outtextxy(PX / 10, PY * 5.6, s_story16);
	outtextxy(PX / 10, PY * 5.9, s_story17);
	outtextxy(PX / 10, PY * 6.2, s_story18);
	outtextxy(PX / 10, PY * 6.5, s_story19);
	outtextxy(PX / 10, PY * 6.8, s_story20);
	outtextxy(PX / 10, PY * 7.1, s_story21);
	outtextxy(PX / 10, PY * 7.6, s_story22);
	outtextxy(PX / 10, PY * 7.9, s_story23);
	outtextxy(PX / 10, PY * 8.2, s_story24);

}

//结束Over页面显示函数-----------（-4）---边缘函数
void Over()
{
	putimage(0, 0, &img_over2);                                                    //放置**结束**背景图片

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(80, 0, 130));                                                 //设置文字颜色
	settextstyle(60, 0, _T("华文行楷"), 0, 0, FW_EXTRABOLD, false, true, false);    //设置文字格式

	TCHAR s_bye1[] = __T("北风吹人不可出，清坐且可与君棋。");                    //定义字符 
	TCHAR s_bye2[] = _T("明朝投局日未晚，从此亦复不吟诗。");
	TCHAR s_bye3[] = _T("棋局深远，后会有期！");
	TCHAR s_bye4[] = _T("请双击‘ESC’退出！");

	outtextxy(PX / 3, PY * 2.5, s_bye1);                                           //输出字符
	outtextxy(PX / 3, PY * 3.5, s_bye2);
	outtextxy(PX, PY * 5, s_bye3);

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(50, 0, 230));                                                //设置文字颜色
	settextstyle(30, 0, _T("隶书"), 0, 0, FW_LIGHT, false, false, false);       //设置文字格式
	outtextxy(PX * 0.35, Y * 0.9, s_bye4);
}

//规则介绍Regulation页面显示函数-（-5）---边缘函数
void Regulation()
{
	putimage(0, 0, &img_Regulation);                                               //放置**规则**背景图片

	setbkmode(TRANSPARENT);                                                        //设置文字背景为透明
	settextcolor(RGB(50, 50, 199));                                              //设置文字颜色
	settextstyle(30, 0, _T("华文新魏"), 0, 0, FW_EXTRABOLD, false, true, false);    //设置文字格式

	TCHAR s_row1[] = _T("仙界一日内，人间千载穷。双棋未遍局，万物皆为空。");      //分行设置字符----讲述规则
	TCHAR s_row2[] = _T("以下为黑白棋局规则：");
	TCHAR s_row3[] = _T("一、操作流程");
	TCHAR s_row4[] = _T("黑白双方轮流下棋，直到游戏结束");
	TCHAR s_row5[] = _T("当落下的棋子在横、竖、斜八个方向内有一个相同颜色棋子时");
	TCHAR s_row6[] = _T("被夹在中间的对方棋子全部翻转会成为落子棋子");
	TCHAR s_row7[] = _T("夹住的位置上必须全部是对手的棋子，不能有空格");
	TCHAR s_row8[] = _T("有在可以翻转棋子的地方才可以下子");
	TCHAR s_row9[] = _T("二、胜负判断：");
	TCHAR s_row10[] = _T("条件一：64宫格棋盘未下满时，无子可下的一方判为输，另一方获胜");
	TCHAR s_row11[] = _T("条件二：64宫格棋盘下满时，剩余棋子数量多的一方获胜，另一方输");
	TCHAR s_row12[] = _T("黑白胜负无已时，目送孤鸿出云外。我们棋局相见！");

	outtextxy(PX / 3, PY, s_row1);                                                 //分行输出字符            
	outtextxy(PX / 3, PY * 1.5, s_row2);
	outtextxy(PX / 3, PY * 2.5, s_row3);
	outtextxy(PX / 3, PY * 3, s_row4);
	outtextxy(PX / 3, PY * 3.5, s_row5);
	outtextxy(PX / 3, PY * 4, s_row6);
	outtextxy(PX / 3, PY * 4.5, s_row7);
	outtextxy(PX / 3, PY * 5, s_row8);
	outtextxy(PX / 3, PY * 6, s_row9);
	outtextxy(PX / 3, PY * 6.5, s_row10);
	outtextxy(PX / 3, PY * 7, s_row11);
	outtextxy(PX / 3, PY * 8, s_row12);
}

//战斗Fight页面显示函数------------1-----边缘函数
//输入：当前执子类型：Order_W_B->Chess_W_B
//功能：实现对战时的页面显示
//输出：无
void FIGHT(int Chess_W_B)
{
	if (Class == 1)
		putimage(0, 0, &img_game1_beginner);                 //放置**新手村**背景图片
	else if (Class == 2)
		putimage(0, 0, &img_game2_easy);                     //放置**简单**背景图片
	else if (Class == 3)
		putimage(0, 0, &img_game3_middle);                   //放置**中等**背景图片
	else if (Class == 4)
		putimage(0, 0, &img_game4_hard);                     //放置**困难**背景图片

															 //显示文字“黑子个数为：”“白子个数为：”
	setbkmode(TRANSPARENT);                                                     //设置文字背景为透明
	settextcolor(BLACK);                                                        //设置文字颜色
	settextstyle(40, 0, _T("华文行楷"), 0, 0, FW_BOLD, true, true, false);       //设置文字（“高”：40）|（“字体”：华文行楷）|（“加粗”：FW_BOLD）|（“斜体”：有）|（“下划线”：有）|（“删除线”：无）
	TCHAR s_BlackNum[] = _T("黑子的数量为：");                                   //定义字符串
	TCHAR s_WhiteNum[] = _T("白字的数量为：");
	outtextxy(PX / 14, PY, s_BlackNum);                                         //于指定位置-（PX/14,PY)-输出字符串
	outtextxy(PX / 14, PY * 2.5, s_WhiteNum);                                   //于指定位置-（PX/14,PY*2.5）

																				//显示棋子数量
	setbkmode(TRANSPARENT);                                                     //设置数字背景为透明  
	settextcolor(BLUE);                                                         //设置数字颜色
	settextstyle(40, 0, _T("幼圆"), 0, 0, FW_HEAVY, false, true, false);        //设置文字（“高”：40）|（“字体”：幼圆）|（“加粗”：FW_HEAVY）|（“斜体”：无）|（“下划线”：有）|（“删除线”：无）
	TCHAR s_figure_BlackNum[3];                                                 //定义字符串
	TCHAR s_figure_WhiteNum[3];
	swprintf_s(s_figure_BlackNum, _T("%d"), BlackNum);                          //把数字（BlackNum||WhiteNum）赋值给字符串
	swprintf_s(s_figure_WhiteNum, _T("%d"), WhiteNum);
	outtextxy(PX / 4, PY * 1.5, s_figure_BlackNum);                               //于指定位置-（PX/14,PY)-输出字符串                              
	outtextxy(PX / 4, PY * 3, s_figure_WhiteNum);                               //于指定位置-（PX/14,PY*2.5）

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
			else if (map[i][j] == 2 * Chess_W_B)                               //如果棋盘格为（2*Chess_W_B）---放置红圈
			{
				setlinecolor(RED);                                             //红边空心圆绘制--显示“（Chess_W_B）子”可落点
				circle(PX + BBLACK / 2 + i * BBLACK, PY + BBLACK / 2 + j * BBLACK, (int)(CHESS_SIZE * 0.28));
			}
		}
	}
}

//显示----------------------------------------------------------------主体函数
void show()
{
	if (Boundary == 0)                                    //（0）开始start页面
		Start();
	else if (Boundary == -1)                              //（ - 1）难度choice选择页面
		Choice();
	else if (Boundary == -2)                              //（-2）胜利win页面
		Win();
	else if (Boundary == -3)                              //（ - 3）故事story页面
		Story();
	else if (Boundary == -4)                              //（-4）结束over页面
		Over();
	else if (Boundary == -5)                              //（-5）规则介绍Regulation页面
		Regulation();
	else if (Boundary == 1)                               //（1）对战FIGHT页面
		FIGHT(Order_B_W);                                 //Order_B_W棋子下棋

	setlinecolor(RGB(150, 150, 0));                       //设置边框颜色
	setfillcolor(RGB(50, 155, 200));                      //设置填充颜色
	fillrectangle(PX * 3.3, PY * 8.2, PX * 4, PY * 9);        //在指定位置（PX*3.3, PY*8.2）放置指定大小与颜色的方框

	setbkmode(TRANSPARENT);                               //设置文字背景为透明
	settextcolor(RGB(200, 200, 255));                     //设置文字颜色
	settextstyle(20, 0, _T("华文行楷"), 0, 0, FW_EXTRALIGHT, true, true, false);//设置文字字体格式
	TCHAR s_EXIT[] = _T("返回主页面");
	outtextxy(PX * 3.3 + PX * 0.18, PY * 8.2 + PY * 0.2, s_EXIT);

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

			//开始页面时（（（0）））***************************************
			if (Boundary == 0)
			{
				if (m.x >= PX * 1.5 && m.x <= PX * 3 && m.y >= PY * 1.2 && m.y <= PY * 2)                                //点击进入规则接介绍页面
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = -5;
				}
				if (m.x >= PX * 1.5 && m.x <= PX * 3 && m.y >= PY * 1.2 + PY * 1.5 && m.y <= PY * 2 + PY * 1.5)          //点击进入战斗等级选择页面
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = -1;
				}
				if (m.x >= PX * 1.5 && m.x <= PX * 3 && m.y >= PY * 1.2 + PY * 2 * 1.5 && m.y <= PY * 2 + PY * 2 * 1.5)  //点击进入故事页面
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = -3;
				}
				if (m.x >= PX * 1.5 && m.x <= PX * 3 && m.y >= PY * 1.2 + PY * 3 * 1.5 && m.y <= PY * 2 + PY * 3 * 1.5)  //点击进入结束页面
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = -4;
				}
			}

			//选择难度（（（-1）））****************************************
			if (Boundary == -1)
			{
				if (m.x >= PX * 1.3 && m.x <= PX * 2.8 && m.y >= PY * 4.2 && m.y <= PY * 4.8)                            //从难度选择页面-->>对战模式--新手村关卡
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = 1;                    //界面跳转
					Class = 1;                       //难度等级----beginner

					for (int i = 0; i < SIZE; i++)                   //8*8--64宫格循环查找||所有棋盘格初始化为0
						for (int j = 0; j < SIZE; j++)
							map[i][j] = 0;
					map[3][3] = 1;                                   //中间4个棋盘格棋子初始化
					map[4][4] = 1;
					map[3][4] = -1;
					map[4][3] = -1;
				}
				if (m.x >= PX * 1.3 && m.x <= PX * 2.8 && m.y >= PY * 4.2 + PY * 1.5 && m.y <= PY * 4.8 + PY * 1.5)      //从难度选择页面-->>对战模式--简单关卡
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = 1;                    //界面跳转                    
					Class = 2;                       //难度等级----easy

					for (int i = 0; i < SIZE; i++)                   //8*8--64宫格循环查找||所有棋盘格初始化为0
						for (int j = 0; j < SIZE; j++)
							map[i][j] = 0;
					map[3][3] = 1;                                   //中间4个棋盘格棋子初始化
					map[4][4] = 1;
					map[3][4] = -1;
					map[4][3] = -1;
				}
				if (m.x >= PX * 1.3 && m.x <= PX * 2.8 && m.y >= PY * 4.2 + PY * 3 && m.y <= PY * 4.8 + PY * 3)          //从难度选择页面-->>对战模式--中等关卡
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = 1;                    //界面跳转
					Class = 3;                       //难度等级----middle

					for (int i = 0; i < SIZE; i++)                   //8*8--64宫格循环查找||所有棋盘格初始化为0
						for (int j = 0; j < SIZE; j++)
							map[i][j] = 0;
					map[3][3] = 1;                                   //中间4个棋盘格棋子初始化
					map[4][4] = 1;
					map[3][4] = -1;
					map[4][3] = -1;
				}
				if (m.x >= PX * 1.3 && m.x <= PX * 2.8 && m.y >= PY * 4.2 + PY * 4.5 && m.y <= PY * 4.8 + PY * 4.5)       //从难度选择页面-->>对战模式--困难关卡
				{
					mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
					mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
					mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

					Boundary = 1;                    //界面跳转
					Class = 4;                       //难度等级----hard

					for (int i = 0; i < SIZE; i++)                   //8*8--64宫格循环查找||所有棋盘格初始化为0
						for (int j = 0; j < SIZE; j++)
							map[i][j] = 0;
					map[3][3] = 1;                                   //中间4个棋盘格棋子初始化
					map[4][4] = 1;
					map[3][4] = -1;
					map[4][3] = -1;
				}
			}

			//比赛结束--胜利（（（-2）））
			//我的故事（（（-3）））
			//结束页面（（（-4）））
			//规则介绍（（（-5）））

			//战斗页面（（（1）））****************************************
			if (Boundary == 1)
			{
				for (int i = 0; i < SIZE; i++)                                      //64宫格循环查找
				{
					for (int j = 0; j < SIZE; j++)
					{
						if (abs(m.x - (PX + BBLACK / 2 + i * BBLACK)) < BBLACK / 2 && abs(m.y - (PY + BBLACK / 2 + j * BBLACK)) < BBLACK / 2)        //范围确认
						{
							mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
							mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
							mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

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

			//返回开始菜单*****************************************************
			if (m.x >= PX * 3.3 && m.x <= PX * 4 && m.y >= PY * 8.2 && PY * 9)
			{
				mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
				mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
				mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

				Boundary = 0;
			}

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
void GAMEover()
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

		if (_kbhit())                                                               //感应键盘输入
		{
			input = _getch();                                                       //把键盘输入值付给input
			if (input == ESC)                                                       //如果键盘输入“Esc”键
			{
				mciSendString(_T("close ckmusic"), NULL, 0, NULL); // 先把前面一次的音乐关闭
				mciSendString(_T("open G:\\MusicMATERIAL\\click.mp3 alias ckmusic"), NULL, 0, NULL); // 打开跳动音乐
				mciSendString(_T("play ckmusic"), NULL, 0, NULL); // 仅播放一次

				break;
			}
		}
	}
	GAMEover();                      //游戏结束---后续处理
	return 0;
}