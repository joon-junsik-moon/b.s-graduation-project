#include "Stdafx.h"
#include "pch.h"
#include "ImageStreamingServer.h"

#pragma warning(disable:4996)
#pragma comment (lib, "ws2_32.lib") // Linker 를 위한 컴파일러 옵션

//extern IplImage* loadImage;

ImageStreamingServer::ImageStreamingServer()
{
	// 변수 초기화 
	mServerThread = mClientThread = mCacheThread = NULL;
	mClientThreadSock = NULL;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		AfxMessageBox(L"윈속 초기화 실패!!");
	}

	//호스트이름과 IP주소 알아오기
	struct hostent* lphost;
	struct in_addr in;
	gethostname(mHostName, 99);
	lphost = gethostbyname(mHostName);
	in.s_addr = ((struct in_addr*)lphost->h_addr)->s_addr;
	mHostIp = CA2W(inet_ntoa(in));
}

ImageStreamingServer::~ImageStreamingServer()
{
	// 서버가 켜진상태라면 Stop 메소드 실행
	extern bool isRunning;
	if (isRunning) {
		Stop();
	}

	// 윈속 사용 해제
	WSACleanup();
}


BOOL ImageStreamingServer::Start(UINT port)
{
	mPort = port;

	if (mServerThread == NULL && mCacheThread == NULL) {
		//Server Thread 가 실행되고 있지 않을 때
		mServerThread = AfxBeginThread(Server_Thread, (LPVOID)this);
		mCacheThread = AfxBeginThread(Cache_Thread, (LPVOID)this);
		return TRUE;
	}
	else {
		AfxMessageBox(L"Server Thread 가 이미 실행중입니다.");
		return FALSE;
	}
}

BOOL ImageStreamingServer::Start()
{
	// 인수 없이 시작하면 default 로 8080 port로 시작
	return Start(8080);
}

void ImageStreamingServer::Stop()
{
	if (mServerThread->m_hThread != NULL) {
		// Server Thread 종료
		TerminateThread(mServerThread->m_hThread, NULL);
		mServerThread = NULL;
	}

	if (mCacheThread->m_hThread != NULL) {
		// Cache Thread 종료
		TerminateThread(mCacheThread->m_hThread, NULL);
		mCacheThread = NULL;
	}

	for (unsigned int i = 0; i < mClientSockets.size(); i++) {
		// Client Socket 종료
		closesocket(mClientSockets[i]);
		Sleep(10);
	}

	for (unsigned int i = 0; i < mClientThreads.size(); i++) {
		if (mClientThreads[i]->m_hThread != NULL) {
			// Client Thread 종료
			TerminateThread(mClientThreads[i]->m_hThread, NULL);
			mClientThreads[i] = NULL;
		}
	}

	// 서버 종료 및 소켓 제거
	mServerThread = NULL;
	closesocket(mServerThreadSock);

	// vector들 clear
	mClientThreads.clear();
	mClientSockets.clear();
}

UINT ImageStreamingServer::Server_Thread(LPVOID arg)
{
	ImageStreamingServer* sThread = (ImageStreamingServer*)arg;

	// socket()
	int option = 1;
	SOCKET server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (LPSTR)&option, sizeof(option)); // 소켓 옵션설정
	sThread->mServerThreadSock = server_sock;

	if (server_sock == INVALID_SOCKET)
	{
		AfxMessageBox(L"socket() 에러");
		return 0;
	}

	// bind()
	SOCKADDR_IN serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(sThread->mPort);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(server_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) {
		AfxMessageBox(L"bind() 에러");
		return 0;
	}

	// listen()
	if (listen(server_sock, SOMAXCONN) == SOCKET_ERROR) {
		AfxMessageBox(L"listen() 에러");
		return 0;
	}

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	int addrlen;
	addrlen = sizeof(client_addr);

	while (1)
	{
		while (sThread->mClientThreadSock != NULL)	//자식스레드에서 소켓값을 처리할때까지 기다린다.
			Sleep(50);

		// accept()
		client_sock = accept(server_sock, (SOCKADDR*)&client_addr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			AfxMessageBox(L"accept() 에러");
			return 0;
		}

		sThread->mClientThreadSock = client_sock;
		sThread->mClientAddr = client_addr;
		// Client_Thread 실행
		sThread->mClientThread = AfxBeginThread(Client_Thread, (LPVOID)sThread);
	}

	return 0;
}

UINT ImageStreamingServer::Client_Thread(LPVOID arg)
{
	ImageStreamingServer* sThread = (ImageStreamingServer*)arg;
	SOCKET client_sock;
	SOCKADDR_IN client_addr;
	CWinThread* clientThread;

	int retval;

	client_sock = sThread->mClientThreadSock;	//서버 스레드로부터 방금 접속한 Client의 정보를 얻어옴
	client_addr = sThread->mClientAddr;
	sThread->mClientThreadSock = NULL;
	clientThread = sThread->mClientThread;

	// client의 GET 요청 받기
	if ((retval = recv(client_sock, (LPSTR)sThread->buf, BUF_SIZE, 0)) == SOCKET_ERROR) {
		//AfxMessageBox(L"recv() 에러");
		return 0;
	}

	if (strncmp(sThread->buf, "GET /?method=camView", 20) == 0) {
		// camView만 vector에 저장한다.
		extern IplImage* loadImage;
		sThread->mClientThreads.push_back(clientThread);
		sThread->mClientSockets.push_back(client_sock);
		sThread->sendMjpeg(client_sock, loadImage);
	}
	else if (strncmp(sThread->buf, "GET /?method=listView", 21) == 0) {
		// 사진 리스트를 보냄
		sThread->sendList(client_sock);
		//CString pscp = _T("php C:\\Bitnami\\wampstack-7.3.18-1\\apache2\\htdocs\\listView.php");
		//CStringA strA(pscp); // a helper string
		//LPCSTR ptr = strA;
		//WinExec(ptr, SW_SHOW);
	}
	else if (strncmp(sThread->buf, "GET /?method=imageView&name=", 28) == 0) {
		CString parameters, name, width, height;
		CString name_val, width_val, height_val;

		// name 뒤의 내용들을 간단하게 파싱해서 변수에 저장
		AfxExtractSubString(parameters, CA2T(sThread->buf), 1, ' ');

		AfxExtractSubString(name, parameters, 1, '&');
		AfxExtractSubString(width, parameters, 2, '&');
		AfxExtractSubString(height, parameters, 3, '&');

		AfxExtractSubString(name_val, name, 1, '=');
		AfxExtractSubString(width_val, width, 1, '=');
		AfxExtractSubString(height_val, height, 1, '=');

		// name 을 이상한걸로 줬을 때 처리...
		/*if(fileName.) {

		}*/

		if (width_val == "" || height_val == "") {
			// width 와 height가 없을땐 default로 640 x 480
			sThread->sendResizedImage(client_sock, CT2A(name_val), 640, 480);
		}
		else {
			sThread->sendResizedImage(client_sock, CT2A(name_val), _ttoi(width_val), _ttoi(height_val));
		}
		//CString pscp = _T("php C:\\Bitnami\\wampstack-7.3.18-1\\apache2\\htdocs\\imageView.php ");
		//pscp += name_val;
		//CStringA strA(pscp); // a helper string
		//LPCSTR ptr = strA;
		//WinExec(ptr, SW_SHOW);
		//sThread->sendResizedImage(client_sock, CT2A(name_val), 1024, 768);
	}
	else {
	}

	// 클라이언트 종료 및 소켓 제거
	sThread->mClientThread = NULL;
	closesocket(client_sock);

	return 0;
}

// camView Thread가 특정 수 이상 생기면 가장 오래전에 사용한 Thread를 죽인다.
UINT ImageStreamingServer::Cache_Thread(LPVOID arg)
{
	ImageStreamingServer* sThread = (ImageStreamingServer*)arg;
	CWinThread* clientThread = NULL;
	SOCKET client_sock;

	// Server가 Stop() 메소드를 부르기 전까지 
	while (1) {
		if (sThread->mClientThreads.size() >= MAX_NUM_CAMVIEW_THREAD) {
			// 가장 오래된(가장 앞에 있는) Thread와 socket을 참조한다.
			clientThread = sThread->mClientThreads.front();
			client_sock = sThread->mClientSockets.front();

			if (clientThread->m_hThread != NULL) {
				// Thread 와 socket을 각각 종료 시키고 vector에서 뺀다.
				TerminateThread(clientThread->m_hThread, NULL);
				clientThread = NULL;
				sThread->mClientThreads.erase(sThread->mClientThreads.begin());

				closesocket(client_sock);
				sThread->mClientSockets.erase(sThread->mClientSockets.begin());
			}
		}
		Sleep(1000);
	}

	return 0;
}

void ImageStreamingServer::sendImageHeader(SOCKET client_sock, BOOL mode)
{
	char buf[BUF_SIZE];

	if (mode) {
		// 이미지 헤더 전송
		sprintf_s(buf, BUF_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n");
		send(client_sock, buf, strlen(buf), 0);
	}
	else {
		// 이미지 헤더 전송(replace)
		sprintf_s(buf, BUF_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: multipart/x-mixed-replace; boundary=--boundary\r\n");
		send(client_sock, buf, strlen(buf), 0);
	}
}

void ImageStreamingServer::sendImage(SOCKET client_sock, IplImage* sendImage, BOOL mode)
{
	// 메모리에서 IplImage 를 jpeg stream 으로 변환
	vector<int> params(2);
	vector<uchar> imgbuf(sendImage->imageSize);
	/*params.push_back(IMWRITE_JPEG_QUALITY);
	params.push_back(95);*/
	params[0] = IMWRITE_JPEG_QUALITY;
	params[1] = 95;
	Mat imgOriginal;
	imgOriginal = cvarrToMat(sendImage);
	imencode(".jpg", imgOriginal, imgbuf, params);


	// jpg stream (vector<unsigned char>) 를 char* 로 변환
	unsigned char* pimgbuf = &imgbuf[0];

	if (mode) {
		// 일반 이미지 전송 모드
		sprintf_s(buf, BUF_SIZE, "Content-Length: %d\r\n\r\n", imgbuf.size());
		send(client_sock, buf, strlen(buf), 0);
		send(client_sock, (char*)pimgbuf, imgbuf.size(), 0);

		sprintf_s(buf, BUF_SIZE, "\r\n");
		send(client_sock, buf, strlen(buf), 0);
	}
	else {
		// 동영상 이미지 전송 모드
		sprintf_s(buf, BUF_SIZE, "\r\n--boundary\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", imgbuf.size());
		send(client_sock, buf, strlen(buf), 0);
		send(client_sock, (char*)pimgbuf, imgbuf.size(), 0);

		sprintf_s(buf, BUF_SIZE, "\r\n");
		send(client_sock, buf, strlen(buf), 0);
	}
}

void ImageStreamingServer::sendResizedImage(SOCKET client_sock, char* fileName, int width, int height)
{
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       sbuf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(sbuf, sizeof(sbuf), "%Y%m%d", &tstruct); // YYYYMMDD 형태의 스트링
	cbuf = LPSTR(sbuf);

	CString strFullPath = getImagePath() + "\\" + cbuf + "\\" + fileName;
	//wcout << "fullpath: " << (const wchar_t*)strFullPath << endl;

	IplImage* resizedImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	//IplImage* loadImage = cvLoadImage(CT2A(strFullPath), CV_LOAD_IMAGE_UNCHANGED);

	string strpath = CT2CA(strFullPath);
	Mat tempimage = imread(strpath, IMREAD_UNCHANGED);

	//IplImage* loadImage = cvLoadImage(CT2A(strFullPath), -1);
	IplImage* loadImage = new IplImage(tempimage);

	//파일이 없는 경우
	if (loadImage == NULL) {
		return;
	}
	cvResize(loadImage, resizedImage, CV_INTER_LINEAR);

	// 일반 이미지 전송 모드로 전송
	sendImageHeader(client_sock, DEFAULT_MODE);
	sendImage(client_sock, &resizedImage[0], DEFAULT_MODE);

	///*if (loadImage != NULL) */cvReleaseImage(&loadImage);
	///*if (resizedImage != NULL) */cvReleaseImage(&resizedImage);
}

void ImageStreamingServer::sendMjpeg(SOCKET client_sock, IplImage* sendImage)
{
	// 동영상 이미지 전송 모드로 전송
	sendImageHeader(client_sock, REPLACE_MODE);

	while (1) {
		this->sendImage(client_sock, sendImage, REPLACE_MODE);

		// 15 프레임으로 보냄
		Sleep(1000 / 15);
	}
}

void ImageStreamingServer::sendList(SOCKET client_sock)
{
	char buf[BUF_SIZE];

	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       sbuf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(sbuf, sizeof(sbuf), "%Y%m%d", &tstruct); // YYYYMMDD 형태의 스트링
	cbuf = LPSTR(sbuf);

	// 캡쳐 파일 리스트 보내기
	CFileFind finder;
	CString strWildCard(getImagePath());
	strWildCard += "\\";
	strWildCard += cbuf;
	strWildCard += "\\cam-*.jpg";

	//wcout << "폴더: " << (const wchar_t*)strWildCard << endl;

	CString sendString = _T("");
	vector<CString> sendList;

	BOOL bWorking = finder.FindFile(strWildCard);
	if (!bWorking)
		sendString = "empty";

	while (bWorking) {
		bWorking = finder.FindNextFile();

		if (finder.IsDots() || finder.IsDirectory())
			continue;
		else {
			sendList.push_back(finder.GetFileName());
			//CString getfname = finder.GetFileName();
			//wcout << "전송 파일: " << (const wchar_t*)getfname << endl;
			//sendList.push_back(getfname);
		}
	}
	finder.Close();

	reverse(sendList.begin(), sendList.end());

	for (unsigned int i = 0; i < sendList.size(); i++) {
		//wcout << "파일: " << i << (const wchar_t*)sendString << endl;
		sendString = sendString + ";" + sendList[i];
	}
	sendString.Delete(0, 9);
	//wcout << "최종파일: " << (const wchar_t*)sendString << endl;
	//sendString = "empty";
	// 리스트를 string 으로 보냄
	sprintf_s(buf, BUF_SIZE, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n", strlen(CT2A(sendString)));
	//cout << buf << endl;
	send(client_sock, buf, strlen(buf), 0);
	send(client_sock, CT2A(sendString), strlen(CT2A(sendString)), 0);
	//cout << buf << endl;

	sprintf_s(buf, BUF_SIZE, "\r\n");
	send(client_sock, buf, strlen(buf), 0);
}

CString ImageStreamingServer::getImagePath()
{
	CString	mPath;
	TCHAR iniPath[MAX_PATH];
	TCHAR imagePath[MAX_PATH];

	// iniPath 얻기
	GetCurrentDirectory(MAX_PATH, iniPath);
	wcscat_s(iniPath, L"\\semiMD.ini");

	// imagePath 얻기
	GetPrivateProfileString(L"MotionDetector", L"CapPath", L"C:\\", imagePath, MAX_PATH, iniPath);
	mPath.Append(imagePath);

	return mPath;
}