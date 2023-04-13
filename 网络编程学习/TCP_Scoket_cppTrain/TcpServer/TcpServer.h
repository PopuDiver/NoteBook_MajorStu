#pragma once
#include<vector>
#include<iostream>
#include<map>
#include<queue>
#include "../ScoketInit/ScoketInit.hpp"
#include "../MessageType/messageType.hpp"
#include "windows.h"

class ThreadRecv;

/// <summary>
/// 封装客户端类
/// </summary>
class ClientObject {
private:
	SOCKET m_cs;
	char m_buffer[PACKET_MAX_SIZE * 2];
	int m_lastPos;
public:
	ClientObject();
	virtual ~ClientObject();
	SOCKET getSocket();//提供一个接口
	void setSocket(SOCKET s);//设置socket
	void OnMsgProc(ThreadRecv* pThread, MsgHead* pMsgHead);
	char* getBuffer();
	int getLastPos();
	void setLastPos(int pos);
	bool flag;
	void SendData(MsgHead* pMsgHead);
};

class TcpServer;

/// <summary>
/// 创建线程类
/// </summary>
class ThreadRecv {
	HANDLE m_h;
private:
	std::vector<ClientObject*> m_vecClientBuffer;//临时的客户端，多线程共享的
	std::map<SOCKET , ClientObject*> m_mapClient;//真实的一部分客户端
	FD_SET m_fd_recv;
	int clientChange;//置一个标记，是否有客户端接入
	CRITICAL_SECTION m_sec;//创建一个临界资源
	
public:
	TcpServer* m_pBaseThis;
	static int IsRun;
	void start();//启动线程函数
	ThreadRecv();
	~ThreadRecv();
	//线程函数如果作为成员函数，必须声明为静态
	static DWORD WINAPI TreadPro(LPVOID lp);
	int getMapClientSize();
	void addToClientBuffer(ClientObject* p);
	void setBaseObject(TcpServer* p);
};


/// <summary>
/// 封装服务器
/// </summary>
class TcpServer
{
	ScoketInit scoketInit;
	SOCKET  sListen;
	//文件集合
	FD_SET fd_read;//存储SOCKET对象
	//服务器里面去维护一个客户端容器数组
	ThreadRecv* m_pThreadRecv;
	std::vector<ClientObject*> m_vecClient;//所有的客户端
	int m_threadCount;
public:
	int g;
	TcpServer();
	virtual ~TcpServer();

	void Bind(unsigned short port);
	void Listen();
	void OnListen();
	//void SendData(MsgHead* pMsgHead);

	void StartThread(int nCount);

	virtual void OnMsgProc(ClientObject* pClient,MsgHead* pMsgHead);
	
};

