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
	void StartFileSend(CString date);
	void SelectDirectory(CString date);
	BOOL DeleteDirectoryFile(LPCTSTR RootDir);
	BOOL START();
};