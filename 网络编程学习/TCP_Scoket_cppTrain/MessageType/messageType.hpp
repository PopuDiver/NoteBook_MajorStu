#pragma once
#define PACKET_MAX_SIZE 1024
#define ROOM_MAX_NUM 100
//����ʹ�ú궨����ʵ��
//#define MSG_SHOW 1

enum MSG_TYPE {
	MSG_SHOW,
	MSG_SHOW_REPLY,
	MSG_JOIN,
	MSG_CREATE,
	MSG_LEAVE,
	MSG_TALK,
	MSG_CREATE_REPLY
};

/// <summary>
/// ��Ϣ����
/// </summary>
class MsgHead {
public:
	int msgType;//��Ϣ����
	int dataLen;//��Ϣ����
};

/// <summary>
/// ������ʾ
/// </summary>
class MsgShow :public MsgHead {
public:
	MsgShow() {
		msgType = MSG_SHOW;
		dataLen = sizeof(MsgShow);
	}
};

/// <summary>
/// ��Ϣ�ظ�
/// </summary>
class MsgShowReply :public MsgHead {
public:
	int roomCount;
	struct  RoomInfo
	{
		int roomID;
		int onlineNum;
		int totalNum;
	};
	RoomInfo rooms[ROOM_MAX_NUM];
	MsgShowReply() {
		msgType = MSG_SHOW_REPLY;
		dataLen = sizeof(MsgShowReply);
	}
};

/// <summary>
/// ����������
/// </summary>
class MsgJoin :public MsgHead {
public:
	int roomID = 0;
	MsgJoin(int r) {
		msgType = MSG_JOIN;
		dataLen = sizeof(MsgJoin);
		roomID = r;
	}
};

/// <summary>
/// ����������
/// </summary>
class MsgCreate :public MsgHead {
public:
	MsgCreate() {
		msgType = MSG_CREATE;
		dataLen = sizeof(MsgCreate);
	}
};

/// <summary>
/// �뿪������
/// </summary>
class MsgLeave :public MsgHead {
public:
	int roomID;
	MsgLeave() {
		msgType = MSG_LEAVE;
		dataLen = sizeof(MsgLeave);
	}
};


/// <summary>
/// ����
/// </summary>
class MsgTalk :public MsgHead {
private:
	
	char buff[1000];
public:
	int roomID;
	int ID;
	MsgTalk(int roomID) {
		this->roomID = roomID;
		msgType = MSG_TALK;
		dataLen = sizeof(MsgTalk);
		ID = 0;
	}
	char* getBuff() {
		return buff;
	}
};

/// <summary>
/// �����ظ�
/// </summary>
class MsgCreateReply :public MsgHead {
public:
	int roomID;
	int onlineCount;
	int totalCount;
	MsgCreateReply() {
		msgType = MSG_CREATE_REPLY;
		dataLen = sizeof(MsgCreateReply);
	}
};