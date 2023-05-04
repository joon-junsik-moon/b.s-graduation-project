#pragma once
#pragma warning(disable:4996)

#include "stdafx.h"
#include "pch.h"
#include "afxdialogex.h"
#include "framework.h"
#include "MFC_webnautesDlg.h"

using namespace std;
using namespace cv;

class SendToNAS/* : public CDialogEx*/
{
public:
	SendToNAS();
	virtual ~SendToNAS();
	void StartFileSend(CString date);			// 클라우드에 사진 보내기
	void SelectDirectory(CString date);			// 보낸 폴더 지우기
	BOOL DeleteDirectoryFile(LPCTSTR RootDir);	// 보낸 사진 지우기
	BOOL START();								// 변수 초기화
};