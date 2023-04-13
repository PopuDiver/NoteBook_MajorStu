#pragma once
#include<stdio.h>
#include "../ScoketInit/ScoketInit.hpp"
#include "../MessageType/messageType.hpp"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996)
class TcpClient
{
	ScoketInit scoketInit;
	SOCKET  sClient;
public:
	int roomID;
	TcpClient();
	virtual ~TcpClient();
	void ConnectServer(const char* ip, unsigned short port);
	void RecvData();
	void SendData(MsgHead* pMsgHead);
	void OnMsgProc(MsgHead* buff);
	SOCKET getSocket();
};

