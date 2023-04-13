#include "TcpServer.h"

int ThreadRecv::IsRun = 1;

/// <summary>
/// ���캯��
/// </summary>
ClientObject::ClientObject() {
	m_cs = INVALID_SOCKET;
	m_lastPos = 0;
}

/// <summary>
/// ��������
/// </summary>
/// <param name=""></param>
ClientObject::~ClientObject() {
	if (m_cs != INVALID_SOCKET) {
		closesocket(m_cs);
	}
}

/// <summary>
/// �ṩ�ӿڣ���ȡsocket
/// </summary>
SOCKET ClientObject::getSocket() {
	return m_cs;
}

/// <summary>
/// ����socket
/// </summary>
void ClientObject::setSocket(SOCKET s) {
	m_cs = s;
}

/// <summary>
/// ����
/// </summary>
TcpServer::TcpServer() {
	g = 0;
	m_pThreadRecv = NULL;
	m_threadCount = 0;
	//���������׽���
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	FD_ZERO(&fd_read);//��ʼ��

	//�ж��׽����Ƿ񴴽��ɹ�
	if (sListen == SOCKET_ERROR) {
		printf("�����׽��ִ���ʧ�ܣ�\n");
		return ;
	}
}

/// <summary>
/// ����
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
/// ��
/// </summary>
/// <param name="port"></param>
void TcpServer::Bind(unsigned short port) {
	sockaddr_in sock_in;
	sock_in.sin_family = AF_INET;//Э����
	sock_in.sin_port = htons(port);//�˿ں�
	sock_in.sin_addr.S_un.S_addr = INADDR_ANY;//ֱ�ӻ�ȡ����IP
	//sock_in.sin_addr.S_un.S_addr = inet_addr("192.168.1.105");//ʹ�ÿͻ���IP
	//���׽���
	int ret = bind(sListen, (const sockaddr*)&sock_in, sizeof(sockaddr_in));

	//�жϰ��Ƿ�ɹ�
	if (ret == SOCKET_ERROR) {
		printf("���׽���ʧ�ܣ�\n");
		closesocket(sListen);
		return ;
	}
}

/// <summary>
/// ����
/// </summary>
void TcpServer::Listen() {
	//��������
	if (listen(sListen, 10) == SOCKET_ERROR) {
		printf("����ʧ�ܣ�\n");
		return ;
	}
	FD_SET(sListen, &fd_read);//��sListen�׽��ַŵ�fd_read
}

/// <summary>
/// ɸѡ�¼�
/// </summary>
void TcpServer::OnListen() {

	FD_SET fd_tmp = fd_read;//fd_read�൱���Ǹ����ݣ�ÿ�δ���tmp
	const timeval tv = { 1,0 };
	//select����ɸѡ���������¼����׽��ֻ���fd_tmp�����£�û�������¼��ᱻɾ��
	int ret = select(NULL, &fd_tmp, NULL, NULL, &tv);
	if (ret == 0) {
		//���û�������¼�����
		Sleep(1);
		return;
	}

	for (int i = 0; i < fd_tmp.fd_count; i++) {
		//��������׽����������¼���֤���пͻ��������ӷ�����
		if (fd_tmp.fd_array[i] == sListen) {
			sockaddr_in clientAddr;//�ͻ���
			int nlen = sizeof(sockaddr_in);

			//���ܿͻ��˵�����
			SOCKET sClient = accept(sListen, (sockaddr*)&clientAddr, &nlen);//���ܿͻ��˵ķ���socket

			//�жϽ��յ�socket�Ƿ����
			if (sClient == SOCKET_ERROR) {
				printf("���տͻ���ʧ�ܣ�\n");
				//closesocket(sListen);
				return ;
			}

			printf("�ͻ��� %d �ѽ������ӣ�%s...\n",g++, inet_ntoa(clientAddr.sin_addr));
			ClientObject* pcb =  new ClientObject;
			pcb->setSocket(sClient);
			m_vecClient.push_back(pcb);
			//������̴߳��� ���߳��д���ͻ������ٵ��߳�
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
/// ��������
/// </summary>
/// <param name="pMsgHead"></param>
void ClientObject::OnMsgProc(ThreadRecv* pThread, MsgHead* pMsgHead) {

	pThread->m_pBaseThis->OnMsgProc(this,pMsgHead);
	
}

/// <summary>
/// ��ȡ����
/// </summary>
/// <returns></returns>
char* ClientObject::getBuffer() {
	return m_buffer;
}

/// <summary>
/// ��ȡ����λ��
/// </summary>
/// <returns></returns>
int ClientObject::getLastPos() {
	return m_lastPos;
}

/// <summary>
/// ��¼λ��
/// </summary>
/// <param name="pos"></param>
void ClientObject::setLastPos(int pos) {
	m_lastPos = pos;
}

/// <summary>
/// ʵ���̺߳���
/// </summary>
/// <param name="lp"></param>
/// <returns></returns>
DWORD WINAPI ThreadRecv::TreadPro(LPVOID lp) {
	ThreadRecv* threadRecv = (ThreadRecv*)lp;
	while (IsRun) {
		//������ת�Ƶ�������
		if (!threadRecv->m_vecClientBuffer.empty()) {
			//�̼߳�ͬ�� -> ����
			EnterCriticalSection(&threadRecv->m_sec);
			for (int s = threadRecv->m_vecClientBuffer.size() - 1; s >= 0; s--) {
				threadRecv->m_mapClient[threadRecv->m_vecClientBuffer[s]->getSocket()] =
					threadRecv->m_vecClientBuffer[s];
			}
			threadRecv->m_vecClientBuffer.clear();
			threadRecv->clientChange = true;
			LeaveCriticalSection(&threadRecv->m_sec);
		}
		//����ͻ��˷����仯������m_fd_recv
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
				//����ͻ����׽����������¼���֤���ͻ������ڷ������ݣ���������������
				char buff[1024] = { 0 };
				if (iter != threadRecv->m_mapClient.end()) {
					char* clientBuffer = iter->second->getBuffer();
					int lastPos = iter->second->getLastPos();
					//��ǰ�������ֽ���
					int nlen = recv(fd_tmp.fd_array[i], clientBuffer + lastPos, PACKET_MAX_SIZE - lastPos, 0);
					if (nlen > 0) {

						//�ҵ�map�еĿͻ��˶��󲢽�������
						MsgHead* phead = (MsgHead*)clientBuffer;
						lastPos = lastPos + nlen;
						//ѭ�����
						while (lastPos >= sizeof(MsgHead)) {

							if (lastPos>= phead->dataLen) {

								iter->second->OnMsgProc(threadRecv,(MsgHead*)phead);//������Ϣͷ
								memcpy(clientBuffer, clientBuffer + phead->dataLen, lastPos - phead->dataLen);
								lastPos -= phead->dataLen;

							}
							else {
								break;
							}
							
						}
						iter->second->setLastPos(lastPos);
						//printf("���յ������ݣ�%s\n", buff);
					}
					else {
						//����ʵ��map���Ƴ��ͻ���
						if (iter != threadRecv->m_mapClient.end()) {
							threadRecv->m_mapClient.erase(iter);
						}
						threadRecv->clientChange = true;
						printf("��ͻ��˶Ͽ����ӣ�\n");
						break;
					}
				}	
			}
		}
		
	}
	return 0;
}

/// <summary>
/// �����̺߳���
/// </summary>
void ThreadRecv::start() {
	m_h = CreateThread(0, 0,TreadPro ,(LPVOID)this, 0, 0);
}

/// <summary>
/// �ͻ��˷�����Ϣ
/// </summary>
/// <param name="pMsgHead"></param>
void ClientObject::SendData(MsgHead* pMsgHead) {
	send(m_cs, (const char*)pMsgHead, pMsgHead->dataLen, 0);
}

/// <summary>
/// �߳�����
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
///��������Ϣ����
/// </summary>
/// <param name="pClient"></param>
/// <param name="pMsgHead"></param>
void TcpServer::OnMsgProc(ClientObject* pClient, MsgHead* pMsgHead) {

}

/// <summary>
/// ���캯��
/// </summary>
ThreadRecv::ThreadRecv() {
	clientChange = true;
	InitializeCriticalSection(&m_sec);//��ʼ���ٽ���Դ����
}

/// <summary>
/// ��������
/// </summary>
ThreadRecv::~ThreadRecv() {
	WaitForSingleObject(m_h, INFINITE);
	DeleteCriticalSection(&m_sec);//�ͷ��ٽ���Դ
}

/// <summary>
/// ��ȡ�ͻ��˵�����
/// </summary>
int ThreadRecv::getMapClientSize() {
	return m_mapClient.size();
}

/// <summary>
/// ����
/// </summary>
/// <param name="p"></param>
void ThreadRecv::addToClientBuffer(ClientObject* p) {
	EnterCriticalSection(&m_sec);
	m_vecClientBuffer.push_back(p);
	LeaveCriticalSection(&m_sec);
}

/// <summary>
/// ��ȡ����������
/// </summary>
/// <param name="p"></param>
void ThreadRecv::setBaseObject(TcpServer* p) {
	this->m_pBaseThis = p;
}