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

#define TIMER_FRAME				1000/15		// 15FRMAE을 ms로 환산
#define BUF_SIZE				1024		// 기본 버퍼크기
#define IMGBUF_SIZE				1024*1024	// 이미지를 전송하기 위해 메모리에 저장할 때 사용하는 버퍼 크기
#define DEFAULT_MODE			TRUE		// 그냥 이미지를 전송할 때
#define REPLACE_MODE			FALSE		// 동영상을 전송할 때 
#define MAX_NUM_CAMVIEW_THREAD	20			// 동영상을 보는 thread의 최대 갯수

using namespace std;
using namespace cv;

class ImageStreamingServer // ImageManager 상속
{
public:
	ImageStreamingServer();
	virtual ~ImageStreamingServer();

private:
	//ImageManager* pIM;					// ImageManager 객체
	CWinThread *mServerThread;			// ServerThread
	CWinThread* mClientThread;			// ClientThread
	CWinThread *mCacheThread;			// CacheThread
	vector<CWinThread*> mClientThreads;	// 동영상을 보여주는 client thread 들을 모아놓은 vector
	vector<SOCKET> mClientSockets;		// 동영상을 보여주는 socket 들을 모아놓은 vector
	SOCKET mServerThreadSock;			// ServerThread의 소켓
	SOCKET mClientThreadSock;			// ClientThread의 소켓
	SOCKADDR_IN mClientAddr;			// Client의 주소

	UINT mPort = 0;							// 포트번호
	char mHostName[100] = { NULL, };				// 호스트이름
	char buf[BUF_SIZE] = { NULL, };					// 데이터를 받을 버퍼
	TCHAR *mHostIp = NULL;						// 호스트 ip

private:
	static UINT Server_Thread(LPVOID arg);					// ServerThread
	static UINT Client_Thread(LPVOID arg);					// ClientThread
	static UINT Cache_Thread(LPVOID arg);					// CacheThread
	void sendImageHeader(SOCKET client_sock, BOOL mode);	// 페킷 헤더를 전송
	void sendImage(SOCKET client_sock, IplImage *sendImage, BOOL mode);	// 이미지를 바이너리 형태로 전송
	void sendResizedImage(SOCKET client_sock, char *fileName, int width, int height);	// 이미지를 리사이즈해서 전송
	void sendMjpeg(SOCKET client_sock, IplImage *sendImage);	// 동영상으로 전송
	void sendList(SOCKET client_sock);						// 저장된 사진파일 리스트 전송
	CString getImagePath();									// 저장된 사진의 path 알아냄

public:
	BOOL Start(UINT port);			// 해당 port로 시작
	BOOL Start();					// Default 시작(8080 port)
	void Stop();					// 중지
};