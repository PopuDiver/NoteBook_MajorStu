#include "TcpClient.h"

/// <summary>
/// ���죬��ʼ���ʹ����׽���
/// </summary>
TcpClient::TcpClient() {
	

	//���������׽���
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//�ж��׽����Ƿ񴴽��ɹ�
	if (sClient == SOCKET_ERROR) {
		printf("�����׽��ִ���ʧ�ܣ�\n");
		return ;
	}
	roomID = 0;
}

/// <summary>
/// ��������Դ�ͷ�
/// </summary>
TcpClient::~TcpClient() {
	closesocket(sClient);
}

/// <summary>
/// ��������
/// </summary>
/// <param name="ip"></param>
/// <param name="port"></param>
void TcpClient::ConnectServer(const char* ip, unsigned short port) {
	sockaddr_in sock_in;
	sock_in.sin_family = AF_INET;//Э����
	sock_in.sin_port = htons(port);//�˿ں�
	sock_in.sin_addr.S_un.S_addr = inet_addr(ip);//ʹ�ÿͻ���IP

	//�жϷ������������
	if (connect(sClient, (const sockaddr*)&sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		printf("���ӷ�����ʧ�ܣ�\n");
		return ;
	}
}

/// <summary>
/// ������
/// </summary>
void TcpClient::RecvData() {
	while (true) {
		char buff[1024] = { 0 };
		
		fd_set fd_recv;
		FD_ZERO(&fd_recv);
		FD_SET(sClient, &fd_recv);
		timeval tv = { 0,1 };
		int s = select(0, &fd_recv, NULL, NULL,&tv);
		if (s > 0) {
			int ret = recv(sClient, buff, 1024, 0);
			if (ret > 0) {
				//������
				OnMsgProc((MsgHead *) buff);
			}
			else {
				printf("�ͻ��˽�������ʧ��...\n");
				break;
			}
		}
		
	}
}

/// <summary>
/// ������
/// </summary>
/// <param name="pMsgHead"></param>
void TcpClient::SendData(MsgHead* pMsgHead) {
	send(sClient, (const char*)pMsgHead, pMsgHead->dataLen, 0);
}

/// <summary>
/// ʶ����Ϣ
/// </summary>
/// <param name="buff"></param>
void TcpClient::OnMsgProc(MsgHead* msgHead) {
	switch (msgHead->msgType) {
		case MSG_SHOW_REPLY:
		{
			MsgShowReply* reply = (MsgShowReply*)msgHead;
			int num = reply->roomCount;
			printf("----------������Ϣ-----------\n");
			for (int i = 0; i < num; i++) {
				printf("RoomID: %d ��%d / %d��\n", reply->rooms[i].roomID, reply->rooms[i].onlineNum, reply->rooms[i].totalNum);
			}
			printf("----------END-----------\n");
		}
			break;
		case MSG_CREATE_REPLY:
		{
			MsgCreateReply* reply = (MsgCreateReply*)msgHead;
			roomID = reply->roomID;
			printf("��������ɹ�...\n");
			printf("----------�·�����Ϣ----------\n");
			printf("RoomID: %d ��%d / %d��\n", reply->roomID, reply->onlineCount, reply->totalCount);
			printf("-------------END--------------\n");
		}
			break;
		case MSG_TALK:
		{
			MsgTalk* t = (MsgTalk*)msgHead;
			printf("--------���� %d ����Ϣ----------\n",t->ID);
			printf("%s...\n", t->getBuff());
			printf("-------------END--------------\n");
		}
			break;
		default:
			//printf("��Ϣ����ʧ�ܣ�\n");
			break;
	}
}

/// <summary>
/// �ӿ�
/// </summary>
/// <returns></returns>
SOCKET TcpClient::getSocket() {
	return sClient;
}