#include "ui.h"
#include "message.h"
#include "gobang.h"
#define random(x) (rand()%x)
#include<sys/time.h>
extern int internetUserOperationHandler(SOCKET s_client);
extern int internetGobangDataHandler(SOCKET s_client);
extern void internetClientMode(char* SERVER_IP);
extern void internetServerMode();
extern void internetGameStart(SOCKET s_client);
void clear();
void initListener();
void renderBotton();
void internetGameInit();
void internetGameInit(){
	//启动游戏
	setIsBlackMove(1);
	clear();
	//注册按钮监听器
	initListener();
	resetBoard();
	
	SetConsoleTitle(TITLE_GOBANG);
	hideCursor();
	//渲染按钮
	renderBotton();
	pos(25,0);
	puts(BANNER_GOBANG);
	pos(25,19);
	//输出自己的身份
	puts(isBlackIdentity?MESSAGE_IDENTITY_BLACK:MESSAGE_IDENTITY_WHITE);		
}
void internetClientMode(char* SERVER_IP){

    WORD ws_version = MAKEWORD(2, 2);   //指定Winsock version
    WSADATA wsaData;                    //WSA 函数的参数

    /*初始化winsock*/
    WSAStartup(ws_version, &wsaData);

	int send_status, recv_status;
	SOCKADDR_IN addr_server;
	SOCKET s_client;
	while(1){
		s_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN addr_server;
        addr_server.sin_family = AF_INET;   //协议
        addr_server.sin_port = htons(5050); //端口
        addr_server.sin_addr.s_addr = inet_addr(SERVER_IP);
		int cnct_status = connect(s_client, (SOCKADDR*)&addr_server, sizeof(SOCKADDR));
	    if(cnct_status == 0)//连接成功
        {
			break;
		}else {
			int result=MessageBox(NULL, TEXT(MESSAGE_FAILED_TO_CONNECT_SERVER), TEXT(TITLE_ERROR), MB_RETRYCANCEL | MB_ICONERROR);
			if(result==IDCANCEL) return;
		}
	}
	
	char buf[100];
	
    while(1)
    {
        
		memset(buf,0,sizeof(buf));

		
		puts(MESSAGE_CONNECTION_ESTABLISHED);
		
		//与服务器建立连接成功，发送READY信号
		send_status = send(s_client, CLIENT_READYSIGNAL, strlen(CLIENT_READYSIGNAL), 0);
		if(send_status == SOCKET_ERROR)//发送失败
		{
			puts(MESSAGE_CONNECTION_TIMEOUT);
		}
		else//发送成功
		{
			//等待服务端传来的身份信息
			recv_status = recv(s_client, buf, 100, 0);
			buf[recv_status] = 0x00;//在字符串结尾填上 结束符 0x00 == /0 参考回答
			
			if(buf[0]=='S'){//信息有效
				
				//读取身份信息
				sscanf(buf,SERVER_START_DATA,&isBlackIdentity);
				//启动游戏
				internetGameInit();
				break;
			}
		}
			
    }
	internetGameStart(s_client);
	closesocket(s_client);
    WSACleanup();
}
void internetServerMode(){
    WORD ws_version = MAKEWORD(2, 2);   //指定Winsock version
    WSADATA wsaData;                    //WSA 函数的参数

    /*初始化winsock*/
    WSAStartup(ws_version, &wsaData);

    /*socket*/
    SOCKET s_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr_server;
    addr_server.sin_family = AF_INET;   //协议
    addr_server.sin_port = htons(5050); //端口
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);    //IP:任意IP

    /*bind*/
    int bind_status;
	bindport:
    bind_status = bind(s_server, (SOCKADDR*)&addr_server, sizeof(SOCKADDR));
    if(bind_status == SOCKET_ERROR)
    {
		int result=MessageBox(NULL, TEXT(MESSAGE_CLOUD_NOT_BIND_TO_PORT), TEXT(TITLE_ERROR), MB_RETRYCANCEL | MB_ICONERROR);
		if(result==IDCANCEL) return;
		else goto bindport;
    }
    else
    {
        puts(MESSAGE_BIND_OK);
    }

    /*listen*/
    listen(s_server, 1);//max=5
	
    printf(MESSAGE_LISTENING);

    SOCKADDR_IN addr_client;            //存储client地址信息
    int len = sizeof(SOCKADDR);
    SOCKET s_client;                    //连接的socket
	

	int send_status, recv_status;
	
	
	while(1){
		puts(MESSAGE_WAIT_CONNECTION);
		/*accept*/
		s_client = accept(s_server, (SOCKADDR*)&addr_client, &len);
		if(s_client == INVALID_SOCKET)
		{
			printf(MESSAGE_ESTABLISHED_CONNECTION_WITH_CLIENT_FAILED);
		}else break;
	}
	
    char buf[100];
    while(1)
    {
		memset(buf,0,sizeof(buf));
		//与客户端建立连接成功，接受客户端READY信号
		puts(MESSAGE_CONNECTION_ESTABLISHED);
		recv_status = recv(s_client, buf, 100, 0);
		if(recv_status > 0)
		{
			buf[recv_status] = 0x00;//在字符串结尾填上 结束符 0x00 == /0 参考回答
			if(strcmp(buf,CLIENT_READYSIGNAL)==0){
				//信号有效
				
				puts(MESSAGE_STARTING_GAME);
				
				//随机分配自己的身份(黑棋/白棋)
				srand(time(0)); 
				isBlackIdentity=random(1);
				
				//将客户端的身份发送过去
				sprintf(buf,SERVER_START_DATA,1-isBlackIdentity);
				send(s_client, buf, strlen(buf), 0);
				
				internetGameInit();
				break;
			}
			
		}
		
    }
	
	internetGameStart(s_client);
	closesocket(s_client);
    closesocket(s_server);     
    WSACleanup();

    return;
}
int internetGobangDataHandler(SOCKET s_client){
	COORD coord;
	int send_status, recv_status;
	
	char buf[100];
	memset(buf,0,sizeof(buf));
	recv_status = recv(s_client, buf, 100, 0);
	//接受对手的棋子数据
	if(recv_status>0){//数据有效
		buf[recv_status]=0x00;
		
		if(strcmp(buf,SURRENDER_SIGNAL)==0){
			MessageBox(NULL, TEXT(MESSAGE_YOU_WIN), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
			return GAMEOVER_SIGNAL;
		}else if(strcmp(buf,UNDO_REQUEST)==0){
			int result=MessageBox(NULL, TEXT(MESSAGE_UNDO_REQUEST), TEXT(TITLE_UNDO_REQUEST), MB_YESNO | MB_ICONQUESTION);
			
			if(result==IDYES){
				onLocalUndoClick(0,0);
				send(s_client,UNDO_ACCEPT,strlen(UNDO_ACCEPT),0);
				
			}else{
				send(s_client,UNDO_DENIED,strlen(UNDO_DENIED),0);
			}
			return CONTINUE_SIGNAL;
		}else{
			//更新本地棋盘数据
			sscanf(buf,PLACE_DATA,&coord.X,&coord.Y);
			int ret=Trigger(coord.X,coord.Y);
			if(ret==GAMEOVER_SIGNAL) {
				MessageBox(NULL, TEXT( (isBlackTheWinner&&isBlackIdentity) ? MESSAGE_YOU_WIN: MESSAGE_YOU_LOSE ), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
				return GAMEOVER_SIGNAL;
			}
			return CONTINUE_SIGNAL;
		}
	}
}
int internetUserOperationHandler(SOCKET s_client){
	COORD coord=waitForMouseClick();
	
	int ret=Trigger(coord.X,coord.Y);
	int send_status, recv_status;
	
	char buf[100];
	memset(buf,0,sizeof(buf));
	if(ret==UNDO_CLICK_SIGNAL){
		send(s_client,UNDO_REQUEST,strlen(UNDO_REQUEST),0);
		recv_status = recv(s_client, buf, 100, 0);
		if(recv_status>0){
			buf[recv_status]=0x00;
			if(strcmp(UNDO_ACCEPT,buf)==0){
				onLocalUndoClick(0,0);
			}else{
				pos(25,2);
				puts(MESSAGE_UNDO_DENIED);
			}
		}
		return CONTINUE_SIGNAL;
		
	}else if(ret==SURRENDER_CLICK_SIGNAL){
		send(s_client,SURRENDER_SIGNAL,strlen(SURRENDER_SIGNAL),0);
		MessageBox(NULL, TEXT(MESSAGE_SURRENDERED), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
		return GAMEOVER_SIGNAL;
	}else{
		//获取点击的棋子数据
		sprintf(buf,PLACE_DATA,coord.X,coord.Y);
		//发送给客户端
		send_status = send(s_client, buf, strlen(buf), 0);
		if(send_status!=SOCKET_ERROR){//发送成功
			//更新本地棋盘数据 并判断输赢
			
			if(ret==GAMEOVER_SIGNAL) {
				send(s_client,SURRENDER_SIGNAL,strlen(SURRENDER_SIGNAL),0);
				MessageBox(NULL, TEXT((isBlackTheWinner&&isBlackIdentity) ? MESSAGE_YOU_WIN: MESSAGE_YOU_LOSE ), TEXT(TITLE_GAMEOVER), MB_OK | MB_ICONINFORMATION);
				return GAMEOVER_SIGNAL;
			}
			SetConsoleTitle(gobangGameMode==GOBANG_SERVER_MODE?MESSAGE_WAIT_CLIENT_MOVE:MESSAGE_WAIT_SERVER_MOVE);
		}
		return CONTINUE_SIGNAL;
	}
			
}
void internetGameStart(SOCKET s_client){
	COORD coord;
	int send_status, recv_status;
	
	char buf[100];
	while(1){
		
		memset(buf,0,sizeof(buf));
		
		int res=CONTINUE_SIGNAL;
		pos(25,1);
		if(isBlackMove ^ isBlackIdentity==0){
			//是自己的回合
			//1.自己是黑棋，现在是黑棋移动
			//2.自己是白棋，现在是白棋移动
			puts(MESSAGE_WAIT_YOU);
			//接收用户的操作数据，处理并发送给对方
			res=internetUserOperationHandler(s_client);
		}else{
			//不是自己的回合
			//1.自己是黑棋，现在是白棋移动
			//2.自己是白棋，现在是黑棋移动
			puts(isBlackIdentity?MESSAGE_WAIT_WHITE:MESSAGE_WAIT_BLACK);
			//接收来自对方的操作数据并处理
			res=internetGobangDataHandler(s_client);
		}
		if(res==GAMEOVER_SIGNAL) break;
	}
}
