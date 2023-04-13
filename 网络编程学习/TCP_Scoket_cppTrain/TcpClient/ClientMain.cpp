#include<stdio.h>
#include "TcpClient.h"

DWORD WINAPI TreadProc(LPVOID lp) {
	TcpClient sClient = *(TcpClient*)lp;
	printf("show join create talk exit\n");
	while (1) {
		char buff[1024] = { 0 };
		gets_s(buff, 1024);
		
		if (strcmp(buff, "show") == 0) {
			//显示当前服务器下具体有哪些聊天室/群
			MsgShow msgShow;
			sClient.SendData(&msgShow);
			//send(sClient, (const char*)&msgShow, msgShow.dataLen , 0);
		}	
		else if(strncmp(buff,"join",strlen("join")) == 0) {
			int r = atoi(buff + strlen("join"));//转换为数字
			
			//join 聊天室名 加入一个聊天室
			MsgJoin msgJoin(r);
			sClient.SendData(&msgJoin);
			sClient.roomID = r;

			//send(sClient, (const char*)&msgJoin, msgJoin.dataLen, 0);
		}
		else if (strcmp(buff, "create") == 0) {
			//创建一个聊天室
			MsgCreate msgCreate;
			sClient.SendData(&msgCreate);
		}
		else if (strcmp(buff, "talk") == 0) {
			//聊天
			printf("开始聊天\n");
			while (true) {
				MsgTalk msgTalk(sClient.roomID);
				gets_s(msgTalk.getBuff(),1000);
				if (strcmp(msgTalk.getBuff(), "leave") == 0) {
					printf("结束聊天...\n");
					MsgLeave msgLeave;
					msgLeave.roomID = sClient.roomID;
					sClient.SendData(&msgLeave);
					//send(sClient, (const char*)&msgLeave, msgLeave.dataLen, 0);
					break;
				}
				sClient.SendData(&msgTalk);
				//send(sClient, (const char*)&msgTalk, msgTalk.dataLen, 0);
			}
		}
		else if (strcmp(buff, "exit") == 0) {
			//退出
			break;
		}
	}
	return -1;
}

#define TCPCLIENT_NUM 1

int main() {
	
	TcpClient tcpClient[TCPCLIENT_NUM];
	for (int i = 0; i < TCPCLIENT_NUM; i++) {
		tcpClient[i].ConnectServer("172.23.96.255", 12306);
	}
	
	

	//多线程实现收发数据分离
	//发送指令
	HANDLE hSends[TCPCLIENT_NUM] = { 0 };
	for (int i = 0; i < TCPCLIENT_NUM; i++) {
		hSends[i] = CreateThread(0, 0, TreadProc, (LPVOID)&tcpClient, 0, 0);
	}
	
	while (1) {
		//接受数据
		for (int i = 0; i < TCPCLIENT_NUM; i++) {
			tcpClient[i].RecvData();
		}
	}

	/*	发数据，但是前置练习版
		while (true) {
			char buff[1024] = { 0 };
			gets_s(buff, 1024);
			send(sClient, buff, strlen(buff), 0);
		}*/

	
	WaitForMultipleObjects(TCPCLIENT_NUM,hSends, true, INFINITE);
	//WaitForSingleObject(h, INFINITE);//无限等待进程

	//closesocket(sClient);

	getchar();

	return 0;
}