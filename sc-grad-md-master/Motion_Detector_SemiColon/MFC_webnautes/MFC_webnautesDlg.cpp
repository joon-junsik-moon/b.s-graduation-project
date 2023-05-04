// MFC_webnautesDlg.cpp: 구현 파일
#pragma warning(disable:4996)

#include "pch.h"
#include "framework.h"
#include "MFC_webnautes.h"
#include "MFC_webnautesDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "MD_CONFIG.h"
#include "MD_Calendar.h"
#include <atlimage.h>
#include "stdafx.h"
#include "SendToNAS.h"

using namespace cv;
using namespace std;

//전역변수 선언부
bool isImageClick = true; //서브 영상 화면으로 전환, 메인 영상으로 돌아오게 하기 위한 변수
VideoCapture* capture; //웹캠으로부터 이미지를 받아오기 위한 변수
vector<vector<Point> > cnts; //모션이 감지됨을 확인하기 위한 프레임 픽셀 차이 저장 변수
//이미지 저장 변수
Mat frame, gray, frameDelta, thresh, firstFrame, mat_frame;
CString CapPath; //이미지 저장 폴더
CString selFolderDate = _T(""); //folder 이름을 저장하기 위한 변수
CString FolderPath = _T("");  //"이미지 저장 폴더\\하위 날짜 폴더" 경로를 저장하기 위한 변수
CImage cimage_mfc; //프로그램 화면에 이미지를 띄우기 위한 변수
CImage m_image; //임시 저장용 변수
int setContour = 5; //민감도의 Index
int insert = 0; //모션이 감지됨에 따라 해당 이미지를 저장할 것인지 판단하기 위한 변수
int sensor[10] = { 20000, 15000, 10000, 7000, 2500, 2000, 1600, 1200, 800, 400 }; //민감도
IplImage* loadImage; //HTTP 소켓 통신을 위한 이미지 저장 변수
char* curtime; //HTTP 소켓 통신을 위한 시간 저장 변수
bool isRunning = true; //프로그램이 실행 중인지 확인하기 위한 변수
//백업 서버의 정보를 저장하기 위한 4개의 변수
CString serverIP = _T("");
CString serverPath = _T("");
CString serverID = _T("");
CString serverPW = _T("");

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL isIniFile() {
	CString cBuf = _T("");
	GetPrivateProfileString(_T("MotionDetector"), _T("CapPath"), _T("-"), (LPWSTR)(LPCWSTR)cBuf, 256, _T(".\\semiMD.ini"));

	if (cBuf == "-") {
		return false;
	}
	else {
		return true;
	}
}

void iniWrite() {
	// Write 
	WritePrivateProfileString(_T("MotionDetector"), _T("CapPath"), _T("C:\\MD_Capture"), _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerIP"), _T(""), _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerPath"), _T(""), _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerID"), _T(""), _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerPW"), _T(""), _T(".\\semiMD.ini"));
	AfxMessageBox(_T("고급 설정에서 백업 서버 정보를 입력해주세요."));
}

void iniRead() {
	// Read 
	CString Path = _T("");
	GetPrivateProfileString(_T("MotionDetector"), _T("CapPath"), _T("-"), (LPWSTR)(LPCWSTR)Path, 256, _T(".\\semiMD.ini"));
	CapPath.Format(_T("%s"), static_cast<LPCTSTR>(Path));

	CString iniServerIP = _T("");
	GetPrivateProfileString(_T("ServerInfo"), _T("ServerIP"), _T("-"), (LPWSTR)(LPCWSTR)iniServerIP, 256, _T(".\\semiMD.ini"));
	serverIP.Format(_T("%s"), static_cast<LPCTSTR>(iniServerIP));

	CString iniServerPath = _T("");
	GetPrivateProfileString(_T("ServerInfo"), _T("ServerPath"), _T("-"), (LPWSTR)(LPCWSTR)iniServerPath, 256, _T(".\\semiMD.ini"));
	serverPath.Format(_T("%s"), static_cast<LPCTSTR>(iniServerPath));

	CString iniServerID = _T("");
	GetPrivateProfileString(_T("ServerInfo"), _T("ServerID"), _T("-"), (LPWSTR)(LPCWSTR)iniServerID, 256, _T(".\\semiMD.ini"));
	serverID.Format(_T("%s"), static_cast<LPCTSTR>(iniServerID));

	CString iniServerPW = _T("");
	GetPrivateProfileString(_T("ServerInfo"), _T("ServerPW"), _T("-"), (LPWSTR)(LPCWSTR)iniServerPW, 256, _T(".\\semiMD.ini"));
	serverPW.Format(_T("%s"), static_cast<LPCTSTR>(iniServerPW));
}

BOOL myUtil_CheckDir(CString sDirName)
{
	CFileFind file;
	CString strFile = _T("*.*");
	BOOL bResult = file.FindFile(sDirName + strFile);

	if (!bResult)
	{
		bResult = CreateDirectory(sDirName + "\\", NULL);
		if (!bResult)
		{
			/*Error*/
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

char* curTime() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%H:%M:%S", &tstruct); // HH:mm:ss 형태의 스트링
	return buf;
}

CString currentDateTime() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tstruct); // YYYYMMDD-HHmmss 형태의 스트링
	cbuf = LPSTR(buf);
	return cbuf;
}

CString sendFileDate() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m", &tstruct); // YYYYMM 형태의 스트링
	cbuf = LPSTR(buf);
	int number = _ttoi(cbuf);
	cbuf.Format(_T("%d"), number - 1);
	return cbuf;
}

CString CMFCwebnautesDlg::currentDate() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d", &tstruct); // YYYYMMDD 형태의 스트링
	cbuf = LPSTR(buf);
	return cbuf;
}

void CMFCwebnautesDlg::capPicture(Mat image) {
	if (insert == 0) {
		CString tempDate = currentDate();
		CString curDateTime = currentDateTime();
		curDateTime.Delete(0, 2);
		CString tempfile = CapPath + "\\" + tempDate + "\\cam-" + curDateTime + ".jpg";
		string savefile = CT2CA(tempfile);
		CString tempDir = CapPath + "\\" + tempDate;
		myUtil_CheckDir(tempDir);
		Mat resize;
		cv::resize(image, resize, cv::Size(1024, 768));
		imwrite(savefile, resize);        // img를 파일로 저장한다.
		insert++;
		ShowThumbnailList();
	}
	else if (insert > 50) {
		CString tempDate = currentDate();
		CString curDateTime = currentDateTime();
		curDateTime.Delete(0, 2);
		CString tempfile = CapPath + "\\" + tempDate + "\\cam-" + curDateTime + ".jpg";
		string savefile = CT2CA(tempfile);
		CString tempDir = CapPath + "\\" + tempDate;
		myUtil_CheckDir(tempDir);
		Mat resize;
		cv::resize(image, resize, cv::Size(1024, 768));
		imwrite(savefile, resize);        // img를 파일로 저장한다.
		insert = 0;
		ShowThumbnailList();
	}
	else {
		insert++;
	}

}

void CMFCwebnautesDlg::NowImage(Mat image) {
	if (PathFileExists(CapPath + "\\" + "cam-*.jpg"))
		DeleteFile(CapPath + "\\" + "cam-*.jpg");

	CString tempfile = CapPath + "\\image.jpg";
	string savefile = CT2CA(tempfile);
	Mat resize;
	cv::resize(image, resize, cv::Size(1024, 768));
	imwrite(savefile, resize); // img를 파일로 저장한다.
}

void CMFCwebnautesDlg::Running() {
	//Motion Detection 부분
	int v = 0;

	capture->read(frame);

	//첫 프레임과 현재 프레임의 차이 계산
	cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
	GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);

	while (capture->read(frame)) {

		Mat sendNowImage = frame;
		curtime = curTime();
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 1);
		loadImage = new IplImage(sendNowImage);
		cvPutText(loadImage, curtime, cvPoint(4, loadImage->height - 10), &font, CV_RGB(255, 255, 255));

		//grayscale로 변환
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		//전 프레임과 현재 프레임의 차이 계산
		absdiff(firstFrame, gray, frameDelta);
		threshold(frameDelta, thresh, 25, 255, THRESH_BINARY);

		dilate(thresh, thresh, Mat(), Point(-1, -1), 2);
		findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < cnts.size(); i++) {
			if (contourArea(cnts[i]) < sensor[setContour]) {
				continue;
			}
			capPicture(frame);
		}

		Sleep(50);

		cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
		GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);

		if (waitKey(1) == 27) {
			//해당 if문이 없으면 에러가 떠서 아무 기능이 없지만 추가해둠.
		}

	}
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCwebnautesDlg 대화 상자

CMFCwebnautesDlg::CMFCwebnautesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_WEBNAUTES_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCwebnautesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
	DDX_Control(pDX, IDC_SLIDER1, m_sld);
	DDX_Control(pDX, IDCANCEL, m_list);
	DDX_Control(pDX, IDC_LIST1, m_piclist);
	DDX_Control(pDX, IDC_PICTURE2, m_picture2);
	DDX_Control(pDX, IDC_COMBO2, m_cb);
}

BEGIN_MESSAGE_MAP(CMFCwebnautesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_PICTURE, &CMFCwebnautesDlg::OnStnClickedPicture)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMFCwebnautesDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDOK, &CMFCwebnautesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCwebnautesDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCwebnautesDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &CMFCwebnautesDlg::OnEnChangeEdit1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CMFCwebnautesDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMFCwebnautesDlg::OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCwebnautesDlg::OnBnClickedButton3)
	ON_STN_CLICKED(IDC_PICTURE2, &CMFCwebnautesDlg::OnClickedPicture2)
	ON_STN_DBLCLK(IDC_PICTURE2, &CMFCwebnautesDlg::OnDblclkPicture2)
	ON_STN_DBLCLK(IDC_PICTURE, &CMFCwebnautesDlg::OnDblclkPicture)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCwebnautesDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CMFCwebnautesDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CMFCwebnautesDlg 메시지 처리기

BOOL CMFCwebnautesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//카메라 연결
	capture = new VideoCapture(1);
	if (!capture->isOpened())
	{
		capture = new VideoCapture(0);
		if (!capture->isOpened()) {
			isRunning = false;
			MessageBox(_T("캠을 열수 없습니다. \n"));
		}
	}
	capture->read(frame);
	Mat sendNowImage = frame;
	loadImage = new IplImage(sendNowImage);

	//ini file 존재 여부 확인 및 열기 또는 쓰기
	if (isIniFile()) {
		iniRead();
	}
	else {
		iniWrite();
		iniRead();
	}

	// 감지 등급 Slide Control 설정
	m_sld.SetRange(1, 10); // 최소 최대 값
	m_sld.SetPos(5); // 현재 위치
	m_sld.SetLineSize(1); // 방향키로 움직일 때 사이즈
	m_sld.SetPageSize(1); // PgUp, PgDn으로 움직일 때 사이즈

	//감지 등급 Combo Box 설정
	int iPos = m_sld.GetPos(); 
	m_cb.SetCurSel(iPos-1);
	m_cb.AddString(TEXT("1"));
	m_cb.AddString(TEXT("2"));
	m_cb.AddString(TEXT("3"));
	m_cb.AddString(TEXT("4"));
	m_cb.AddString(TEXT("5"));
	m_cb.AddString(TEXT("6"));
	m_cb.AddString(TEXT("7"));
	m_cb.AddString(TEXT("8"));
	m_cb.AddString(TEXT("9"));
	m_cb.AddString(TEXT("10"));

	//이미지 저장 폴더 있는지 확인
	myUtil_CheckDir(CapPath);

	//폴더 선택 저장 변수
	FolderPath = CapPath;
	FolderPath += "\\";
	FolderPath += currentDate();

	//// ImagerStreamingServer 시작
	sThread = new ImageStreamingServer();
	sThread->Start();

	//파일 보내는 Thread 실행
	nas = new SendToNAS();
	if (nas->START()) {
		m_nTimeCount = 0;
		SetTimer(1400, 60000, NULL);
	}

	//imagelist 생성 및 list control과 연결
	//이미지리스트 크기 변경하려고 64에서 128으로 변경
	mImageList.Create(192, 144, ILC_COLOR8 | ILC_MASK, 8, 8);
	m_piclist.SetImageList(&mImageList, LVSIL_NORMAL);
	ShowThumbnailList();

	m_DataView = FALSE;
	m_CalView = FALSE;
	
	SetTimer(1000, 1000 / 15, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCwebnautesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_CLOSE)
	{
		//종료버튼 눌릴 시
		capture->release();
		destroyAllWindows();
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_APP_EXIT, NULL);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCwebnautesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCwebnautesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCwebnautesDlg::OnStnClickedPicture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


}


void CMFCwebnautesDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CMFCwebnautesDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1000:

		if (isImageClick) {
			// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
			//mat_frame가 입력 이미지입니다.
			capture->read(mat_frame);


			//이곳에 OpenCV 함수들을 적용합니다.

			//화면에 보여주기 위한 처리입니다.
			int bpp = 8 * mat_frame.elemSize();
			assert((bpp == 8 || bpp == 24 || bpp == 32));

			int padding = 0;
			//32 bit image is always DWORD aligned because each pixel requires 4 bytes
			if (bpp < 32)
				padding = 4 - (mat_frame.cols % 4);

			if (padding == 4)
				padding = 0;

			int border = 0;
			//32 bit image is always DWORD aligned because each pixel requires 4 bytes
			if (bpp < 32)
			{
				border = 4 - (mat_frame.cols % 4);
			}

			Mat mat_temp;
			if (border > 0 || mat_frame.isContinuous() == false)
			{
				// Adding needed columns on the right (max 3 px)
				cv::copyMakeBorder(mat_frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
			}
			else
			{
				mat_temp = mat_frame;
			}

			RECT r;
			m_picture.GetClientRect(&r);
			cv::Size winSize(r.right, r.bottom);

			cimage_mfc.Create(winSize.width, winSize.height, 24);

			BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
			bitInfo->bmiHeader.biBitCount = bpp;
			bitInfo->bmiHeader.biWidth = mat_temp.cols;
			bitInfo->bmiHeader.biHeight = -mat_temp.rows;
			bitInfo->bmiHeader.biPlanes = 1;
			bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitInfo->bmiHeader.biCompression = BI_RGB;
			bitInfo->bmiHeader.biClrImportant = 0;
			bitInfo->bmiHeader.biClrUsed = 0;
			bitInfo->bmiHeader.biSizeImage = 0;
			bitInfo->bmiHeader.biXPelsPerMeter = 0;
			bitInfo->bmiHeader.biYPelsPerMeter = 0;

			//그레이스케일 인경우 팔레트가 필요
			if (bpp == 8)
			{
				RGBQUAD* palette = bitInfo->bmiColors;
				for (int i = 0; i < 256; i++)
				{
					palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
					palette[i].rgbReserved = 0;
				}
			}

			// Image is bigger or smaller than into destination rectangle
			// we use stretch in full rect

			if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
			{
				// source and destination have same size
				// transfer memory block
				// NOTE: the padding border will be shown here. Anyway it will be max 3px width

				SetDIBitsToDevice(cimage_mfc.GetDC(),
					//destination rectangle
					0, 0, winSize.width, winSize.height,
					0, 0, 0, mat_temp.rows,
					mat_temp.data, bitInfo, DIB_RGB_COLORS);
			}
			else
			{
				// destination rectangle
				int destx = 0, desty = 0;
				int destw = winSize.width;
				int desth = winSize.height;

				// rectangle defined on source bitmap
				// using imgWidth instead of mat_temp.cols will ignore the padding border
				int imgx = 0, imgy = 0;
				int imgWidth = mat_temp.cols - border;
				int imgHeight = mat_temp.rows;

				StretchDIBits(cimage_mfc.GetDC(),
					destx, desty, destw, desth,
					imgx, imgy, imgWidth, imgHeight,
					mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
			}

			HDC dc = ::GetDC(m_picture.m_hWnd);

			cimage_mfc.BitBlt(dc, 0, 0);

			::ReleaseDC(m_picture.m_hWnd, dc);

			cimage_mfc.ReleaseDC();
			cimage_mfc.Destroy();

			CDialogEx::OnTimer(nIDEvent);
		}
		else {
			// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
			//mat_frame가 입력 이미지입니다.
			capture->read(mat_frame);


			//이곳에 OpenCV 함수들을 적용합니다.

			//화면에 보여주기 위한 처리입니다.
			int bpp = 8 * mat_frame.elemSize();
			assert((bpp == 8 || bpp == 24 || bpp == 32));

			int padding = 0;
			//32 bit image is always DWORD aligned because each pixel requires 4 bytes
			if (bpp < 32)
				padding = 4 - (mat_frame.cols % 4);

			if (padding == 4)
				padding = 0;

			int border = 0;
			//32 bit image is always DWORD aligned because each pixel requires 4 bytes
			if (bpp < 32)
			{
				border = 4 - (mat_frame.cols % 4);
			}

			Mat mat_temp;
			if (border > 0 || mat_frame.isContinuous() == false)
			{
				// Adding needed columns on the right (max 3 px)
				cv::copyMakeBorder(mat_frame, mat_temp, 0, 0, 0, border, cv::BORDER_CONSTANT, 0);
			}
			else
			{
				mat_temp = mat_frame;
			}

			RECT r;
			m_picture2.GetClientRect(&r);
			cv::Size winSize(r.right, r.bottom);

			cimage_mfc.Create(winSize.width, winSize.height, 24);

			BITMAPINFO* bitInfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
			bitInfo->bmiHeader.biBitCount = bpp;
			bitInfo->bmiHeader.biWidth = mat_temp.cols;
			bitInfo->bmiHeader.biHeight = -mat_temp.rows;
			bitInfo->bmiHeader.biPlanes = 1;
			bitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitInfo->bmiHeader.biCompression = BI_RGB;
			bitInfo->bmiHeader.biClrImportant = 0;
			bitInfo->bmiHeader.biClrUsed = 0;
			bitInfo->bmiHeader.biSizeImage = 0;
			bitInfo->bmiHeader.biXPelsPerMeter = 0;
			bitInfo->bmiHeader.biYPelsPerMeter = 0;

			//그레이스케일 인경우 팔레트가 필요
			if (bpp == 8)
			{
				RGBQUAD* palette = bitInfo->bmiColors;
				for (int i = 0; i < 256; i++)
				{
					palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
					palette[i].rgbReserved = 0;
				}
			}

			// Image is bigger or smaller than into destination rectangle
			// we use stretch in full rect

			if (mat_temp.cols == winSize.width && mat_temp.rows == winSize.height)
			{
				// source and destination have same size
				// transfer memory block
				// NOTE: the padding border will be shown here. Anyway it will be max 3px width

				SetDIBitsToDevice(cimage_mfc.GetDC(),
					//destination rectangle
					0, 0, winSize.width, winSize.height,
					0, 0, 0, mat_temp.rows,
					mat_temp.data, bitInfo, DIB_RGB_COLORS);
			}
			else
			{
				// destination rectangle
				int destx = 0, desty = 0;
				int destw = winSize.width;
				int desth = winSize.height;

				// rectangle defined on source bitmap
				// using imgWidth instead of mat_temp.cols will ignore the padding border
				int imgx = 0, imgy = 0;
				int imgWidth = mat_temp.cols - border;
				int imgHeight = mat_temp.rows;

				StretchDIBits(cimage_mfc.GetDC(),
					destx, desty, destw, desth,
					imgx, imgy, imgWidth, imgHeight,
					mat_temp.data, bitInfo, DIB_RGB_COLORS, SRCCOPY);
			}
			HDC dc = ::GetDC(m_picture2.m_hWnd);

			cimage_mfc.BitBlt(dc, 0, 0);

			::ReleaseDC(m_picture2.m_hWnd, dc);

			cimage_mfc.ReleaseDC();
			cimage_mfc.Destroy();

			CDialogEx::OnTimer(nIDEvent);
		}
		break;
	case 1400:
		m_nTimeCount++;
		if (m_nTimeCount >= m_nTimeLimit)
		{
			nas->StartFileSend(sendFileDate());       // 한달후 한번 호출하기를 바라는 함수..
			m_nTimeCount = 0;      // 호출후에는 초기화.. 그래야 또 한달을 계산하져..
		}

		CDialog::OnTimer(nIDEvent);
		break;
	}
}



void CMFCwebnautesDlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	//감지 등급 Edit Box 설정
	int iPos = m_sld.GetPos();
	setContour = iPos-1;
	CString sPos;
	sPos.Format(_T("%d"), iPos);
	m_cb.SetCurSel(iPos-1);
}


void CMFCwebnautesDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (capture == NULL) {
		capture = new VideoCapture(1);
		if (!capture->isOpened())
		{
			capture = new VideoCapture(0);
			if (!capture->isOpened()) {
				MessageBox(_T("캠을 열수 없습니다. \n"));
			}
		}
		Running();
	}
	Running();
}


void CMFCwebnautesDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MD_CONFIG *dlg = new MD_CONFIG;

	dlg->Create(IDD_DIALOG1);
	dlg->CenterWindow();
	dlg->ShowWindow(SW_SHOWNORMAL);
}

void CMFCwebnautesDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//폴더 선택 저장 변수
	FolderPath = CapPath;
	FolderPath += "\\";
	FolderPath += currentDate();
	ShellExecute(NULL, _T("open"), FolderPath, NULL, NULL, SW_SHOW);

}

void CMFCwebnautesDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MD_Calendar *cdlg = new MD_Calendar;
	cdlg->Create(IDD_DIALOG2);
	cdlg->CenterWindow();
	cdlg->ShowWindow(SW_SHOWNORMAL);
}


void CMFCwebnautesDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	// 하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strRatio;
	GetDlgItemText(IDC_EDIT1, strRatio);

	int nowPos = _ttoi(strRatio);
	setContour = nowPos - 1;
	m_sld.SetPos(nowPos);
}


void CMFCwebnautesDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//*pResult = 0;

}

void CMFCwebnautesDlg::ShowThumbnailList() {
	// nIndex 및 image list 초기화
	nIndex = 0;
	for (int i = 0; i < mImageList.GetImageCount(); i++)
		mImageList.Remove(i);
	mImageList.SetImageCount(nIndex);

	m_piclist.DeleteAllItems();

	// 캡쳐 파일 찾기
	CFileFind finder;
	CString imageStrPath(CapPath);
	imageStrPath += "\\";
	imageStrPath += currentDate();
	imageStrPath += "\\cam-*.jpg";
	Mat image;
	vector<CString> strFileList;

	BOOL bWorking = finder.FindFile(imageStrPath);
	while (bWorking) {
		bWorking = finder.FindNextFile();

		if (finder.IsDots() || finder.IsDirectory())
			continue;
		else {
			// 찾았으면 strFileList vector에 넣기
			strFileList.push_back(finder.GetFileName());
		}
	}
	finder.Close();

	CString tempStr(CapPath);
	tempStr += "\\";
	tempStr += currentDate();

	// 오름차순인 파일 리스트를 내림차순으로 변경
	reverse(strFileList.begin(), strFileList.end());
	for (unsigned int i = 0; i < strFileList.size(); i++) {
		CString temp = tempStr + "\\" + strFileList[i];
		CT2CA pszString(temp);
		std::string strPath(pszString);

		image = imread(strPath, IMREAD_UNCHANGED);
		AddThumbnailList(strFileList[i], image);

		if (!(image.empty())) image.release();
	}
}

HBITMAP CMFCwebnautesDlg::mat2bmp(cv::Mat* image)
{
	// 현재 응용프로그램의 스크린과 호환되는 memory dc를 생성한다.
	HDC         hDC = ::CreateCompatibleDC(0);
	BYTE        tmp[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	BITMAPINFO* bmi = (BITMAPINFO*)tmp;
	HBITMAP     hBmp;
	int i;
	int w = image->cols, h = image->rows;
	int bpp = image->channels() * 8;

	memset(bmi, 0, sizeof(BITMAPINFO));
	bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biWidth = w;
	bmi->bmiHeader.biHeight = h;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biBitCount = bpp;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = w * h * 1;
	bmi->bmiHeader.biClrImportant = 0;

	switch (bpp)
	{
	case 8:
		for (i = 0; i < 256; i++)
		{
			bmi->bmiColors[i].rgbBlue = i;
			bmi->bmiColors[i].rgbGreen = i;
			bmi->bmiColors[i].rgbRed = i;
		}
		break;
	case 32:
	case 24:
		((DWORD*)bmi->bmiColors)[0] = 0x00FF0000; /* red mask  */
		((DWORD*)bmi->bmiColors)[1] = 0x0000FF00; /* green mask */
		((DWORD*)bmi->bmiColors)[2] = 0x000000FF; /* blue mask  */
		break;
	}

	hBmp = ::CreateDIBSection(hDC, bmi, DIB_RGB_COLORS, NULL, 0, 0);
	::SetBitmapBits(hBmp, image->total() * image->channels(), image->data);
	::DeleteDC(hDC);

	return hBmp;
}

void CMFCwebnautesDlg::AddThumbnailList(CString FileName, Mat image) {
	// thumbnail 만들기
	Mat thumb = Mat(Size(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), CV_8UC3);
	resize(image, thumb, Size(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), 0, 0, INTER_LINEAR);

	// 리스트에 추가
	InsertList(thumb, FileName);

	// thumbnail release
	if (!(thumb.empty())) thumb.release();
}

void CMFCwebnautesDlg::InsertList(Mat thumb, CString FileName) {
	UpdateData(TRUE);
	LVITEM Item;

	// image list 길이 정의
	mImageList.SetImageCount(nIndex + 1);

	// 리스트에 아이템 추가
	Item.mask = LVIF_IMAGE | LVIF_TEXT;
	Item.iItem = nIndex;
	Item.iSubItem = 0;
	Item.pszText = FileName.GetBuffer(FileName.GetLength());
	Item.iImage = nIndex;
	m_piclist.InsertItem(&Item);

	// thumbnail 그리기
	CBitmap ThumbBitmap;
	HBITMAP bm = mat2bmp(&thumb);
	ThumbBitmap.Attach(bm);
	mImageList.Replace(nIndex, &ThumbBitmap, NULL);
	m_piclist.RedrawItems(nIndex, nIndex);

	// bm release
	DeleteObject(bm);

	nIndex++;
}

void CMFCwebnautesDlg::removeAllImages() {
	// 캡쳐 파일 찾기
	CFileFind finder;
	CString imageStrPath(CapPath);
	imageStrPath += "\\";
	imageStrPath += currentDate();
	imageStrPath += "\\cam-*.jpg";

	BOOL bWorking = finder.FindFile(imageStrPath);
	while (bWorking) {
		bWorking = finder.FindNextFile();

		if (finder.IsDots() || finder.IsDirectory())
			continue;
		else {
			// 찾았으면 삭제
			DeleteFile(CapPath + finder.GetFileName());
		}
	}
	finder.Close();

	ShowThumbnailList();
}

void CMFCwebnautesDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMItemActivate->iItem != -1) {
		// 이미지 파일 열기
		CString tempPath = (CapPath + "\\" + currentDate() + "\\");
		CString strFullPath = tempPath + m_piclist.GetItemText(pNMItemActivate->iItem, 0);

		isImageClick = false;
		//m_picture.DestroyWindow();
		CDC* pdc = m_picture.GetWindowDC();
		m_picture.ReleaseDC(pdc);

		// get picture-control dc
		CDC* pDc = m_picture.GetWindowDC();
		CDC memdc;
		// load image jpg,bmp,png
		CImage m_bmpBitmap;
		int width, height;
		m_bmpBitmap.Destroy();
		m_bmpBitmap.Load(strFullPath);
		width = m_bmpBitmap.GetWidth();
		height = m_bmpBitmap.GetHeight();

		memdc.CreateCompatibleDC(pDc);
		m_bmpBitmap.Draw(pDc->m_hDC, 0, 0, width, height);

		pDc->StretchBlt(0, 0, width, height, &memdc, 0, 0, width, height, SRCCOPY);

		// release dc to picture-control
		m_picture.ReleaseDC(pDc);

		pDc->DeleteDC();
	}

	*pResult = 0;
}

void CMFCwebnautesDlg::OnBnClickedCancel()
{
	capture->release();
	destroyAllWindows();
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_APP_EXIT, NULL);
}

void CMFCwebnautesDlg::OnClickedPicture2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDC* pdc_ = m_picture.GetWindowDC();
	m_picture.ReleaseDC(pdc_);
	isImageClick = true;
	CDC* pdc = m_picture2.GetWindowDC();
	m_picture2.ReleaseDC(pdc);
}


void CMFCwebnautesDlg::OnDblclkPicture2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDC* pdc_ = m_picture.GetWindowDC();
	m_picture.ReleaseDC(pdc_);
	isImageClick = true;
	CDC* pdc = m_picture2.GetWindowDC();
	m_picture2.ReleaseDC(pdc);
}


void CMFCwebnautesDlg::OnDblclkPicture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDC* pdc_ = m_picture.GetWindowDC();
	m_picture.ReleaseDC(pdc_);
	isImageClick = true;
	CDC* pdc = m_picture2.GetWindowDC();
	m_picture2.ReleaseDC(pdc);
}


void CMFCwebnautesDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDC* pdc_ = m_picture.GetWindowDC();
	m_picture.ReleaseDC(pdc_);
	isImageClick = true;
	CDC* pdc = m_picture2.GetWindowDC();
	m_picture2.ReleaseDC(pdc);
}


void CMFCwebnautesDlg::OnCbnSelchangeCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString temp;

	this->m_cb.GetLBText(this->m_cb.GetCurSel(), temp);
	int nowPos = _ttoi(temp);
	setContour = nowPos;
	m_sld.SetPos(nowPos);
}
