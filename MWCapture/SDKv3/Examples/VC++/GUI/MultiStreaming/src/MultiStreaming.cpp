/************************************************************************************************/
// MultiStreaming.cpp : Defines the class behaviors for the application.

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
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MultiStreaming.h"
#include "MainFrm.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int g_nValidChannel[32] = {-1};
int g_nValidChannelCount = 0;

// CMultiStreamingApp

BEGIN_MESSAGE_MAP(CMultiStreamingApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMultiStreamingApp::OnAppAbout)
END_MESSAGE_MAP()


// CMultiStreamingApp construction

CMultiStreamingApp::CMultiStreamingApp()
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler); 

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("MultiStreaming.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMultiStreamingApp object

CMultiStreamingApp theApp;


// CMultiStreamingApp initialization

BOOL CMultiStreamingApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
	WCHAR wPath[1024] = {0};
	GetCurrentDirectory(1024, wPath);

	int len = WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), NULL, 0, NULL, NULL);  
	char* chPath = new char[len + 1];  
	WideCharToMultiByte(CP_ACP, 0, wPath, wcslen(wPath), chPath, len, NULL, NULL);  
	chPath[len] = '\0';  

	char chDrive[64] = {0};
	_splitpath(chPath, chDrive, NULL, NULL, NULL);

	CString strDrive(chDrive);


	if (!MWCaptureInitInstance()) {
		AfxMessageBox(_T("MWCaptureInitInstance return fail!\n"));
		return FALSE;
	}

	int nVideoCount = MWGetChannelCount();
	for (int i = 0; i < nVideoCount; i++)
	{
		MWCAP_CHANNEL_INFO mci;
		MWGetChannelInfoByIndex(i, &mci);
		if (0 == strcmp(mci.szFamilyName, "Pro Capture"))
		{
			g_nValidChannel[g_nValidChannelCount] = i;
			g_nValidChannelCount ++;
		}
	}

	if (nVideoCount <= 0 || g_nValidChannelCount <= 0) {
		AfxMessageBox(_T("Can't find capture device!\n"));
		return FALSE;
	}

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);






	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

int CMultiStreamingApp::ExitInstance()
{
	MWCaptureExitInstance();

	return CWinApp::ExitInstance();
}

// CMultiStreamingApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CMultiStreamingApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMultiStreamingApp message handlers

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
	CloseHandle(hDumpFile);  
}


LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	CreateDumpFile(L"MultiStreaming.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}

