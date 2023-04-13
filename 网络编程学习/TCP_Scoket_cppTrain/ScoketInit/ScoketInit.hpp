#ifndef _SOCKET_INIT_H_
#define _SOCKET_INIT_H_

#include<stdio.h>
#include<WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

class ScoketInit {
public:
	ScoketInit() {
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wasData;
		if (WSAStartup(sockVersion, &wasData) != 0) {
			printf("∂ØÃ¨¡¥Ω”ø‚º”‘ÿ ß∞‹£°\n");
		}
	}

	~ScoketInit() {
		WSACleanup();
	}
};

#endif // !_SOCKET_INIT_H_
