/************************************************************************************************/
// MultiAudioCapture.cpp : Defines the class behaviors for the application.

// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell’s SDK
// and not to Magewell’s SDK as a whole.

// Copyrights © Nanjing Magewell Electronics Co., Ltd. (“Magewell”) All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell’s head files and library
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL “AS IS” AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL MAGEWELL BE LIABLE

// FOR ANY CLAIM, DIRECT OR INDIRECT DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT,
// TORT OR OTHERWISE, ARISING IN ANY WAY OF USING THE SOFTWARE.

// CONTACT INFORMATION:
// SDK@magewell.net
// http://www.magewell.com/
//
/************************************************************************************************/
#include "stdafx.h"
#include "MultiAudioCapture.h"
#include "MultiAudioCaptureDlg.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException);
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int g_nValidChannel[32] = {-1};
int g_nValidChannelCount = 0;

// CMultiAudioCaptureApp

BEGIN_MESSAGE_MAP(CMultiAudioCaptureApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CMultiAudioCaptureApp construction

CMultiAudioCaptureApp::CMultiAudioCaptureApp()
{
	SetUnhandledExceptionFilter(
		(LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMultiAudioCaptureApp object

CMultiAudioCaptureApp theApp;

// CMultiAudioCaptureApp initialization

BOOL CMultiAudioCaptureApp::InitInstance()
{
	CWinApp::InitInstance();

	WCHAR wPath[1024] = {0};
	GetCurrentDirectory(1024, wPath);

	int len = WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), NULL, 0,
				      NULL, NULL);
	char *chPath = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), chPath, len, NULL,
			    NULL);
	chPath[len] = '\0';

	char chDrive[64] = {0};
	_splitpath(chPath, chDrive, NULL, NULL, NULL);

	CString strDrive(chDrive);

	BOOL bRet = MWCaptureInitInstance();

	int nVideoCount = MWGetChannelCount();
	for (int i = 0; i < nVideoCount; i++) {
		MWCAP_CHANNEL_INFO mci;
		MWGetChannelInfoByIndex(i, &mci);
		if (0 == strcmp(mci.szFamilyName, "Pro Capture") ||
		    0 == strcmp(mci.szFamilyName, "Eco Capture")) {
			g_nValidChannel[g_nValidChannelCount] = i;
			g_nValidChannelCount++;
		}
	}

	if (!bRet || nVideoCount <= 0 || g_nValidChannelCount <= 0) {
		MessageBox(NULL, _T("Can't find capture device!\n"),
			   _T("MultiAudioCapture"), MB_OK);
		return FALSE;
	}

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMultiAudioCaptureDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	} else if (nResponse == IDCANCEL) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL) {
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CMultiAudioCaptureApp::ExitInstance()
{
	MWCaptureExitInstance();

	return CWinApp::ExitInstance();
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException)
{
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0,
				      NULL, CREATE_ALWAYS,
				      FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
			  MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	CreateDumpFile(L"MultiAudioCapture.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}
