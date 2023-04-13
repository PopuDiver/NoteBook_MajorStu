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
/// ��װ�ͻ�����
/// </summary>
class ClientObject {
private:
	SOCKET m_cs;
	char m_buffer[PACKET_MAX_SIZE * 2];
	int m_lastPos;
public:
	ClientObject();
	virtual ~ClientObject();
	SOCKET getSocket();//�ṩһ���ӿ�
	void setSocket(SOCKET s);//����socket
	void OnMsgProc(ThreadRecv* pThread, MsgHead* pMsgHead);
	char* getBuffer();
	int getLastPos();
	void setLastPos(int pos);
	bool flag;
	void SendData(MsgHead* pMsgHead);
};

class TcpServer;

/// <summary>
/// �����߳���
/// </summary>
class ThreadRecv {
	HANDLE m_h;
private:
	std::vector<ClientObject*> m_vecClientBuffer;//��ʱ�Ŀͻ��ˣ����̹߳����
	std::map<SOCKET , ClientObject*> m_mapClient;//��ʵ��һ���ֿͻ���
	FD_SET m_fd_recv;
	int clientChange;//��һ����ǣ��Ƿ��пͻ��˽���
	CRITICAL_SECTION m_sec;//����һ���ٽ���Դ
	
public:
	TcpServer* m_pBaseThis;
	static int IsRun;
	void start();//�����̺߳���
	ThreadRecv();
	~ThreadRecv();
	//�̺߳��������Ϊ��Ա��������������Ϊ��̬
	static DWORD WINAPI TreadPro(LPVOID lp);
	int getMapClientSize();
	void addToClientBuffer(ClientObject* p);
	void setBaseObject(TcpServer* p);
};


/// <summary>
/// ��װ������
/// </summary>
class TcpServer
{
	ScoketInit scoketInit;
	SOCKET  sListen;
	//�ļ�����
	FD_SET fd_read;//�洢SOCKET����
	//����������ȥά��һ���ͻ�����������
	ThreadRecv* m_pThreadRecv;
	std::vector<ClientObject*> m_vecClient;//���еĿͻ���
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

