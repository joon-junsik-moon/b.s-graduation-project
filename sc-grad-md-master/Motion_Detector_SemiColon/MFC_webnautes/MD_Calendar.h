#pragma once

// MD_Calendar 대화 상자
class CMFCwebnautesDlg;
class MFC_webnautesDlg;

class MD_Calendar : public CDialogEx
{
	DECLARE_DYNAMIC(MD_Calendar)

public:
	MD_Calendar(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MD_Calendar();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMcnSelchangeMonthcalendar1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();

	COleDateTime m_time; // 시간
};
