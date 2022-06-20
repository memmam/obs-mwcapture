/************************************************************************************************/
// NDISender.cpp :  Defines the class behaviors for the application.

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
#include "NDISender.h"
#include "NDISenderDlg.h"

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

// CNDISenderApp

BEGIN_MESSAGE_MAP(CNDISenderApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CNDISenderApp 构造

CNDISenderApp::CNDISenderApp()
{
	SetUnhandledExceptionFilter(
		(LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CNDISenderApp 对象

CNDISenderApp theApp;

// CNDISenderApp 初始化

BOOL CNDISenderApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

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

	//add mwcap_init
	MWCaptureInitInstance();
	MWRefreshDevice();

	int nVideoCount = MWGetChannelCount();
	for (int i = 0; i < nVideoCount; i++) {
		MWCAP_CHANNEL_INFO mci;
		MWGetChannelInfoByIndex(i, &mci);
		if (0 == strcmp(mci.szFamilyName, "Pro Capture")) {
			g_nValidChannel[g_nValidChannelCount] = i;
			g_nValidChannelCount++;
		}
	}

	if (nVideoCount <= 0 || g_nValidChannelCount <= 0) {
		AfxMessageBox(_T("Can't find capture device!\n"));
		return FALSE;
	}

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CNDISenderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	} else if (nResponse == IDCANCEL) {
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL) {
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CNDISenderApp::ExitInstance()
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
	CreateDumpFile(L"NDISender.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}