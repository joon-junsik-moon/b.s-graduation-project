#include <stdio.h>
#include <winsock.h>

#pragma warning(disable:4996)
#pragma comment (lib, "ws2_32.lib") // Linker 를 위한 컴파일러 옵션
#define PORT 5000					// 서버 포트 번호 5000 번

int main()
{
	// 보낼 파일 읽어서 구조체로 저장

	FILE* fp;
	long fsize;

	if ((fp = fopen("read.jpg", "rb")) == NULL)
	{
		printf("read.jpg 이 없습니다.\n");
		return 1;

	}

	// 파일의 길이 계산

	fseek(fp, 0, SEEK_END);		// 끝
	fsize = ftell(fp);			// 현재 포인터의 위치 = EOF
	fseek(fp, 0, SEEK_SET);		// 다시 처음으로 돌리기

	//printf("보낼 파일 크기 : %ld 바이트\n", fsize);

	// 윈속 초기화

	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Winsock 생성 오류\n");
		fclose(fp);
		return 1;

	}

	// 서버 생성을 위한 주소 구조체 및 소켓 생성

	int s, ns, addrsize;
	sockaddr_in client, server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Welcome 소켓을 만들어 들어오는 클라이언트와 Bind 시킴

	if (bind(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind() 오류\n");
		WSACleanup();
		return 1;

	}

	// 대기 ... 최대 인원수 2명
	// Blocking 방식으로 받음

	listen(s, 2);
	//printf("클라이언트를 기다립니다.\n");

	// 클라이언트가 왔을때 이 부분이 시작
	// Bind 된 Welcome 소켓을 새로운 소켓에 승낙시켜 클라이언트와 통신 시도

	addrsize = sizeof(client);
	ns = accept(s, (sockaddr*)&client, &addrsize);

	if (ns == INVALID_SOCKET)
	{
		printf("accept() 오류\n");
		WSACleanup();
		return 1;

	}

	// 연결이 완료

	//printf("%s:%d에서 접속\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	// 파일 크기 전송

	int ret;
	char buff[1024] = { 0 };

	ltoa(fsize, buff, 10);
	ret = send(ns, buff, sizeof(char) * 1024, 0);

	if (ret == SOCKET_ERROR)
	{
		printf("Socket_Error : 전송이 되지 않았습니다.\n");
		fclose(fp);
		closesocket(s);
		closesocket(ns);

		return 1;

	}

	else

	{

		// 파일 전송

		char* sendbuff;

		sendbuff = new char[fsize];
		fread(sendbuff, sizeof(char), fsize, fp);

		ret = send(ns, sendbuff, fsize, 0);

		// 파일 전송이 끝났으면 클라이언트에서 파일 크기를 보냄
		// 확인용
		// 주의 : 이 부분이 없다면 클라이언트로 파일을 다 주기 전에 프로세스가 끝이 나므로
		// 정상적인 전송이 불가능, 반복을 시켜서 대기를 하던지 추가적인 전송을 통해 중요 파일은
		// 다 보내야 함.

		ret = recv(ns, buff, sizeof(buff), 0);

		if (fsize == atol(buff))
		{

			//printf("전송이 완료 되었습니다.\n");


		}


		else
		{
			//printf("Socket_Error : 전송이 되지 않았습니다.\n");
		}


		// 마무리

		fclose(fp);
		closesocket(s);
		closesocket(ns);
		WSACleanup();
		delete sendbuff;

	}

	return 0;

}