// SelectImageList.cpp: 구현 파일
//

#include "pch.h"
#include "SelectImageList.h"
#include "afxdialogex.h"
#include "pch.h"
#include "framework.h"
#include "MFC_webnautes.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <time.h>
#include <vector>
#include <atlimage.h>

#pragma warning(disable:4996)

using namespace std;
using namespace cv;

// SelectImageList 대화 상자

IMPLEMENT_DYNAMIC(SelectImageList, CDialogEx)


BOOL SelectImageList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ImageList.Create(180, 135, ILC_COLOR8 | ILC_MASK, 8, 8);
	m_npiclist.SetImageList(&m_ImageList, LVSIL_NORMAL);
	ShowThumbnailList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

CString SelectImageList::currentDate() {
	time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
	struct tm  tstruct;
	char       buf[80];
	CString cbuf = _T("");
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d", &tstruct); // YYYYMMDD 형태의 스트링
	cbuf = LPSTR(buf);
	return cbuf;
}

SelectImageList::SelectImageList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{
	
}

SelectImageList::~SelectImageList()
{
}

void SelectImageList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_LIST1, m_piclist);
	DDX_Control(pDX, IDC_LIST1, m_npiclist);
}


BEGIN_MESSAGE_MAP(SelectImageList, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &SelectImageList::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &SelectImageList::OnNMDblclkList1)
END_MESSAGE_MAP()


// SelectImageList 메시지 처리기

void SelectImageList::ShowThumbnailList() {
	// nIndex 및 image list 초기화
	nIndex = 0;
	for (int i = 0; i < m_ImageList.GetImageCount(); i++)
		m_ImageList.Remove(i);
	m_ImageList.SetImageCount(nIndex);

	m_npiclist.DeleteAllItems();

	// 캡쳐 파일 찾기
	CFileFind finder;
	extern 	CString selectFolderPath;
	CString imageStrPath(selectFolderPath);
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

	CString tempStr(selectFolderPath);

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

HBITMAP SelectImageList::mat2bmp(cv::Mat* image)
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

void SelectImageList::AddThumbnailList(CString FileName, Mat image) {
	// thumbnail 만들기
	Mat thumb = Mat(Size(TWIDTH, THEIGHT), CV_8UC3);
	resize(image, thumb, Size(TWIDTH, THEIGHT), 0, 0, INTER_LINEAR);

	// 리스트에 추가
	InsertList(thumb, FileName);

	// thumbnail release
	if (!(thumb.empty())) thumb.release();
}

void SelectImageList::InsertList(Mat thumb, CString FileName) {
	UpdateData(TRUE);
	LVITEM Item;

	// image list 길이 정의
	m_ImageList.SetImageCount(nIndex + 1);

	// 리스트에 아이템 추가
	Item.mask = LVIF_IMAGE | LVIF_TEXT;
	Item.iItem = nIndex;
	Item.iSubItem = 0;
	Item.pszText = FileName.GetBuffer(FileName.GetLength());
	Item.iImage = nIndex;
	m_npiclist.InsertItem(&Item);

	// thumbnail 그리기
	CBitmap ThumbBitmap;
	HBITMAP bm = mat2bmp(&thumb);
	ThumbBitmap.Attach(bm);
	m_ImageList.Replace(nIndex, &ThumbBitmap, NULL);
	m_npiclist.RedrawItems(nIndex, nIndex);

	// bm release
	DeleteObject(bm);

	nIndex++;
}

void SelectImageList::removeAllImages() {
	// 캡쳐 파일 찾기
	CFileFind finder;
	//extern CString CapPath;
	extern CString selectFolderPath;
	CString imageStrPath(selectFolderPath);
	imageStrPath += "\\cam-*.jpg";

	BOOL bWorking = finder.FindFile(imageStrPath);
	while (bWorking) {
		bWorking = finder.FindNextFile();

		if (finder.IsDots() || finder.IsDirectory())
			continue;
		else {
			// 찾았으면 삭제
			DeleteFile(selectFolderPath + finder.GetFileName());
		}
	}
	finder.Close();

	ShowThumbnailList();
}

void SelectImageList::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//*pResult = 0;
}


void SelectImageList::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (pNMItemActivate->iItem != -1) {
		// 이미지 파일 열기
		extern CString selectFolderPath;
		selectFolderPath += "\\";
		CString strFullPath = selectFolderPath + m_npiclist.GetItemText(pNMItemActivate->iItem, 0);

		CString strViewerPath = _T("\"C:\\Program Files\\Windows Photo Viewer\\PhotoViewer.dll\", ImageView_Fullscreen  ");
		strViewerPath += strFullPath;

		ShellExecute(NULL, _T("open"), _T("Rundll32.exe"), strViewerPath, NULL, SW_SHOW);
	}
	*pResult = 0;
}
