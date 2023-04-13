#include "TcpServer.h"

int ThreadRecv::IsRun = 1;

/// <summary>
/// 构造函数
/// </summary>
ClientObject::ClientObject() {
	m_cs = INVALID_SOCKET;
	m_lastPos = 0;
}

/// <summary>
/// 析构函数
/// </summary>
/// <param name=""></param>
ClientObject::~ClientObject() {
	if (m_cs != INVALID_SOCKET) {
		closesocket(m_cs);
	}
}

/// <summary>
/// 提供接口，获取socket
/// </summary>
SOCKET ClientObject::getSocket() {
	return m_cs;
}

/// <summary>
/// 设置socket
/// </summary>
void ClientObject::setSocket(SOCKET s) {
	m_cs = s;
}

/// <summary>
/// 构造
/// </summary>
TcpServer::TcpServer() {
	g = 0;
	m_pThreadRecv = NULL;
	m_threadCount = 0;
	//创建监听套接字
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	FD_ZERO(&fd_read);//初始化

	//判断套接字是否创建成功
	if (sListen == SOCKET_ERROR) {
		printf("监听套接字创建失败！\n");
		return ;
	}
}

/// <summary>
/// 析构
/// </summary>
TcpServer::~TcpServer() {
	ThreadRecv::IsRun = 0;
	closesocket(sListen);
	for (int s = m_vecClient.size() - 1; s >= 0; s--) {
		delete m_vecClient[s];
	}
	if (m_pThreadRecv != NULL) {
		delete[] m_pThreadRecv;
	}
	m_vecClient.clear();
}

/// <summary>
/// 绑定
/// </summary>
/// <param name="port"></param>
void TcpServer::Bind(unsigned short port) {
	sockaddr_in sock_in;
	sock_in.sin_family = AF_INET;//协议族
	sock_in.sin_port = htons(port);//端口号
	sock_in.sin_addr.S_un.S_addr = INADDR_ANY;//直接获取本机IP
	//sock_in.sin_addr.S_un.S_addr = inet_addr("192.168.1.105");//使用客户端IP
	//绑定套接字
	int ret = bind(sListen, (const sockaddr*)&sock_in, sizeof(sockaddr_in));

	//判断绑定是否成功
	if (ret == SOCKET_ERROR) {
		printf("绑定套接字失败！\n");
		closesocket(sListen);
		return ;
	}
}

/// <summary>
/// 监听
/// </summary>
void TcpServer::Listen() {
	//持续监听
	if (listen(sListen, 10) == SOCKET_ERROR) {
		printf("监听失败！\n");
		return ;
	}
	FD_SET(sListen, &fd_read);//将sListen套接字放到fd_read
}

/// <summary>
/// 筛选事件
/// </summary>
void TcpServer::OnListen() {

	FD_SET fd_tmp = fd_read;//fd_read相当于是个备份，每次处理tmp
	const timeval tv = { 1,0 };
	//select进行筛选，有网络事件的套接字会在fd_tmp中留下，没有网络事件会被删除
	int ret = select(NULL, &fd_tmp, NULL, NULL, &tv);
	if (ret == 0) {
		//如果没有网络事件发生
		Sleep(1);
		return;
	}

	for (int i = 0; i < fd_tmp.fd_count; i++) {
		//如果监听套接字有网络事件，证明有客户端在连接服务器
		if (fd_tmp.fd_array[i] == sListen) {
			sockaddr_in clientAddr;//客户端
			int nlen = sizeof(sockaddr_in);

			//接受客户端的连接
			SOCKET sClient = accept(sListen, (sockaddr*)&clientAddr, &nlen);//接受客户端的发送socket

			//判断接收的socket是否出错
			if (sClient == SOCKET_ERROR) {
				printf("接收客户端失败！\n");
				//closesocket(sListen);
				return ;
			}

			printf("客户端 %d 已建立连接：%s...\n",g++, inet_ntoa(clientAddr.sin_addr));
			ClientObject* pcb =  new ClientObject;
			pcb->setSocket(sClient);
			m_vecClient.push_back(pcb);
			//分配给线程处理 看线程中处理客户端最少的线程
			ThreadRecv* tmp = m_pThreadRecv;
			for (int i = 1; i < m_threadCount; i++) {
				if (tmp->getMapClientSize() > m_pThreadRecv[i].getMapClientSize()) {
					tmp = &m_pThreadRecv[i];
				}
			}
			tmp->addToClientBuffer(pcb);
			FD_SET(sClient, &fd_read);
		}
		else {
			
		}
	}

}

/// <summary>
/// 处理数据
/// </summary>
/// <param name="pMsgHead"></param>
void ClientObject::OnMsgProc(ThreadRecv* pThread, MsgHead* pMsgHead) {

	pThread->m_pBaseThis->OnMsgProc(this,pMsgHead);
	
}

/// <summary>
/// 获取数据
/// </summary>
/// <returns></returns>
char* ClientObject::getBuffer() {
	return m_buffer;
}

/// <summary>
/// 获取数据位置
/// </summary>
/// <returns></returns>
int ClientObject::getLastPos() {
	return m_lastPos;
}

/// <summary>
/// 记录位置
/// </summary>
/// <param name="pos"></param>
void ClientObject::setLastPos(int pos) {
	m_lastPos = pos;
}

/// <summary>
/// 实现线程函数
/// </summary>
/// <param name="lp"></param>
/// <returns></returns>
DWORD WINAPI ThreadRecv::TreadPro(LPVOID lp) {
	ThreadRecv* threadRecv = (ThreadRecv*)lp;
	while (IsRun) {
		//生产者转移到消费者
		if (!threadRecv->m_vecClientBuffer.empty()) {
			//线程间同步 -> 加锁
			EnterCriticalSection(&threadRecv->m_sec);
			for (int s = threadRecv->m_vecClientBuffer.size() - 1; s >= 0; s--) {
				threadRecv->m_mapClient[threadRecv->m_vecClientBuffer[s]->getSocket()] =
					threadRecv->m_vecClientBuffer[s];
			}
			threadRecv->m_vecClientBuffer.clear();
			threadRecv->clientChange = true;
			LeaveCriticalSection(&threadRecv->m_sec);
		}
		//如果客户端发生变化，更新m_fd_recv
		if (threadRecv->clientChange) {
			auto iterBegin = threadRecv->m_mapClient.begin();
			int i = 0;
			for (; iterBegin != threadRecv->m_mapClient.end();iterBegin++, i++) {
				threadRecv->m_fd_recv.fd_array[i] = iterBegin->second->getSocket();
			}
			threadRecv->m_fd_recv.fd_count = i;
		}
		fd_set fd_tmp = threadRecv->m_fd_recv;
		timeval tv = { 0,1 };
		int res = select(0, &fd_tmp, 0, 0, &tv);
		if (res > 0) {
			for (int i = 0; i < fd_tmp.fd_count; i++) {
				auto iter = threadRecv->m_mapClient.find(fd_tmp.fd_array[i]);
				//如果客户端套接字有网络事件，证明客户端正在发送数据，服务器接收数据
				char buff[1024] = { 0 };
				if (iter != threadRecv->m_mapClient.end()) {
					char* clientBuffer = iter->second->getBuffer();
					int lastPos = iter->second->getLastPos();
					//当前读到的字节数
					int nlen = recv(fd_tmp.fd_array[i], clientBuffer + lastPos, PACKET_MAX_SIZE - lastPos, 0);
					if (nlen > 0) {

						//找到map中的客户端对象并接收数据
						MsgHead* phead = (MsgHead*)clientBuffer;
						lastPos = lastPos + nlen;
						//循环拆包
						while (lastPos >= sizeof(MsgHead)) {

							if (lastPos>= phead->dataLen) {

								iter->second->OnMsgProc(threadRecv,(MsgHead*)phead);//处理消息头
								memcpy(clientBuffer, clientBuffer + phead->dataLen, lastPos - phead->dataLen);
								lastPos -= phead->dataLen;

							}
							else {
								break;
							}
							
						}
						iter->second->setLastPos(lastPos);
						//printf("接收到的数据：%s\n", buff);
					}
					else {
						//从真实的map中移除客户端
						if (iter != threadRecv->m_mapClient.end()) {
							threadRecv->m_mapClient.erase(iter);
						}
						threadRecv->clientChange = true;
						printf("与客户端断开连接！\n");
						break;
					}
				}	
			}
		}
		
	}
	return 0;
}

/// <summary>
/// 启动线程函数
/// </summary>
void ThreadRecv::start() {
	m_h = CreateThread(0, 0,TreadPro ,(LPVOID)this, 0, 0);
}

/// <summary>
/// 客户端发送消息
/// </summary>
/// <param name="pMsgHead"></param>
void ClientObject::SendData(MsgHead* pMsgHead) {
	send(m_cs, (const char*)pMsgHead, pMsgHead->dataLen, 0);
}

/// <summary>
/// 线程启动
/// </summary>
/// <param name="nCount"></param>
void TcpServer::StartThread(int nCount) {
	m_threadCount = nCount;
	m_pThreadRecv = new ThreadRecv[nCount];

	for (int i = 0; i < nCount; i++) {
		m_pThreadRecv[i].start();
		m_pThreadRecv[i].setBaseObject(this);
	}
}

/// <summary>
///服务器消息处理
/// </summary>
/// <param name="pClient"></param>
/// <param name="pMsgHead"></param>
void TcpServer::OnMsgProc(ClientObject* pClient, MsgHead* pMsgHead) {

}

/// <summary>
/// 构造函数
/// </summary>
ThreadRecv::ThreadRecv() {
	clientChange = true;
	InitializeCriticalSection(&m_sec);//初始化临界资源对象
}

/// <summary>
/// 析构函数
/// </summary>
ThreadRecv::~ThreadRecv() {
	WaitForSingleObject(m_h, INFINITE);
	DeleteCriticalSection(&m_sec);//释放临界资源
}

/// <summary>
/// 获取客户端的数量
/// </summary>
int ThreadRecv::getMapClientSize() {
	return m_mapClient.size();
}

/// <summary>
/// 分配
/// </summary>
/// <param name="p"></param>
void ThreadRecv::addToClientBuffer(ClientObject* p) {
	EnterCriticalSection(&m_sec);
	m_vecClientBuffer.push_back(p);
	LeaveCriticalSection(&m_sec);
}

/// <summary>
/// 获取服务器对象
/// </summary>
/// <param name="p"></param>
void ThreadRecv::setBaseObject(TcpServer* p) {
	this->m_pBaseThis = p;
}