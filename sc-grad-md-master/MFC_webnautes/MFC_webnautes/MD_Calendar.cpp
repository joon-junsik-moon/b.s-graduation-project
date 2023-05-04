// MD_Calendar.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_webnautes.h"
#include "MD_Calendar.h"
#include "afxdialogex.h"
#include "MFC_webnautesDlg.h"
#include "SelectImageList.h"
// MD_Calendar 대화 상자
//extern CString FolderPath;
CString selectFolderPath;
IMPLEMENT_DYNAMIC(MD_Calendar, CDialogEx)

MD_Calendar::MD_Calendar(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_time(COleDateTime::GetCurrentTime())
{

}

MD_Calendar::~MD_Calendar()
{
}

void MD_Calendar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_MonthCalCtrl(pDX, IDC_MONTHCALENDAR1, m_time);
}


BEGIN_MESSAGE_MAP(MD_Calendar, CDialogEx)
	ON_BN_CLICKED(IDOK, &MD_Calendar::OnBnClickedOk)
	ON_NOTIFY(MCN_SELCHANGE, IDC_MONTHCALENDAR1, &MD_Calendar::OnMcnSelchangeMonthcalendar1)
END_MESSAGE_MAP()


BOOL CheckOpenDir(CString sDirName, CString path)
{
	CFileFind file;
	CString strFile = _T("*.*");
	BOOL bResult = file.FindFile(sDirName + strFile);
	CString msg = _T("선택된 날짜에 찍힌 사진이 없습니다.");

	if (!bResult)
	{
		//ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOW);
		//FolderPath = path;
	
		AfxMessageBox(msg);
		if (!bResult)
		{
			/*Error*/
			return FALSE;
		}
		return TRUE;
	}
	else {
		//ShellExecute(NULL, _T("open"), sDirName, NULL, NULL, SW_SHOW);
		//FolderPath = sDirName;
		selectFolderPath = sDirName;
		SelectImageList* dlg = new SelectImageList;
		dlg->Create(IDD_DIALOG3);
		dlg->CenterWindow();
		dlg->ShowWindow(SW_SHOWNORMAL);
		//ShellExecute(NULL, _T("open"), sDirName, NULL, NULL, SW_SHOW);
	}
	return FALSE;
}

// MD_Calendar 메시지 처리기


void MD_Calendar::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
	UpdateData();
	CString selFolderDate = _T("");

	CString CapPath = _T("");

	CString Path = _T("");
	GetPrivateProfileString(_T("MotionDetector"), _T("CapPath"), _T("-"), (LPWSTR)(LPCWSTR)Path, 256, _T(".\\semiMD.ini"));
	CapPath.Format(_T("%s"), static_cast<LPCTSTR>(Path));

	selFolderDate.Format(_T("%04d%02d%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay());
	CString path = CapPath + "\\" + selFolderDate;
	//AfxMessageBox(path); // 경로 확인하는 용도
	CheckOpenDir(path, CapPath);
	UpdateData(false);
}


void MD_Calendar::OnMcnSelchangeMonthcalendar1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

//BOOL MD_Calendar::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	// TODO: Add extra initialization here
//
//
//	// CParentDlg 클래스의 메인프레임 인스턴스 포인터를 얻어 옵니다...
//
//	IDD_MFC_WEBNAUTES_DIALOG* pDlg = (IDD_MFC_WEBNAUTES_DIALOG*)AfxGetMainWnd();
//
//
//	pDlg->SendMessage(WM_CLOSE, 0, 0); //CParentDlg다이얼로그 상자로 메시지를 전송
//
//	return TRUE; // return TRUE unless you set the focus to a control
//	// EXCEPTION: OCX Property Pages should return FALSE
//}
