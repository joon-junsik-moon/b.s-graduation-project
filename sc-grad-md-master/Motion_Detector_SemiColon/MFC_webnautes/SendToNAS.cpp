#include "pch.h"
#include "SendToNAS.h"
#include "stdafx.h"
#include <iostream>
#include <Windows.h>

using namespace std;

CWinThread* m_pThread;

SendToNAS::SendToNAS() {
	m_pThread = NULL;
}

SendToNAS::~SendToNAS()
{
	// 서버가 켜진상태라면 Stop 메소드 실행
	if (m_pThread->m_hThread != NULL) {
		// Server Thread 종료
		TerminateThread(m_pThread->m_hThread, NULL);
		m_pThread = NULL;
	}

	// 윈속 사용 해제
	//WSACleanup();
}

BOOL SendToNAS::DeleteDirectoryFile(LPCTSTR RootDir)
{
	if (RootDir == NULL)
	{
		return FALSE;
	}

	BOOL bRval = FALSE;

	CString szNextDirPath = _T("");
	CString szRoot = _T("");


	// 해당 디렉토리의 모든 파일을 검사한다.
	szRoot.Format(_T("%s\\*.*"), RootDir);

	CFileFind find;

	bRval = find.FindFile(szRoot);

	if (bRval == FALSE)
	{
		return bRval;
	}

	while (bRval)
	{
		bRval = find.FindNextFile();

		// . or .. 인 경우 무시 한다. 
		if (find.IsDots() == FALSE)
		{
			// Directory 일 경우 재귀호출 한다.
			if (find.IsDirectory())
			{
				DeleteDirectoryFile(find.GetFilePath());
			}
			// file일 경우 삭제
			else
			{
				bRval = DeleteFile(find.GetFilePath());
			}
		}
	}
	find.Close();
	bRval = RemoveDirectory(RootDir);
	return bRval;
}

void SendToNAS::SelectDirectory(CString date) {
	for (int i = 1; i < 32; i++) {
		if (i < 10) {
			CString cbuf = _T("");
			cbuf.Format(_T("%d"), i);
			date += _T("0");
			date += cbuf;
			DeleteDirectoryFile(date);
		}
		else {
			CString cbuf = _T("");
			cbuf.Format(_T("%d"), i);
			date += cbuf;
			DeleteDirectoryFile(date);
		}
	}
}

void SendToNAS::StartFileSend(CString date) {
	extern CString CapPath;
	extern CString serverPath; // /tmp/image
	extern CString serverIP; // 117.16.46.97
	extern CString serverID; // pi
	extern CString serverPW; // semicolon
	CString pscp = _T("pscp.exe -pw ");
	pscp += serverPW;
	pscp += _T(" - r ");
	pscp += CapPath;
	pscp += _T("\\");
	pscp += date;
	pscp += _T("* ");
	pscp += serverID;
	pscp += _T("@");
	pscp += serverIP;
	pscp += _T(":");
	pscp += serverPath;

	CStringA strA(pscp); // a helper string
	LPCSTR ptr = strA;
	if (WinExec(ptr, SW_SHOW)) {
		SelectDirectory(date);
	}
	else {
		StartFileSend(date);
	}
}

BOOL SendToNAS::START() {
	m_pThread = NULL;
	return true;
}