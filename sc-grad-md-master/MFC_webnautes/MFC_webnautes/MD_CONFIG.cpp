// MD_CONFIG.cpp: 구현 파일
//
#include "pch.h"
#include "MFC_webnautes.h"
#include "MD_CONFIG.h"
#include "afxdialogex.h"

// MD_CONFIG 대화 상자

IMPLEMENT_DYNAMIC(MD_CONFIG, CDialogEx)

MD_CONFIG::MD_CONFIG(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

MD_CONFIG::~MD_CONFIG()
{
}

void MD_CONFIG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, InputServerPath);
	DDX_Control(pDX, IDC_EDIT2, InputServerIP);
	DDX_Control(pDX, IDC_EDIT3, InputServerID);
	DDX_Control(pDX, IDC_EDIT4, InputServerPW);
}


BEGIN_MESSAGE_MAP(MD_CONFIG, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &MD_CONFIG::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &MD_CONFIG::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &MD_CONFIG::OnBnClickedOk)
END_MESSAGE_MAP()

void iniServerInfo(CString ServerIP, CString ServerPath, CString ServerID, CString ServerPW) {
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerIP"), ServerIP, _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerPath"), ServerPath, _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerID"), ServerID, _T(".\\semiMD.ini"));
	WritePrivateProfileString(_T("ServerInfo"), _T("ServerPW"), ServerPW, _T(".\\semiMD.ini"));
}

// MD_CONFIG 메시지 처리기

void MD_CONFIG::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BROWSEINFO BrInfo;
	TCHAR szBuffer[512];                                      // 경로저장 버퍼 

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("파일이 저장될 폴더를 선택하세요");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);				// 파일경로 읽어오기

	if (szBuffer != _T("")) {
		extern CString CapPath;
		CapPath.Format(_T("%s"), szBuffer);

		// Write 

		WritePrivateProfileString(_T("MotionDetector"), _T("CapPath"), szBuffer, _T(".\\semiMD.ini"));


		AfxMessageBox(CapPath);
	}
}

void MD_CONFIG::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PortText = _T("");

	GetDlgItemText(IDC_EDIT1, PortText);
}

void MD_CONFIG::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
	extern CString serverPath;
	extern CString serverIP;
	extern CString serverID;
	extern CString serverPW;
	CString tempPath = _T("");
	CString tempIP = _T("");
	CString tempID = _T("");
	CString tempPW = _T("");

	InputServerPath.GetWindowTextW(tempPath);
	InputServerIP.GetWindowTextW(tempIP);
	InputServerID.GetWindowTextW(tempID);
	InputServerPW.GetWindowTextW(tempPW);

	if (tempPath != _T("") && tempIP != _T("") && tempID != _T("") && tempPW != _T("")) {
		//CString temp = tempIP + _T("/") + tempPath + _T("/") + tempID + _T("/") + tempPW;
		//AfxMessageBox(temp);
		serverPath = tempPath;
		serverIP = tempIP;
		serverID = tempID;
		serverPW = tempPW;
		iniServerInfo(serverIP, serverPath, serverID, serverPW);
	}

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

	DestroyWindow();
}

void MD_CONFIG::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	//CDialog::PostNcDestroy();
}

void MD_CONFIG::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	DestroyWindow(); // 윈도우 창 없앰


	// CDialog::OnClose(); // <= 이거 주석처리 해야됨
}

void MD_CONFIG::OnDestroy()
{
	CDialogEx::OnDestroy();

	delete this;

	//이거 찾아봐야겟네 --
}