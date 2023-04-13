#include "TcpClient.h"

/// <summary>
/// 构造，初始化和创建套接字
/// </summary>
TcpClient::TcpClient() {
	

	//创建监听套接字
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//判断套接字是否创建成功
	if (sClient == SOCKET_ERROR) {
		printf("监听套接字创建失败！\n");
		return ;
	}
	roomID = 0;
}

/// <summary>
/// 析构，资源释放
/// </summary>
TcpClient::~TcpClient() {
	closesocket(sClient);
}

/// <summary>
/// 建立连接
/// </summary>
/// <param name="ip"></param>
/// <param name="port"></param>
void TcpClient::ConnectServer(const char* ip, unsigned short port) {
	sockaddr_in sock_in;
	sock_in.sin_family = AF_INET;//协议族
	sock_in.sin_port = htons(port);//端口号
	sock_in.sin_addr.S_un.S_addr = inet_addr(ip);//使用客户端IP

	//判断服务器连接情况
	if (connect(sClient, (const sockaddr*)&sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		printf("连接服务器失败！\n");
		return ;
	}
}

/// <summary>
/// 收数据
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
				//收数据
				OnMsgProc((MsgHead *) buff);
			}
			else {
				printf("客户端接受数据失败...\n");
				break;
			}
		}
		
	}
}

/// <summary>
/// 发数据
/// </summary>
/// <param name="pMsgHead"></param>
void TcpClient::SendData(MsgHead* pMsgHead) {
	send(sClient, (const char*)pMsgHead, pMsgHead->dataLen, 0);
}

/// <summary>
/// 识别消息
/// </summary>
/// <param name="buff"></param>
void TcpClient::OnMsgProc(MsgHead* msgHead) {
	switch (msgHead->msgType) {
		case MSG_SHOW_REPLY:
		{
			MsgShowReply* reply = (MsgShowReply*)msgHead;
			int num = reply->roomCount;
			printf("----------房间信息-----------\n");
			for (int i = 0; i < num; i++) {
				printf("RoomID: %d （%d / %d）\n", reply->rooms[i].roomID, reply->rooms[i].onlineNum, reply->rooms[i].totalNum);
			}
			printf("----------END-----------\n");
		}
			break;
		case MSG_CREATE_REPLY:
		{
			MsgCreateReply* reply = (MsgCreateReply*)msgHead;
			roomID = reply->roomID;
			printf("创建房间成功...\n");
			printf("----------新房间信息----------\n");
			printf("RoomID: %d （%d / %d）\n", reply->roomID, reply->onlineCount, reply->totalCount);
			printf("-------------END--------------\n");
		}
			break;
		case MSG_TALK:
		{
			MsgTalk* t = (MsgTalk*)msgHead;
			printf("--------来自 %d 的消息----------\n",t->ID);
			printf("%s...\n", t->getBuff());
			printf("-------------END--------------\n");
		}
			break;
		default:
			//printf("消息解析失败！\n");
			break;
	}
}

/// <summary>
/// 接口
/// </summary>
/// <returns></returns>
SOCKET TcpClient::getSocket() {
	return sClient;
}