
#include<conio.h>

#include<unistd.h>
#include<string.h>
#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include "internet.h"

void launchLocalMode();
void internetModeMenu();
void initListener();
void renderBotton();
void hideCursor();
int onUndoClick(int x,int y);
int onSurrenderClick(int x,int y);
int mainMenu();
void clear();

int onUndoClick(int x,int y){
	if(internetMode==GOBANG_LOCAL_MODE) return onLocalUndoClick(x, y);
	return UNDO_CLICK_SIGNAL;
	
}
int onSurrenderClick(int x,int y){
	if(internetMode==GOBANG_LOCAL_MODE) return onLocalSurrenderClick(x, y);
	return SURRENDER_CLICK_SIGNAL;
}
void initListener(){//注册游戏按钮监听器
	registerListener(newPoint(1,1),newPoint(21,21),play);
	registerListener(newPoint(25,20),newPoint(30,20),onUndoClick);
	registerListener(newPoint(35,20),newPoint(45,20),onSurrenderClick);
}
void renderBotton(){
	pos(25,20);
	puts("[Undo]");
	pos(35,20);
	puts("[Surrender]");
}

void internetModeMenu(){
	clear();
	pos(0,0);
	puts("Internet Mode Configure");
	puts("[1] Client Mode");
	puts("    Connect a existing GOBANG server.");
	puts("[2] Server Mode");
	puts("    Listening on a PORT and wait client to connect.");
	int op;
	scanf("%d",&op);
	getchar();
	if(op==1){
		internetMode=GOBANG_CLIENT_MODE;
		clear();
		pos(0,0);
		puts(MESSAGE_ENTER_IPADDR);
		char ip[50];
		scanf("%s",&ip);
		getchar();
		internetClientMode(ip);
		
	}else if(op==2){
		internetMode=GOBANG_SERVER_MODE;
		clear();
		pos(0,0);
		internetServerMode();
	}else {
		puts(MESSAGE_UNKNOWN_OPTION);
		return;
	}

}
int mainMenu(){
	clear();
	pos(0,0);
	puts("Welcome to GOBANG!\n[1] Local Multiple Player\n[2] Internet Multiple Player\n[3] Exit\n");
	showCursor();
	int op;
	scanf("%d",&op);
	pos(0,4);
	switch(op){
		case 1:
			launchLocalMode();
			break;
		case 2:
			internetModeMenu();
			break;
		case 3:
			exit(0);
			break;
		default:
			puts(MESSAGE_UNKNOWN_OPTION);
		
	}
}



void launchLocalMode(){
	internetMode=GOBANG_LOCAL_MODE;
	//本地游戏
	clear();
	//注册本地游戏按钮监听器
	initListener();
	resetBoard();
	SetConsoleTitle(TITLE_GOBANG);
	hideCursor();
	renderBotton();
	pos(25,0);
	puts(BANNER_GOBANG);

	
	int ret;
	int click=0;
	while (1) {
		pos(25,1);
		puts(isBlackMove?MESSAGE_WAIT_BLACK:MESSAGE_WAIT_WHITE);
		COORD coord=waitForMouseClick();
		int ret=Trigger(coord.X,coord.Y);
		if(ret==GAMEOVER_SIGNAL) break;
	}
	MessageBox(NULL, TEXT(TITLE_GAMEOVER), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
}
int main(){
	while(1){
		//主菜单
		SetConsoleTitle(TITLE_GOBANG);
		mainMenu();
	}	
	return 0;
}