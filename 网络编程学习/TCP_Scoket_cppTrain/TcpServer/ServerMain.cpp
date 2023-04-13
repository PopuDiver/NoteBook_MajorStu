#include<stdio.h>
#include "TcpServer.h"

//#pragma warning(disable:4996)

//处理客户端发来的消息
void DealWithData(MsgHead *msgHead) {
	
}

DWORD WINAPI ThreadProc(LPVOID lp) {
	SOCKET sClient = *(SOCKET*)lp;
	while (true) {
		char buff[1024] = { 0 };
		int result = recv(sClient, buff, 1024, 0);
		if (result > 0) {
			printf("接收到的数据：%s\n", buff);
		}
		else {
			printf("与客户端断开连接！\n");
			break;
		}
	}
	return NULL;
}

/// <summary>
/// 聊天室
/// </summary>
class TalkingRoom {
#define ROOM_MEMBER_MAX_NUM 50
private:
	
	CRITICAL_SECTION m_cs;
public:
	int m_roomID;
	std::map<SOCKET, ClientObject*> m_roomMember;
	TalkingRoom() {
		InitializeCriticalSection(&m_cs);
	}
	~TalkingRoom() {
		DeleteCriticalSection(&m_cs);
	}
	void addMember(ClientObject* co) {
		EnterCriticalSection(&m_cs);
		if (m_roomMember.size() < ROOM_MEMBER_MAX_NUM) {
			m_roomMember[co->getSocket()] = co;
		}
		LeaveCriticalSection(&m_cs);
	}
	void deleteMember(ClientObject* co) {
		EnterCriticalSection(&m_cs);

		auto iter = m_roomMember.find(co->getSocket());
		if (iter != m_roomMember.end()) {
			m_roomMember.erase(iter);
		}

		LeaveCriticalSection(&m_cs);
	}

	void sendToAll(ClientObject* co, MsgHead* pHead) {
		auto iter = m_roomMember.begin();
		while (iter != m_roomMember.end()) {
			if (iter->first != co->getSocket()) {
				iter->second->SendData(pHead);
			}
			iter++;
		}
	}
};

class ResourceManage {
private:
	std::queue<int> m_queRoomID;
	static ResourceManage* instance;
	ResourceManage() {

	}
	
public:
	void InitRes() {
		for (int i = 1; i <= 1000; i++) {
			m_queRoomID.push(i);
		}
	}
	static ResourceManage* getInstance() {
		if (instance == NULL) {
			instance = new ResourceManage;
		}
		return instance;
	}
	int getRoomID() {
		int t = m_queRoomID.front();
		m_queRoomID.pop();
		return t;
	}

	void pushRoomID(int id) {
		m_queRoomID.push(id);
	}
};

ResourceManage* ResourceManage::instance = NULL;

class MyTcpServer :public TcpServer {
	std::map<int, TalkingRoom> m_room;
public:
	void showRoom(ClientObject* pClient, MsgHead* pMsgHead) {
		MsgShowReply showReply;
		showReply.roomCount = m_room.size();
		auto iter = m_room.begin();
		int i = 0;
		while (iter != m_room.end()) {
			showReply.rooms[i].roomID = iter->second.m_roomID;
			showReply.rooms[i].onlineNum = iter->second.m_roomMember.size();
			showReply.rooms[i].totalNum = ROOM_MEMBER_MAX_NUM;
			i++;
			iter++;
		}
		pClient->SendData(&showReply);
	}

	void CreateRoom(ClientObject* pClient, MsgHead* pMsgHead) {
		
		ResourceManage* instance = ResourceManage::getInstance();
		instance->InitRes();
		int roomID = instance->getRoomID();
		TalkingRoom* tr = new TalkingRoom;
		tr->m_roomID = roomID;
		tr->addMember(pClient);
		m_room[roomID] = *tr;
		//showRoom(pClient, pMsgHead);
		MsgCreateReply msgReply;
		msgReply.roomID = roomID;
		msgReply.onlineCount = 1;
		msgReply.totalCount = ROOM_MEMBER_MAX_NUM;
		pClient->SendData((MsgHead*)&msgReply);
	}

	void JoinRoom(ClientObject* pClient, MsgHead* pMsgHead) {
		MsgJoin* join = (MsgJoin*) pMsgHead;
		auto iter = m_room.find(join->roomID);
		if (iter != m_room.end()) {
			iter->second.addMember(pClient);
		}
		showRoom(pClient, pMsgHead);
	}

	void Talking(ClientObject* pClient, MsgHead* pMsgHead) {
		MsgTalk* talk = (MsgTalk*)pMsgHead;
		auto iter = m_room.find(talk->roomID);
		if (iter != m_room.end()) {
			talk->ID = pClient->getSocket();
			iter->second.sendToAll(pClient, pMsgHead);
		}
	}

	void LeaveRoom(ClientObject* pClient, MsgHead* pMsgHead) {
		MsgLeave* leave = (MsgLeave*)pMsgHead;
		auto iter = m_room.find(leave->roomID);
		if (iter != m_room.end()) {
			iter->second.deleteMember(pClient);
		}

	}

	void OnMsgProc(ClientObject* pClient, MsgHead* pMsgHead) {
		switch (pMsgHead->msgType)
		{
		case MSG_SHOW:
			printf("请求显示聊天室...\n");
			showRoom(pClient,pMsgHead);
			break;
		case MSG_JOIN:
		{
			MsgJoin* join = (MsgJoin*)pMsgHead;
			printf("加入 %d 聊天室...\n", join->roomID);
			JoinRoom(pClient, pMsgHead);
		}
			break;
		case MSG_CREATE:
			printf("请求创建聊天室...\n");
			CreateRoom(pClient, pMsgHead);
			break;
		case MSG_TALK:
		{
			MsgTalk* msgTalk = (MsgTalk*)pMsgHead;
			printf("%s...\n", msgTalk->getBuff());
			Talking(pClient, pMsgHead);
		}
			break;
		case MSG_LEAVE:
		{
			printf("离开聊天室...\n");
			LeaveRoom(pClient, pMsgHead);

		}
			break;
		default:
			printf("消息解析失败！...\n");
			break;
		}
	}
};

int main() {
	
	MyTcpServer tcpServer;
	tcpServer.Bind(12306);
	tcpServer.Listen();
	tcpServer.StartThread(4);

	//接收客户端连接/接收数据
	while (true) {	
		tcpServer.OnListen();

		//CreateThread(NULL, NULL, ThreadProc, (LPVOID) &sClient, NULL, NULL);
	}

	

	

	
	//closesocket(sListen);

	//getchar();
	while (1);

	return 0;
}