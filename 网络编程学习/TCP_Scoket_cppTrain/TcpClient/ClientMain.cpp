#include<stdio.h>
#include "TcpClient.h"

DWORD WINAPI TreadProc(LPVOID lp) {
	TcpClient sClient = *(TcpClient*)lp;
	printf("show join create talk exit\n");
	while (1) {
		char buff[1024] = { 0 };
		gets_s(buff, 1024);
		
		if (strcmp(buff, "show") == 0) {
			//��ʾ��ǰ�������¾�������Щ������/Ⱥ
			MsgShow msgShow;
			sClient.SendData(&msgShow);
			//send(sClient, (const char*)&msgShow, msgShow.dataLen , 0);
		}	
		else if(strncmp(buff,"join",strlen("join")) == 0) {
			int r = atoi(buff + strlen("join"));//ת��Ϊ����
			
			//join �������� ����һ��������
			MsgJoin msgJoin(r);
			sClient.SendData(&msgJoin);
			sClient.roomID = r;

			//send(sClient, (const char*)&msgJoin, msgJoin.dataLen, 0);
		}
		else if (strcmp(buff, "create") == 0) {
			//����һ��������
			MsgCreate msgCreate;
			sClient.SendData(&msgCreate);
		}
		else if (strcmp(buff, "talk") == 0) {
			//����
			printf("��ʼ����\n");
			while (true) {
				MsgTalk msgTalk(sClient.roomID);
				gets_s(msgTalk.getBuff(),1000);
				if (strcmp(msgTalk.getBuff(), "leave") == 0) {
					printf("��������...\n");
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
			//�˳�
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
	
	

	//���߳�ʵ���շ����ݷ���
	//����ָ��
	HANDLE hSends[TCPCLIENT_NUM] = { 0 };
	for (int i = 0; i < TCPCLIENT_NUM; i++) {
		hSends[i] = CreateThread(0, 0, TreadProc, (LPVOID)&tcpClient, 0, 0);
	}
	
	while (1) {
		//��������
		for (int i = 0; i < TCPCLIENT_NUM; i++) {
			tcpClient[i].RecvData();
		}
	}

	/*	�����ݣ�����ǰ����ϰ��
		while (true) {
			char buff[1024] = { 0 };
			gets_s(buff, 1024);
			send(sClient, buff, strlen(buff), 0);
		}*/

	
	WaitForMultipleObjects(TCPCLIENT_NUM,hSends, true, INFINITE);
	//WaitForSingleObject(h, INFINITE);//���޵ȴ�����

	//closesocket(sClient);

	getchar();

	return 0;
}