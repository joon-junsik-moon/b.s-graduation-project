
// MFC_webnautesDlg.h: 헤더 파일
//

#pragma once
#pragma warning(disable:4996)

#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "MD_CONFIG.h"
#include "MD_Calendar.h"
#include "pch.h"
#include "framework.h"
#include "MFC_webnautes.h"
#include "MFC_webnautesDlg.h"
#include "afxdialogex.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <Windows.h>
#include "MD_CONFIG.h"
#include "MD_Calendar.h"
#include <atlimage.h>
#include "SendToNAS.h"
#include "ImageStreamingServer.h"

class SendToNAS;
//(original height / original width) x new width = new height 
#define	THUMBNAIL_WIDTH		192		// thubmail 이미지 크기 1024
#define	THUMBNAIL_HEIGHT	144		// thubmail 이미지 크기 768

using namespace cv;

// CMFCwebnautesDlg 대화 상자
class CMFCwebnautesDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCwebnautesDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

												// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_WEBNAUTES_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnStnClickedPicture();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnClickedPicture2();
	afx_msg void OnDblclkPicture2();
	afx_msg void OnDblclkPicture();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnCbnSelchangeCombo2();

	void InsertList(Mat thumb, CString FileName);		// 리스트에 thumbnail 넣기
	void ShowThumbnailList();							// list control 에 사진 thumbnail 보여줌
	void AddThumbnailList(CString FileName, Mat image);	// list control 에 thumbnail 추가
	void removeAllImages();								// mPath 에 저장된 모든 사진 삭제
	void capPicture(Mat image);							// 모션 저장
	void Running();										// 모션 감지 부분
	void NowImage(Mat image);							//현재 이미지 저장
	CString currentDate();								// 오늘 일자 구하기
	HBITMAP mat2bmp(cv::Mat* image);					//

	MD_Calendar cdlg;
	SendToNAS *nas;
	MD_CONFIG dlg;
	ImageStreamingServer* sThread;
	CListCtrl m_piclist;	
	CSliderCtrl m_sld;
	CButton m_list;
	CImageList mImageList;		// 사진 리스트에 같에 사용할 image list
	CStatic m_picture;			// 메인 영상 화면
	CStatic m_picture2;			// 서브 영상 화면
	CComboBox m_cb;
	BOOL m_DataView;
	BOOL m_CalView;
	UINT nIndex;				// 사진 리스트에 사용되는 list control 과 image list 의 index
	int m_nTimeCount;			// NAS에 보내기 위해 사용되는 시간 카운트
	int m_nTimeLimit = 43200;	// NAS에 보내기 위해 사용되는 시간 카운트 최댓값
};
