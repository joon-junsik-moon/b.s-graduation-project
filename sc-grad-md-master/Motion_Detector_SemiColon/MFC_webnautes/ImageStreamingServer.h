#pragma once
#include <opencv2/core/core_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <winsock2.h>
#include <Windows.h>
#include "configs.h"
//#include "MFC_webnautesDlg.h"

#define TIMER_FRAME				1000/15		// 15FRMAE�� ms�� ȯ��
#define BUF_SIZE				1024		// �⺻ ����ũ��
#define IMGBUF_SIZE				1024*1024	// �̹����� �����ϱ� ���� �޸𸮿� ������ �� ����ϴ� ���� ũ��
#define DEFAULT_MODE			TRUE		// �׳� �̹����� ������ ��
#define REPLACE_MODE			FALSE		// �������� ������ �� 
#define MAX_NUM_CAMVIEW_THREAD	20			// �������� ���� thread�� �ִ� ����

using namespace std;
using namespace cv;

class ImageStreamingServer // ImageManager ���
{
public:
	ImageStreamingServer();
	virtual ~ImageStreamingServer();

private:
	//ImageManager* pIM;					// ImageManager ��ü
	CWinThread *mServerThread;			// ServerThread
	CWinThread* mClientThread;			// ClientThread
	CWinThread *mCacheThread;			// CacheThread
	vector<CWinThread*> mClientThreads;	// �������� �����ִ� client thread ���� ��Ƴ��� vector
	vector<SOCKET> mClientSockets;		// �������� �����ִ� socket ���� ��Ƴ��� vector
	SOCKET mServerThreadSock;			// ServerThread�� ����
	SOCKET mClientThreadSock;			// ClientThread�� ����
	SOCKADDR_IN mClientAddr;			// Client�� �ּ�

	UINT mPort = 0;							// ��Ʈ��ȣ
	char mHostName[100] = { NULL, };				// ȣ��Ʈ�̸�
	char buf[BUF_SIZE] = { NULL, };					// �����͸� ���� ����
	TCHAR *mHostIp = NULL;						// ȣ��Ʈ ip

private:
	static UINT Server_Thread(LPVOID arg);					// ServerThread
	static UINT Client_Thread(LPVOID arg);					// ClientThread
	static UINT Cache_Thread(LPVOID arg);					// CacheThread
	void sendImageHeader(SOCKET client_sock, BOOL mode);	// ��Ŷ ����� ����
	void sendImage(SOCKET client_sock, IplImage *sendImage, BOOL mode);	// �̹����� ���̳ʸ� ���·� ����
	void sendResizedImage(SOCKET client_sock, char *fileName, int width, int height);	// �̹����� ���������ؼ� ����
	void sendMjpeg(SOCKET client_sock, IplImage *sendImage);	// ���������� ����
	void sendList(SOCKET client_sock);						// ����� �������� ����Ʈ ����
	CString getImagePath();									// ����� ������ path �˾Ƴ�

public:
	BOOL Start(UINT port);			// �ش� port�� ����
	BOOL Start();					// Default ����(8080 port)
	void Stop();					// ����
};