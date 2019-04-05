#include<windows.h>
#include<stdio.h>
#include "message.h"
typedef int (*CallbackFunc)(int,int);
typedef struct Point
{
	int x,y;
}Point;
typedef struct Listener
{
	struct Point p1,p2;
	CallbackFunc func;
}Listener;
extern COORD waitForMouseClick();
extern void registerListener(Point p1,Point p2,CallbackFunc p);
extern Listener newListener(Point _p1,Point _p2,CallbackFunc p);
extern Point newPoint(int x,int y);
extern void pos(int x,int y);
extern int Trigger(int nx,int ny);
extern void hideCursor();
extern void showCursor();
Listener listener[101];
int listenercnt=0;
Listener newListener(Point _p1,Point _p2,CallbackFunc p){
	Listener listener;
	if(_p1.x>_p2.x){
		int t;
		t=_p1.x;
		_p1.x=_p2.x;
		_p2.x=t;
		t=_p1.y;
		_p1.y=_p2.y;
		_p2.y=t;
	}
	listener.p1=_p1;
	listener.p2=_p2;
	//this->enabled=true;
	listener.func=p;
	return listener;
}
Point newPoint(int x,int y){
	Point p;
	p.x=x;
	p.y=y;
	return p;
}
void registerListener(Point p1,Point p2,CallbackFunc p){
	listener[++listenercnt]=newListener(p1,p2,p);
}
void pos(int x,int y){
	COORD posPoint = {x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),posPoint);
}
int Trigger(int nx,int ny){
	int i;
	for(i=1;i<=listenercnt;i++){
		//printf("%d<=%d %d>=%d %d>=%d %d<=%d\n", listener[i].p1.x,nx,listener[i].p1.y,ny,listener[i].p2.x,nx,listener[i].p2.y,ny);
		if(listener[i].p1.x<=nx && listener[i].p1.y<=ny && listener[i].p2.x>=nx &&listener[i].p2.y>=ny){
			
			return (*listener[i].func)(nx,ny);
		}
	}
	//nothing found!
	//(*callbacks[0])(x,y);
}
COORD waitForMouseClick(){
	//读取鼠标点击数据 直到数据有效才回传
	INPUT_RECORD inRec;
	DWORD res;
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE); 
	
	char title[101];
	
	while(1){
		ReadConsoleInput(hInput, &inRec, 1, &res);
		
		//puts("")
		if (inRec.EventType == MOUSE_EVENT){
			COORD coord= { inRec.Event.MouseEvent.dwMousePosition.X ,inRec.Event.MouseEvent.dwMousePosition.Y};
			if(inRec.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) { 
				return coord;
			}
			sprintf(title, TITLE_GOBANG_COORD_FORMAT
					,coord.X ,coord.Y);
			SetConsoleTitle(title);
			
		}
		

	}
}
void hideCursor(){//隐藏光标
	HANDLE hOut=GetStdHandle(STD_OUTPUT_HANDLE); 
	CONSOLE_CURSOR_INFO cinfo;
	cinfo.bVisible=0;
	cinfo.dwSize=1;
	SetConsoleCursorInfo(hOut,&cinfo);
}
void showCursor(){//显示光标
	HANDLE hOut=GetStdHandle(STD_OUTPUT_HANDLE); 
	CONSOLE_CURSOR_INFO cinfo;
	cinfo.bVisible=1;
	cinfo.dwSize=1;
	SetConsoleCursorInfo(hOut,&cinfo);
}
void clear(){//清屏
	pos(0,0);
	int i;
	for(i=0;i<=30;i++)
		puts("                                                                       ");
}