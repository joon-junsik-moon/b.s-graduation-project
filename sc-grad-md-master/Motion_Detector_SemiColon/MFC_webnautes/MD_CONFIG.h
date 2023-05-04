#pragma once


// MD_CONFIG 대화 상자
class CMFCwebnautesDlg;
class MFC_webnautesDlg;

class MD_CONFIG : public CDialogEx
{
	DECLARE_DYNAMIC(MD_CONFIG)

public:
	MD_CONFIG(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MD_CONFIG();
	CString PortText; // Port 번호를 변경할 때 사용하는 변수입니다.
	CString FolderPath; // Folder 경로를 변경할 때 사용하는 변수입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void MD_CONFIG::PostNcDestroy();
	afx_msg void MD_CONFIG::OnDestroy();
	afx_msg void MD_CONFIG::OnClose();

	//서버 정보
	CEdit InputServerPath;
	CEdit InputServerIP;
	CEdit InputServerID;
	CEdit InputServerPW;
private:
	
};
