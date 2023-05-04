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

// SelectImageList 대화 상자
using namespace cv;

#define	TWIDTH		192		// thubmail 이미지 크기 1024
#define	THEIGHT	144		// thubmail 이미지 크기 768

class SelectImageList : public CDialogEx
{
	DECLARE_DYNAMIC(SelectImageList)

public:
	SelectImageList(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~SelectImageList();
	void InsertList(Mat thumb, CString FileName);			// 리스트에 thumbnail 넣기
	void ShowThumbnailList();									// list control 에 사진 thumbnail 보여줌
	void AddThumbnailList(CString FileName, Mat image);	// list control 에 thumbnail 추가
	void removeAllImages();										// mPath 에 저장된 모든 사진 삭제
	HBITMAP mat2bmp(cv::Mat* image);
	CString currentDate();
	UINT nIndex;				// 사진 리스트에 사용되는 list control 과 image list 의 index
	CImageList m_ImageList;		// 사진 리스트에 같에 사용할 image list
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
//	CListCtrl m_piclist;
	CListCtrl m_npiclist;
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
};

