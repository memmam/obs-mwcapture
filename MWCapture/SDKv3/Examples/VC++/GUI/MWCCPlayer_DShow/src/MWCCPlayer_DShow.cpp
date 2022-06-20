/************************************************************************************************/
// MWCCPlayer_DShow.cpp : Defines the class behaviors for the application.

// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell¡¯s SDK
// and not to Magewell¡¯s SDK as a whole.

// Copyrights ? Nanjing Magewell Electronics Co., Ltd. (¡°Magewell¡±) All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell¡¯s head files and library
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL ¡°AS IS¡± AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
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
#include "MWCCPlayer_DShow.h"
#include "MWCCPlayer_DShowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMWCCPlayer_DShowApp

BEGIN_MESSAGE_MAP(CMWCCPlayer_DShowApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CMWCCPlayer_DShowApp construction

CMWCCPlayer_DShowApp::CMWCCPlayer_DShowApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMWCCPlayer_DShowApp object

CMWCCPlayer_DShowApp theApp;

// CMWCCPlayer_DShowApp initialization

BOOL CMWCCPlayer_DShowApp::InitInstance()
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
	CoInitialize(NULL);

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(
		RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMWCCPlayer_DShowDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	} else if (nResponse == IDCANCEL) {
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	} else if (nResponse == -1) {
		TRACE(traceAppMsg, 0,
		      "Warning: Create dialog failed，application will exit.\n");
		TRACE(traceAppMsg, 0,
		      "Warning: If you use MFC control in dialog，you can't #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL) {
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CMWCCPlayer_DShowApp::ExitInstance()
{
	CoUninitialize();
	return CWinApp::ExitInstance();
}
