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
	//������Ϸ
	setIsBlackMove(1);
	clear();
	//ע�ᰴť������
	initListener();
	resetBoard();
	
	SetConsoleTitle(TITLE_GOBANG);
	hideCursor();
	//��Ⱦ��ť
	renderBotton();
	pos(25,0);
	puts(BANNER_GOBANG);
	pos(25,19);
	//����Լ������
	puts(isBlackIdentity?MESSAGE_IDENTITY_BLACK:MESSAGE_IDENTITY_WHITE);		
}
void internetClientMode(char* SERVER_IP){

    WORD ws_version = MAKEWORD(2, 2);   //ָ��Winsock version
    WSADATA wsaData;                    //WSA �����Ĳ���

    /*��ʼ��winsock*/
    WSAStartup(ws_version, &wsaData);

	int send_status, recv_status;
	SOCKADDR_IN addr_server;
	SOCKET s_client;
	while(1){
		s_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN addr_server;
        addr_server.sin_family = AF_INET;   //Э��
        addr_server.sin_port = htons(5050); //�˿�
        addr_server.sin_addr.s_addr = inet_addr(SERVER_IP);
		int cnct_status = connect(s_client, (SOCKADDR*)&addr_server, sizeof(SOCKADDR));
	    if(cnct_status == 0)//���ӳɹ�
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
		
		//��������������ӳɹ�������READY�ź�
		send_status = send(s_client, CLIENT_READYSIGNAL, strlen(CLIENT_READYSIGNAL), 0);
		if(send_status == SOCKET_ERROR)//����ʧ��
		{
			puts(MESSAGE_CONNECTION_TIMEOUT);
		}
		else//���ͳɹ�
		{
			//�ȴ�����˴����������Ϣ
			recv_status = recv(s_client, buf, 100, 0);
			buf[recv_status] = 0x00;//���ַ�����β���� ������ 0x00 == /0 �ο��ش�
			
			if(buf[0]=='S'){//��Ϣ��Ч
				
				//��ȡ�����Ϣ
				sscanf(buf,SERVER_START_DATA,&isBlackIdentity);
				//������Ϸ
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
    WORD ws_version = MAKEWORD(2, 2);   //ָ��Winsock version
    WSADATA wsaData;                    //WSA �����Ĳ���

    /*��ʼ��winsock*/
    WSAStartup(ws_version, &wsaData);

    /*socket*/
    SOCKET s_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr_server;
    addr_server.sin_family = AF_INET;   //Э��
    addr_server.sin_port = htons(5050); //�˿�
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);    //IP:����IP

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

    SOCKADDR_IN addr_client;            //�洢client��ַ��Ϣ
    int len = sizeof(SOCKADDR);
    SOCKET s_client;                    //���ӵ�socket
	

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
		//��ͻ��˽������ӳɹ������ܿͻ���READY�ź�
		puts(MESSAGE_CONNECTION_ESTABLISHED);
		recv_status = recv(s_client, buf, 100, 0);
		if(recv_status > 0)
		{
			buf[recv_status] = 0x00;//���ַ�����β���� ������ 0x00 == /0 �ο��ش�
			if(strcmp(buf,CLIENT_READYSIGNAL)==0){
				//�ź���Ч
				
				puts(MESSAGE_STARTING_GAME);
				
				//��������Լ������(����/����)
				srand(time(0)); 
				isBlackIdentity=random(1);
				
				//���ͻ��˵���ݷ��͹�ȥ
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
	//���ܶ��ֵ���������
	if(recv_status>0){//������Ч
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
			//���±�����������
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
		//��ȡ�������������
		sprintf(buf,PLACE_DATA,coord.X,coord.Y);
		//���͸��ͻ���
		send_status = send(s_client, buf, strlen(buf), 0);
		if(send_status!=SOCKET_ERROR){//���ͳɹ�
			//���±����������� ���ж���Ӯ
			
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
			//���Լ��Ļغ�
			//1.�Լ��Ǻ��壬�����Ǻ����ƶ�
			//2.�Լ��ǰ��壬�����ǰ����ƶ�
			puts(MESSAGE_WAIT_YOU);
			//�����û��Ĳ������ݣ��������͸��Է�
			res=internetUserOperationHandler(s_client);
		}else{
			//�����Լ��Ļغ�
			//1.�Լ��Ǻ��壬�����ǰ����ƶ�
			//2.�Լ��ǰ��壬�����Ǻ����ƶ�
			puts(isBlackIdentity?MESSAGE_WAIT_WHITE:MESSAGE_WAIT_BLACK);
			//�������ԶԷ��Ĳ������ݲ�����
			res=internetGobangDataHandler(s_client);
		}
		if(res==GAMEOVER_SIGNAL) break;
	}
}
