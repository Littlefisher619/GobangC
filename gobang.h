#pragma once
#include<stdio.h>
#include<string.h>
#include<windows.h>
#include "message.h"
#define SIZE 20 

int board[SIZE][SIZE];//0-* 1-O
int history[400][3]={0},num=0;//record whlch step who go while one
int isBlackMove=1;//if it is the time for black one to play, flag=0, or flag=1
int isBlackTheWinner=0;
int isBlackIdentity=0;
int gobangGameMode;
extern void draw(); //绘制棋盘  
extern int pending_win (int x ,int y); //judge whether someone can win
extern int pending_legal(int x,int y); //judging whether put the chess here is legal
extern void insert(int x,int y); //put the chess into the board
extern void output(int x,int y);
extern void back(int history [][3],int num);//悔棋 
extern void record (int x,int y,int num);
extern void resetBoard(void);
extern int play(int x,int y);
extern int onLocalUndoClick(int y,int x);
extern int onLocalSurrenderClick(int x,int y);
extern int isLegalMove(int y,int x);
extern void setIsBlackMove(int x);
void flipIsBlackMove();
void setIsBlackMove(int x);
void pos(int x,int y);
void setIsBlackMove(int x){
	isBlackMove=x;
}
void flipIsBlackMove(){
	isBlackMove=1-isBlackMove;
}
int isLegalMove(int y,int x){
	return (board[x][y]==-1);
}
int onLocalSurrenderClick(int x,int y){
	pos(25,2);
	if(isBlackMove==1) puts(MESSAGE_WHITE_WIN);
	else puts(MESSAGE_BLACK_WIN);
	return 0;
}
void resetBoard(void) //棋盘预处理
{
	int i,j;
	for(i=0;i<SIZE;i++)
		for(j=0;j<SIZE;j++)
			board[i][j]=-1;
	draw();
}
int onLocalUndoClick(int y,int x){
	if(num>=2)
	{
		back(history,--num);
		back(history,--num);
		draw();
	}
	else
	{
		pos(25,2);
		puts(MESSAGE_UNDO_NOTHING);
	}
}
int play(int y,int x)
{

	y--;
	x--;
	if(board[x][y]!=-1){ 
		pos(25,2);
		puts(MESSAGE_ILLEGAL_MOVE);
		return CONTINUE_SIGNAL;
	}
	
	record(x,y,num++);//record the step now 
	insert(x,y); //put the chess into the board
	
	draw();
	if (pending_win(x,y))
	{
		pos(25,2);
		if(isBlackMove==1)isBlackTheWinner=0;
		else isBlackTheWinner=1;
		puts(isBlackTheWinner?MESSAGE_BLACK_WIN:MESSAGE_WHITE_WIN);
		return GAMEOVER_SIGNAL;
	}
	flipIsBlackMove();
	return CONTINUE_SIGNAL;
}

void record (int x,int y,int num)
{
	history[num][0]=x;
	history[num][1]=y;
}
void draw()
{
	pos(0,0);
	int i;
	puts(" ABCDEFGHIJKLMNOPQRST");//绘制棋盘列标格 
	for(i=0; i<SIZE; i++)
	{
		putchar(i+65);
		if (i == 0) // 第1行 
		{
			if (board[i][0] < 0)
				putchar('.');
			else output(i,0);	
			int j;
			for (j = 1; j < SIZE-1; j++)
				if (board[i][j] < 0)
					putchar('.');
				else output(i,j);	
			if (board[i][j] < 0)
				putchar('.');
			else output(i,j);
			putchar('\n');
		}
		
		else if (i>=1 && i<SIZE-1) //第2-19行 
		{
			if (board[i][0] < 0)
				putchar('.');
				else output(i,0);
			int j;
			for (j = 1; j < SIZE-1; j++)
				if (board[i][j] < 0)
					putchar('.');
				else output(i,j);
			if (board[i][j] < 0)
				putchar('.');
			else output(i,j);
			putchar('\n');
		}
		
		else // 第20行 
		{
			if (board[i][0] < 0)
				putchar('.');
			else output(i,0);
			int j;
			for (j = 1; j < SIZE-1; j++)
				if (board[i][j] < 0)
					putchar('.');
				else output(i,j);
			if (board[i][j] < 0)
				putchar('.');
			else output(i,j);
			putchar('\n');
		}
	}
}

void back(int history[][3],int num)
{
	
	int x=history[num][0],y=history[num][1];
	
	pos(25,2);
	//printf("[%c] (%c,%c) Undo OK!             ",(board[x][y]==0)?'*':'0',y+65,x+65);
	puts(MESSAGE_UNDO_OK);
	board[x][y]=-1;
}

void insert(int x,int y)
{
	pos(25,2);
	printf(MESSAGE_LASTMOVE_FORMAT,isBlackMove?'O':'*',y+65,x+65);
	board[x][y]=isBlackMove?1:0;
	
	
}

void output (int x,int y)
{
	putchar(board[x][y]==0?'O':'*');
}


int pending_win (int x ,int y)
{
	int ch=board[x][y];
	if (isBlackMove==0) ch=0;
	else ch=1;
	//竖着五连 
	if(x-4>=0&&board[x-1][y]==ch&&board[x-2][y]==ch&&board[x-3][y]==ch&&board[x-4][y]==ch) return 1;
	if(x-3>=0&&board[x+1][y]==ch&&board[x-1][y]==ch&&board[x-2][y]==ch&&board[x-3][y]==ch&&x+1<SIZE) return 1;
	if(x-2>=0&&board[x+2][y]==ch&&board[x+1][y]==ch&&board[x-1][y]==ch&&board[x-2][y]==ch&&x+2<SIZE) return 1;
	if(x-1>=0&&board[x+3][y]==ch&&board[x+2][y]==ch&&board[x+1][y]==ch&&board[x-1][y]==ch&&x+3<SIZE) return 1;
	if(board[x+4][y]==ch&&board[x+3][y]==ch&&board[x+2][y]==ch&&board[x+1][y]==ch&&x+4<SIZE) return 1;
	//横着五连 
	if(y-4>=0&&board[x][y-1]==ch&&board[x][y-2]==ch&&board[x][y-3]==ch&&board[x][y-4]==ch) return 1;
	if(y-3>=0&&y+1<SIZE&&board[x][y+1]==ch&&board[x][y-1]==ch&&board[x][y-2]==ch&&board[x][y-3]==ch) return 1;
	if(y-2>=0&&y+2<SIZE&&board[x][y+2]==ch&&board[x][y+1]==ch&&board[x][y-1]==ch&&board[x][y-2]==ch) return 1;
	if(y-1>=0&&y+3<SIZE&&board[x][y+3]==ch&&board[x][y+2]==ch&&board[x][y+1]==ch&&board[x][y-1]==ch) return 1;
	if(y+4<SIZE&&board[x][y+4]==ch&&board[x][y+3]==ch&&board[x][y+2]==ch&&board[x][y+1]==ch) return 1;
	//斜连
	if(                  x-4>=0&&y+4<SIZE&&board[x-1][y+1]==ch&&board[x-2][y+2]==ch&&board[x-3][y+3]==ch&&board[x-4][y+4]==ch) return 1;
	if(x-3>=0&&y+3<SIZE&&x+1<SIZE&&y-1>=0&&board[x+1][y-1]==ch&&board[x-1][y+1]==ch&&board[x-2][y+2]==ch&&board[x-3][y+3]==ch) return 1;
	if(x-2>=0&&y+2<SIZE&&x+2<SIZE&&y-2>=0&&board[x+2][y-2]==ch&&board[x+1][y-1]==ch&&board[x-1][y+1]==ch&&board[x-2][y+2]==ch) return 1;
	if(x-1>=0&&y+1<SIZE&&x+3<SIZE&&y-3>=0&&board[x+3][y-3]==ch&&board[x+2][y-2]==ch&&board[x+1][y-1]==ch&&board[x-1][y+1]==ch) return 1;
	if(                  x+4<SIZE&&y-4>=0&&board[x+4][y-4]==ch&&board[x+3][y-3]==ch&&board[x+2][y-2]==ch&&board[x+1][y-1]==ch) return 1;
	
	if(                  x+4<SIZE&&y+4<SIZE&&board[x+1][y+1]==ch&&board[x+2][y+2]==ch&&board[x+3][y+3]==ch&&board[x+4][y+4]==ch) return 1;
	if(x+3<SIZE&&y+3<SIZE&&x-1>=0&&y-1>=0&&board[x-1][y-1]==ch&&board[x+1][y+1]==ch&&board[x+2][y+2]==ch&&board[x+3][y+3]==ch) return 1;
	if(x+2<SIZE&&y+2<SIZE&&x-2>=0&&y-2>=0&&board[x-2][y-2]==ch&&board[x-1][y-1]==ch&&board[x+1][y+1]==ch&&board[x+2][y+2]==ch) return 1;
	if(x+1<SIZE&&y+1<SIZE&&x-3>=0&&y-3>=0&&board[x-3][y-3]==ch&&board[x-2][y-2]==ch&&board[x-1][y-1]==ch&&board[x+1][y+1]==ch) return 1;
	if(                  x-4>=0&&y-4>=0&&board[x-4][y-4]==ch&&board[x-3][y-3]==ch&&board[x-2][y-2]==ch&&board[x-1][y-1]==ch) return 1;
	return 0;
}
