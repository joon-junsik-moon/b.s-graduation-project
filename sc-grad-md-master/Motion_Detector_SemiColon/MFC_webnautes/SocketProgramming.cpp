#include <stdio.h>
#include <winsock.h>

#pragma warning(disable:4996)
#pragma comment (lib, "ws2_32.lib") // Linker �� ���� �����Ϸ� �ɼ�
#define PORT 5000					// ���� ��Ʈ ��ȣ 5000 ��

int main()
{
	// ���� ���� �о ����ü�� ����

	FILE* fp;
	long fsize;

	if ((fp = fopen("read.jpg", "rb")) == NULL)
	{
		printf("read.jpg �� �����ϴ�.\n");
		return 1;

	}

	// ������ ���� ���

	fseek(fp, 0, SEEK_END);		// ��
	fsize = ftell(fp);			// ���� �������� ��ġ = EOF
	fseek(fp, 0, SEEK_SET);		// �ٽ� ó������ ������

	//printf("���� ���� ũ�� : %ld ����Ʈ\n", fsize);

	// ���� �ʱ�ȭ

	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Winsock ���� ����\n");
		fclose(fp);
		return 1;

	}

	// ���� ������ ���� �ּ� ����ü �� ���� ����

	int s, ns, addrsize;
	sockaddr_in client, server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Welcome ������ ����� ������ Ŭ���̾�Ʈ�� Bind ��Ŵ

	if (bind(s, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("bind() ����\n");
		WSACleanup();
		return 1;

	}

	// ��� ... �ִ� �ο��� 2��
	// Blocking ������� ����

	listen(s, 2);
	//printf("Ŭ���̾�Ʈ�� ��ٸ��ϴ�.\n");

	// Ŭ���̾�Ʈ�� ������ �� �κ��� ����
	// Bind �� Welcome ������ ���ο� ���Ͽ� �³����� Ŭ���̾�Ʈ�� ��� �õ�

	addrsize = sizeof(client);
	ns = accept(s, (sockaddr*)&client, &addrsize);

	if (ns == INVALID_SOCKET)
	{
		printf("accept() ����\n");
		WSACleanup();
		return 1;

	}

	// ������ �Ϸ�

	//printf("%s:%d���� ����\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	// ���� ũ�� ����

	int ret;
	char buff[1024] = { 0 };

	ltoa(fsize, buff, 10);
	ret = send(ns, buff, sizeof(char) * 1024, 0);

	if (ret == SOCKET_ERROR)
	{
		printf("Socket_Error : ������ ���� �ʾҽ��ϴ�.\n");
		fclose(fp);
		closesocket(s);
		closesocket(ns);

		return 1;

	}

	else

	{

		// ���� ����

		char* sendbuff;

		sendbuff = new char[fsize];
		fread(sendbuff, sizeof(char), fsize, fp);

		ret = send(ns, sendbuff, fsize, 0);

		// ���� ������ �������� Ŭ���̾�Ʈ���� ���� ũ�⸦ ����
		// Ȯ�ο�
		// ���� : �� �κ��� ���ٸ� Ŭ���̾�Ʈ�� ������ �� �ֱ� ���� ���μ����� ���� ���Ƿ�
		// �������� ������ �Ұ���, �ݺ��� ���Ѽ� ��⸦ �ϴ��� �߰����� ������ ���� �߿� ������
		// �� ������ ��.

		ret = recv(ns, buff, sizeof(buff), 0);

		if (fsize == atol(buff))
		{

			//printf("������ �Ϸ� �Ǿ����ϴ�.\n");


		}


		else
		{
			//printf("Socket_Error : ������ ���� �ʾҽ��ϴ�.\n");
		}


		// ������

		fclose(fp);
		closesocket(s);
		closesocket(ns);
		WSACleanup();
		delete sendbuff;

	}

	return 0;

}