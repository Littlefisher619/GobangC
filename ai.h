#include<windows.h>
#include "gobang.h"
extern void launchAIMode();
extern COORD waitAI();

int score(int x,int y);
int score_board[SIZE][SIZE];
void launchAIMode(){
	gobangGameMode=GOBANG_AI_MODE;
	clear();
	initListener();
	resetBoard();
	SetConsoleTitle(TITLE_GOBANG);
	hideCursor();
	renderBotton();
	pos(25,0);
	puts(BANNER_GOBANG);
	srand(time(0)); 
	isBlackIdentity=random(1);
	pos(25,19);
	puts(isBlackIdentity?MESSAGE_IDENTITY_BLACK:MESSAGE_IDENTITY_WHITE);		
	while (1) {
		pos(25,1);
		COORD coord;
		if(isBlackIdentity^isBlackMove==0){
			puts(MESSAGE_WAIT_YOU);
			coord=waitForMouseClick();
		}else{
			puts(MESSAGE_WAIT_AI);
			coord=waitAI();
			
		}
		int ret=Trigger(coord.X,coord.Y);
		if(ret==GAMEOVER_SIGNAL) break;
	}
	MessageBox(NULL, TEXT( (isBlackTheWinner&&isBlackIdentity) ? MESSAGE_YOU_WIN: MESSAGE_YOU_LOSE ), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
}
COORD waitAI(){
	COORD coord;
	
	int max_score=0; //place for AI to play
	int i,j;
	for (i=0;i<SIZE;i++)
		for (j=0;j<SIZE;j++)
		{
			if(board[i][j]!=-1) continue;
			if ( (score_board[i][j]=score(i,j)) >=max_score)
			{
			//	printf("%d %d %d\n",i,j,score_board[i][j]);
				coord.X=i,coord.Y=j,max_score=score_board[i][j];
				max_score=score_board[i][j];
			}
		}
	
	int temp=coord.X;
	coord.X=coord.Y;
	coord.Y=temp;
	coord.X++;
	coord.Y++;
	return coord;
	
}


int score(int x,int y)
{
	int flag=isBlackMove;
	int i1=x+1,j1=y,i2=x-1,j2=y,ans=0,i3,j3,i4,j4;
	while(board[i1][y]==(1-flag) && i1<SIZE)
	{
		i1++;
	}
	while( (board[i2][y]==(1-flag)) && i2>=0) //敌方棋子 
		i2--;
	//printf("%d %d %d %d\n",x,y,i1,i2);
	i1--; //该方向最远敌方棋子位置 
	i2++;
	int pl=i1-i2;
	
	if(pl>=4) ans+=1000000;
	else if(pl==3) ans+=100000;
	else if(pl==2) ans+=1500;
	else if(pl==1) ans+=500;
	i1=x,j1=y+1,j2=y-1;
	while(board[x][j1]==(1-flag)&&j1<SIZE)
		j1++;
	while(board[x][j2]==(1-flag)&&j2>=0) //敌方棋子
		j2--;
	j1--;
	j2++;
	pl=j1-j2;
	if(pl>=4) ans+=1000000;
	else if(pl==3) ans+=100000;
	else if(pl==2) ans+=1500;
	else if(pl==1) ans+=500;
	i1=x-1,j1=y-1,i2=x+1,j2=y+1;
	while(board[i1][j1]==(1-flag)&& i1>=0 &&j1>=0)
		i1--,j1--;
	while(board[i2][j2]==(1-flag)&& i2<SIZE &&j2<SIZE)
		i2++,j2++;
	i1++,i2--;
	pl=i2-i1;
	if(pl>=4) ans+=1000000;
	else if(pl==3) ans+=100000;
	else if(pl==2) ans+=1500;
	else if(pl==1) ans+=500;
	i1=x-1,j1=y+1,i2=x+1,j2=y-1;
	while(board[i1][j1]==(1-flag)&& i1>=0 &&j1<SIZE)
		i1--,j1++;
	while(board[i2][j2]==(1-flag)&& i2<SIZE &&j2>=0)
		i2++,j2--;
	i1++,i2--;
	pl=i2-i1;
	if(pl>=4) ans+=1000000;
	else if(pl==3) ans+=100000;
	else if(pl==2) ans+=1500;
	else if(pl==1) ans+=500;
	
	//following are the computer attack player position
	i1=x-1,j1=y,i2=x+1,j2=y;
	while(board[i1][y]==flag&&i1>=0)
		i1--;
	i1++;
	while(board[i2][y]==flag&&i2<SIZE)
		i2++;
	i2--;
	pl=i2-i1;
	//judging whether this direction can win
	i3=x-1,i4=x+1;
	while(board[i3][y]!=(1-flag)&&x-i3<=6)
		i3--;
	i3++;
	while(board[i4][y]!=(1-flag)&&i4-x<=6)
		i4++;
	i4--;
	if(i4-i3>=5)
	{
		if(pl==4) ans+=10000000;   //我方冲5 
		else if(pl==3) ans+=10000; //我方冲4 
		else if(pl==2) ans+=5000;  //我方冲3 
		else if(pl==1) ans+=501;
	}
	
	j1=y-1,j2=y+1;
	while(board[x][j1]==flag&&j1>=0)
		j1--;
	j1++;
	while(board[x][j2]==flag&&j2<SIZE)
		j2++;
	j2--;
	j3=y-1,j4=y+1;
	while(board[x][j3]!=(1-flag)&&j3>=0&&y-j3<=6)
		j3--;
	j3++;
	while(board[x][j4]!=(1-flag)&&j4<SIZE&&j4-y<=6)
		j4++;
	j4--;
	pl=j2-j1;
	if(j4-j3>=5)
	{
		if(pl==4) ans+=10000000;
		else if(pl==3) ans+=10000;
		else if(pl==2) ans+=5000;
		else if(pl==1) ans+=501;
	}
	
	i1=x-1,j1=y-1,i2=x+1,j2=y+1;
	while(board[i1][j1]==flag&&i1>=0&&j1>=0)
		i1--,j1--;
	while(board[i2][j2]==flag&&i2<SIZE&&j2<SIZE)
		i2++,j2++;
	i1++,i2--;
	pl=i2-i1;
	i3=x-1,j3=y-1,i4=x+1,j4=y+1;
	while(board[i3][j3]!=(1-flag) &&i3>=0&&j3>=0&&x-i3<=6)
		i3--,j3--;
	i3++;
	while(board[i4][j4]!=(1-flag) &&i4<SIZE&&j4<SIZE&&i4-x<=6)
		i4++,j4++;
	i4--;
	if(i4-i3>=5)
	{
		if(pl==4) ans+=10000000;
		else if(pl==3) ans+=10000;
		else if(pl==2) ans+=5000;
		else if(pl==1) ans+=501;
	}
	
	i1=x-1,j1=y+1,i2=x+1,j2=y-1;
	while(board[i1][j1]==flag&&i1>=0&&j1<SIZE)
		i1--,j1++;
	while(board[i2][j2]==flag&&i2<SIZE&&j2>=0)
		i2++,j2--;
	i1++,i2--;
	pl=i2-i1;
	i3=x-1,j3=y+1,i4=x+1,j4=y-1;
	while(board[i3][j3]!=(1-flag) &&i3>=0&&j3<SIZE)
		i3--,j3++;
	i3++;
	while(board[i4][j4]!=(1-flag) &&i4<SIZE&&j4>=0)
		i4++,j4--;
	i4--;
	if(i4-i3>=5)
	{
		if(pl==4) ans+=10000000;
		else if(pl==3) ans+=100000;
		else if(pl==2) ans+=5000;
		else if(pl==1) ans+=501;
	}
	return ans;
}
