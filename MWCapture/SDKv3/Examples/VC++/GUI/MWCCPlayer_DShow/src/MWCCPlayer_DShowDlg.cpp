/************************************************************************************************/
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
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMWCCPlayer_DShowDlg dialog



CMWCCPlayer_DShowDlg::CMWCCPlayer_DShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMWCCPlayer_DShowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CMWCCPlayer_DShowDlg::~CMWCCPlayer_DShowDlg()
{
	if (m_pCCGrapgBuilder != NULL)
		delete m_pCCGrapgBuilder;
}

void CMWCCPlayer_DShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMWCCPlayer_DShowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMWCCPlayer_DShowDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RADIO_WITH_CC, &CMWCCPlayer_DShowDlg::OnBnClickedRadioWithCc)
	ON_BN_CLICKED(IDC_RADIO_WITOUT_CC, &CMWCCPlayer_DShowDlg::OnBnClickedRadioWitoutCc)
	ON_BN_CLICKED(IDC_BUT_STOP, &CMWCCPlayer_DShowDlg::OnBnClickedButStop)
	ON_BN_CLICKED(IDC_BUT_PAUSE, &CMWCCPlayer_DShowDlg::OnBnClickedButPause)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICES, &CMWCCPlayer_DShowDlg::OnCbnSelchangeComboDevices)
	ON_BN_CLICKED(IDC_BUT_OK, &CMWCCPlayer_DShowDlg::OnBnClickedButOk)
	ON_BN_CLICKED(IDC_BUT_CANCEL, &CMWCCPlayer_DShowDlg::OnBnClickedButCancel)
	ON_BN_CLICKED(IDC_CHECK_CC708, &CMWCCPlayer_DShowDlg::OnBnClickedCheckCc708)
	ON_BN_CLICKED(IDC_CHECK_CC608, &CMWCCPlayer_DShowDlg::OnBnClickedCheckCc608)
	ON_CBN_SELCHANGE(IDC_COMBO_CC_CHANNEL, &CMWCCPlayer_DShowDlg::OnCbnSelchangeComboCcChannel)
	ON_BN_CLICKED(IDC_BUT_Open, &CMWCCPlayer_DShowDlg::OnBnClickedButOpen)
	ON_EN_CHANGE(IDC_EDIT_FONT_SIZE, &CMWCCPlayer_DShowDlg::OnEnChangeEditFontSize)
	ON_CBN_SELCHANGE(IDC_COMBO_BACK_COLOR, &CMWCCPlayer_DShowDlg::OnCbnSelchangeComboBackColor)
	ON_EN_CHANGE(IDC_EDIT_BACK_R, &CMWCCPlayer_DShowDlg::OnEnChangeEditBackR)
	ON_EN_CHANGE(IDC_EDIT_BACK_G, &CMWCCPlayer_DShowDlg::OnEnChangeEditBackG)
	ON_EN_CHANGE(IDC_EDIT_BACK_B, &CMWCCPlayer_DShowDlg::OnEnChangeEditBackB)
	ON_EN_CHANGE(IDC_EDIT_BACK_A, &CMWCCPlayer_DShowDlg::OnEnChangeEditBackA)
	ON_CBN_SELCHANGE(IDC_COMBO_FORE_COLOR, &CMWCCPlayer_DShowDlg::OnCbnSelchangeComboForeColor)
	ON_EN_CHANGE(IDC_EDIT_FORE_R, &CMWCCPlayer_DShowDlg::OnEnChangeEditForeR)
	ON_EN_CHANGE(IDC_EDIT_FORE_G, &CMWCCPlayer_DShowDlg::OnEnChangeEditForeG)
	ON_EN_CHANGE(IDC_EDIT_FORE_B, &CMWCCPlayer_DShowDlg::OnEnChangeEditForeB)
	ON_EN_CHANGE(IDC_EDIT_FORE_A, &CMWCCPlayer_DShowDlg::OnEnChangeEditForeA)
	ON_EN_CHANGE(IDC_EDIT_CC_WIDTH, &CMWCCPlayer_DShowDlg::OnEnChangeEditCcWidth)
	ON_EN_CHANGE(IDC_EDIT_CC_HEIGHT, &CMWCCPlayer_DShowDlg::OnEnChangeEditCcHeight)
	ON_BN_CLICKED(IDC_RADIO_SET_CC, &CMWCCPlayer_DShowDlg::OnBnClickedRadioSetCc)
	ON_EN_KILLFOCUS(IDC_EDIT_FONT_SIZE, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditFontSize)
	ON_EN_KILLFOCUS(IDC_EDIT_BACK_R, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackR)
	ON_EN_KILLFOCUS(IDC_EDIT_BACK_G, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackG)
	ON_EN_KILLFOCUS(IDC_EDIT_BACK_B, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackB)
	ON_EN_KILLFOCUS(IDC_EDIT_BACK_A, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackA)
	ON_EN_KILLFOCUS(IDC_EDIT_FORE_R, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeR)
	ON_EN_KILLFOCUS(IDC_EDIT_FORE_G, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeG)
	ON_EN_KILLFOCUS(IDC_EDIT_FORE_B, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeB)
	ON_EN_KILLFOCUS(IDC_EDIT_FORE_A, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeA)
	ON_EN_KILLFOCUS(IDC_EDIT_CC_WIDTH, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditCcWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_CC_HEIGHT, &CMWCCPlayer_DShowDlg::OnEnKillfocusEditCcHeight)
END_MESSAGE_MAP()


// CMWCCPlayer_DShowDlg message handlers

BOOL CMWCCPlayer_DShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_bCCChecked = false;
	m_bWithoutCCChecked = false;

	GetClientRect(&m_Rect);
	m_pcwndRadioCC = GetDlgItem(IDC_RADIO_WITH_CC);
	m_pcwndRadioWithoutCC = GetDlgItem(IDC_RADIO_WITOUT_CC);
	m_pcwndRadioSetCC = GetDlgItem(IDC_RADIO_SET_CC);
	m_pcwndDeviceCombo = GetDlgItem(IDC_COMBO_DEVICES);
	m_pcwndCheck708 = GetDlgItem(IDC_CHECK_CC708);
	m_pcwndCheck608 = GetDlgItem(IDC_CHECK_CC608);
	m_pcwndChannel608 = GetDlgItem(IDC_COMBO_CC_CHANNEL);
	m_pcwndFontFilePath = GetDlgItem(IDC_EDIT_FONT_FILE);
	m_pcwndFontOpen = GetDlgItem(IDC_BUT_Open);
	m_pcwndFontSize = GetDlgItem(IDC_EDIT_FONT_SIZE);
	m_pcwndBackCombo = GetDlgItem(IDC_COMBO_BACK_COLOR);
	m_pcwndBackR = GetDlgItem(IDC_EDIT_BACK_R);
	m_pcwndBackG = GetDlgItem(IDC_EDIT_BACK_G);
	m_pcwndBackB = GetDlgItem(IDC_EDIT_BACK_B);
	m_pcwndBackA = GetDlgItem(IDC_EDIT_BACK_A);
	m_pcwndForeComno = GetDlgItem(IDC_COMBO_FORE_COLOR);
	m_pcwndForeR = GetDlgItem(IDC_EDIT_FORE_R);
	m_pcwndForeB = GetDlgItem(IDC_EDIT_FORE_G);
	m_pcwndForeG = GetDlgItem(IDC_EDIT_FORE_B);
	m_pcwndForeA = GetDlgItem(IDC_EDIT_FORE_A);
	m_pcwndWidth = GetDlgItem(IDC_EDIT_CC_WIDTH);
	m_pcwndHeight = GetDlgItem(IDC_EDIT_CC_HEIGHT);
	m_pcwndButRun = GetDlgItem(IDC_BUTTON1);
	m_pcwndButPause = GetDlgItem(IDC_BUT_PAUSE);
	m_pcwndButStop = GetDlgItem(IDC_BUT_STOP);

	m_pCCGrapgBuilder = new MWCCGraphBuilder();

	HWND it = AfxGetApp()->GetMainWnd()->GetSafeHwnd();
	GetDlgItem(IDC_STATIC, &it);
	m_pCCGrapgBuilder->InitVMR9(it);
	m_pCCGrapgBuilder->EnumDevices();
	CComboBox *t_cb = (CComboBox*)m_pcwndDeviceCombo;
	if (t_cb->GetCount()){
		t_cb->Clear();
	}
	t_cb->InsertString(0, L"NULL");
	for (int i = 0; i < m_pCCGrapgBuilder->m_vec_DeviceName.size(); i++){
		t_cb->InsertString(i + 1, m_pCCGrapgBuilder->m_vec_DeviceName.at(i).c_str());
	}
	t_cb->SetCurSel(0);

	m_pCCGrapgBuilder->CreateCCDecoder();
	bool t_bshow708, t_bshow608;
	m_pCCGrapgBuilder->m_pSet->GetShow608_708(&t_bshow608,&t_bshow708);
	CButton* t_check708 = (CButton*)m_pcwndCheck708;
	CButton *t_check608 = (CButton*)m_pcwndCheck608;
	int t_n608Check = t_bshow608 == true ? 1 : 0;
	int t_n708Check = t_bshow708 == true ? 1 : 0;
	t_check608->SetCheck(t_n608Check);
	t_check708->SetCheck(t_n708Check);

	CComboBox *t_ccb = (CComboBox*)m_pcwndChannel608;
	t_ccb->InsertString(0, L"All");
	t_ccb->InsertString(1, L"CC1");
	t_ccb->InsertString(2, L"CC2");
	t_ccb->InsertString(3, L"CC3");
	t_ccb->InsertString(4, L"CC4");
	int t_nField;
	int t_nChannel[3];
	m_pCCGrapgBuilder->m_pSet->GetCC608Channel(&t_nField, t_nChannel);
	if (t_nField == 0){
		t_ccb->SetCurSel(0);
		m_ntCC608ChannelIndex = 0;
		m_nCC608ChannelIndex = 0;
	}
	else{
		int t_nSel = t_nField * 2 + t_nChannel[t_nField] - 1;
		t_ccb->SetCurSel(t_nSel);
		m_ntCC608ChannelIndex = t_nSel;
		m_nCC608ChannelIndex = t_nSel;
	}

	m_pCCGrapgBuilder->m_pSet->GetFont(m_wcsFontPath);
	CEdit *t_edit = (CEdit *)m_pcwndFontFilePath;
	t_edit->SetWindowTextW(m_wcsFontPath);
	memcpy(m_wcstFontPath, m_wcsFontPath,256*sizeof(WCHAR));

	m_pCCGrapgBuilder->m_pSet->GetFontSize(&m_nFontRadio);
	m_ntFontRadio = m_nFontRadio;
	CEdit* t_editFS = (CEdit *)m_pcwndFontSize;
	SetDlgItemInt(IDC_EDIT_FONT_SIZE, m_nFontRadio, true);

	bool t_bBack = false;
	m_pCCGrapgBuilder->m_pSet->GetBacColor(&t_bBack, &m_nBackR, &m_nBackG, &m_nBackB, &m_nBackA);
	m_nBackSel = t_bBack == false ? 0 : 1;
	m_ntBackSel = m_nBackSel;
	CComboBox *t_selBack = (CComboBox *)m_pcwndBackCombo;
	t_selBack->InsertString(0, L"DEFAULT");
	t_selBack->InsertString(1, L"CUSTOM");
	t_selBack->SetCurSel(m_nBackSel);
	m_ntBackR = m_nBackR;
	m_ntBackG = m_nBackG;
	m_ntBackB = m_nBackB;
	m_ntBackA = m_nBackA;
	SetDlgItemInt(IDC_EDIT_BACK_R, m_nBackR);
	SetDlgItemInt(IDC_EDIT_BACK_G, m_nBackG);
	SetDlgItemInt(IDC_EDIT_BACK_B, m_nBackB);
	SetDlgItemInt(IDC_EDIT_BACK_A, m_nBackA);

	bool t_bFore = false;
	m_pCCGrapgBuilder->m_pSet->GetFtcColor(&t_bFore, &m_nForeR, &m_nForeG, &m_nForeB, &m_nForeA);
	m_nForeSel = t_bFore == false ? 0 : 1;
	m_ntForeSel = m_nForeSel;
	CComboBox *t_selFore = (CComboBox *)m_pcwndForeComno;
	t_selFore->InsertString(0, L"DEFAULT");
	t_selFore->InsertString(1, L"CUSTOM");
	t_selFore->SetCurSel(m_nForeSel);
	m_ntForeR = m_nForeR;
	m_ntForeG = m_nForeG;
	m_ntForeB = m_nForeB;
	m_ntForeA = m_nForeA;
	SetDlgItemInt(IDC_EDIT_FORE_R, m_nForeR);
	SetDlgItemInt(IDC_EDIT_FORE_G, m_nForeG);
	SetDlgItemInt(IDC_EDIT_FORE_B, m_nForeB);
	SetDlgItemInt(IDC_EDIT_FORE_A, m_nForeA);

	m_pCCGrapgBuilder->m_pSet->GetPicSize(&m_nWith, &m_nHeight);
	m_ntWidth = m_nWith;
	m_ntHeight = m_nHeight;
	SetDlgItemInt(IDC_EDIT_CC_WIDTH, m_nWith);
	SetDlgItemInt(IDC_EDIT_CC_HEIGHT, m_nHeight);

	FreshUIConnectCC(m_pCCGrapgBuilder->m_bConnectCC);
	FreshUIConnect(m_pCCGrapgBuilder->m_bConnect);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMWCCPlayer_DShowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMWCCPlayer_DShowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CMWCCPlayer_DShowDlg::OnSize(UINT nType,int cx,int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType == 1)
		return;

	CWnd *t_hwndStatic=GetDlgItem(IDC_STATIC);
	if (t_hwndStatic){
		ChangeSize(t_hwndStatic, cx, cy);
		HWND hwnd;
		GetDlgItem(IDC_STATIC, &hwnd);
		//CDC *cdc = t_hwndStatic->GetDC();
		//HDC hdc = cdc->GetSafeHdc();
		//m_pCCGrapgBuilder->m_pVMR9WindowlessControl->RepaintVideo(hwnd, hdc);
		RECT rec;
		t_hwndStatic->GetClientRect(&rec);
		m_pCCGrapgBuilder->m_pVMR9WindowlessControl->SetVideoPosition(NULL, &rec);
	}
	GetClientRect(&m_Rect);
}

void CMWCCPlayer_DShowDlg::ChangeSize(CWnd *hwnd, int cx, int cy)
{
	if (hwnd == NULL)
		return;
	CRect t_Rect;
	hwnd->GetWindowRect(&t_Rect);
	ScreenToClient(&t_Rect);

	t_Rect.left=t_Rect.left*cx / m_Rect.Width();
	t_Rect.right = t_Rect.right*cx / m_Rect.Width();
	t_Rect.top = t_Rect.top*cy / m_Rect.Height();
	t_Rect.bottom = t_Rect.bottom*cy / m_Rect.Height();
	hwnd->MoveWindow(t_Rect);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMWCCPlayer_DShowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMWCCPlayer_DShowDlg::OnBnClickedButton1()
{
	printf("clicked");
	OAFilterState t_State;
	HRESULT hr = m_pCCGrapgBuilder->m_pMediaControl->GetState(10, &t_State);
	if (hr == S_OK || hr == VFW_S_CANT_CUE){
		hr = E_FAIL;
		switch (t_State)
		{
		case State_Running:
			break;
		case State_Paused:
			hr=m_pCCGrapgBuilder->m_pMediaControl->Run();
			break;
		case State_Stopped:
			hr=m_pCCGrapgBuilder->m_pMediaControl->Run();
		default:
			break;
		}
	}
	if (SUCCEEDED(hr)){
		m_pcwndButRun->EnableWindow(0);
		m_pcwndButPause->EnableWindow(1);
		m_pcwndButStop->EnableWindow(1);
	}
	else{
		m_pcwndButRun->EnableWindow(0);
		m_pcwndButPause->EnableWindow(0);
		m_pcwndButStop->EnableWindow(0);
	}
}


void CMWCCPlayer_DShowDlg::OnBnClickedRadioWithCc()
{
	ReviseCCSet();
	CButton *t_ra = (CButton*)m_pcwndRadioCC;
	CButton *t_ra1 = (CButton*)m_pcwndRadioWithoutCC;
	CButton *t_ra2 = (CButton*)m_pcwndRadioSetCC;
	bool t_bcheck=t_ra->GetCheck()==0?false:true;
	if (t_bcheck != m_bCCChecked)
	{
		if (t_bcheck == true){
			HRESULT hr=m_pCCGrapgBuilder->ConnectFiltersWithCC();
			if (SUCCEEDED(hr)){
				m_bCCChecked = true;
				m_bWithoutCCChecked = false;
				m_bSetCCChecked = false;
				t_ra->SetCheck(1);
				t_ra1->SetCheck(0);
				t_ra2->SetCheck(0);
				m_pcwndButRun->EnableWindow(1);
				m_pcwndButPause->EnableWindow(0);
				m_pcwndButStop->EnableWindow(0);
			}
			else{
				m_bCCChecked = false;
				m_bWithoutCCChecked = true;
				m_bSetCCChecked = false;
				t_ra->SetCheck(0);
				t_ra1->SetCheck(1);
				t_ra2->SetCheck(0);
				m_pcwndButRun->EnableWindow(1);
				m_pcwndButPause->EnableWindow(0);
				m_pcwndButStop->EnableWindow(0);
			}
		}
	}
	FreshUIConnectCC(m_pCCGrapgBuilder->m_bConnectCC);
}

void CMWCCPlayer_DShowDlg::OnBnClickedRadioWitoutCc()
{
	ReviseCCSet();
	CButton *t_ra = (CButton*)m_pcwndRadioCC;
	CButton *t_ra1 = (CButton*)m_pcwndRadioWithoutCC;
	CButton *t_ra2 = (CButton*)m_pcwndRadioSetCC;
	bool t_bcheck = t_ra1->GetCheck() == 0 ? false : true;
	if (t_bcheck != m_bWithoutCCChecked)
	{
		if (t_bcheck == true){
			m_pCCGrapgBuilder->DisconectFiltersWithCC();
			m_bCCChecked = false;
			m_bWithoutCCChecked = true;
			m_bSetCCChecked = false;
			t_ra->SetCheck(0);
			t_ra1->SetCheck(1);
			t_ra2->SetCheck(0);
			m_pcwndButRun->EnableWindow(1);
			m_pcwndButPause->EnableWindow(0);
			m_pcwndButStop->EnableWindow(0);
		}
		else{
			m_bWithoutCCChecked = false;
			t_ra1->SetCheck(0);
			m_pcwndButRun->EnableWindow(1);
			m_pcwndButPause->EnableWindow(0);
			m_pcwndButStop->EnableWindow(0);
		}
	}
	FreshUIConnectCC(m_pCCGrapgBuilder->m_bConnectCC);
}


void CMWCCPlayer_DShowDlg::OnBnClickedButStop()
{

	OAFilterState t_State;
	HRESULT hr = m_pCCGrapgBuilder->m_pMediaControl->GetState(10, &t_State);
	if (hr == S_OK || hr == VFW_S_CANT_CUE){
		hr = E_FAIL;
		switch (t_State)
		{
		case State_Running:
			hr=m_pCCGrapgBuilder->m_pMediaControl->Stop();
			break;
		case State_Paused:
			hr=m_pCCGrapgBuilder->m_pMediaControl->Stop();
			break;
		case State_Stopped:
		default:
			break;
		}
	}
	if (SUCCEEDED(hr)){
		m_pcwndButRun->EnableWindow(1);
		m_pcwndButPause->EnableWindow(0);
		m_pcwndButStop->EnableWindow(0);
	}
	else{
		m_pcwndButRun->EnableWindow(0);
		m_pcwndButPause->EnableWindow(0);
		m_pcwndButStop->EnableWindow(0);
	}
}


void CMWCCPlayer_DShowDlg::OnBnClickedButPause()
{

	OAFilterState t_State;
	HRESULT hr = m_pCCGrapgBuilder->m_pMediaControl->GetState(10, &t_State);
	if (hr == S_OK || hr == VFW_S_CANT_CUE){
		hr = E_FAIL;
		switch (t_State)
		{
		case State_Running:
			hr=m_pCCGrapgBuilder->m_pMediaControl->Pause();
			break;
		case State_Paused:
			hr = S_OK;
			break;
		case State_Stopped:
		default:
			break;
		}
	}
	if (SUCCEEDED(hr)){
		m_pcwndButRun->EnableWindow(1);
		m_pcwndButPause->EnableWindow(0);
		m_pcwndButStop->EnableWindow(1);
	}
	else{
		m_pcwndButRun->EnableWindow(0);
		m_pcwndButPause->EnableWindow(0);
		m_pcwndButStop->EnableWindow(0);
	}
}


void CMWCCPlayer_DShowDlg::OnCbnSelchangeComboDevices()
{
	CComboBox *t_cb = (CComboBox*)m_pcwndDeviceCombo;
	m_ntDevIndex=t_cb->GetCurSel();
}


void CMWCCPlayer_DShowDlg::OnBnClickedButOk()
{
	bool t_bDevChange = false;
	bool t_bShowCCChange = false;
	bool t_bChannelIndexChange = false;
	bool t_bFontChange = false;
	bool t_bFontSizeChange = false;
	bool t_bBackColorChange = false;
	bool t_bForeColorChange = false;
	bool t_bSizeChange = false;

	if (m_nDevIndex != m_ntDevIndex){
		t_bDevChange = true;
	}
	if (m_btShow608 != m_bShow608 || m_btShow708 != m_bShow708){
		t_bShowCCChange = true;
	}
	if (m_ntCC608ChannelIndex != m_nCC608ChannelIndex)
		t_bChannelIndexChange = true;
	if (wcscmp(m_wcsFontPath, m_wcstFontPath) != 0){
		t_bFontChange = true;
	}
	if (m_ntFontRadio != m_nFontRadio)
		t_bFontSizeChange = true;
	if (m_nBackSel != m_ntBackSel || m_nBackR != m_nBackR || m_ntBackG != m_nBackG || m_ntBackB != m_nBackB || m_nBackA != m_ntBackA){
		t_bBackColorChange = true;
	}
	if (m_nForeSel != m_ntForeSel || m_nForeR != m_ntForeR || m_nForeG != m_ntForeG || m_nForeB != m_ntForeB || m_nForeA != m_ntForeA){
		t_bForeColorChange = true;
	}
	if (m_ntWidth != m_nWith || m_ntHeight != m_nHeight){
		t_bSizeChange = true;
	}

	if (t_bDevChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			m_pCCGrapgBuilder->DisconectFiltersWithCC();
		}
		if (m_pCCGrapgBuilder->m_bConnect){
			m_pCCGrapgBuilder->DisconnectFilters();
		}
		if (m_ntDevIndex == 0){
			m_nDevIndex = 0;
		}
		else{
			HRESULT hr = m_pCCGrapgBuilder->ConnectFilters(m_ntDevIndex);
			FreshUIConnect(m_pCCGrapgBuilder->m_bConnect);
			if (SUCCEEDED(hr)){
				m_nDevIndex = m_ntDevIndex;
				m_pcwndButRun->EnableWindow(1);
				m_pcwndButPause->EnableWindow(0);
				m_pcwndButStop->EnableWindow(0);
			}
			else{
				m_ntDevIndex = m_nDevIndex;
				m_pcwndButRun->EnableWindow(0);
				m_pcwndButPause->EnableWindow(0);
				m_pcwndButStop->EnableWindow(0);
			}
		}
		
		CButton *t_ra = (CButton *)m_pcwndRadioCC;
		CButton *t_ra1 = (CButton*)m_pcwndRadioWithoutCC;
		CButton *t_ra2 = (CButton*)m_pcwndRadioSetCC;
		if (m_pCCGrapgBuilder->m_bConnectCC){
			t_ra->SetCheck(1);
			t_ra1->SetCheck(0);	
			t_ra2->SetCheck(0);
		}
		else{
			t_ra->SetCheck(0);
			t_ra1->SetCheck(1);
			t_ra2->SetCheck(0);
		}
	}

	if (t_bShowCCChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				HRESULT hr=m_pCCGrapgBuilder->m_pSet->SetShow608_708(m_btShow608, m_btShow708);
				if (SUCCEEDED(hr)){
					m_bShow608 = m_btShow608;
					m_bShow708 = m_btShow708;
				}
				else{
					m_btShow608 = m_bShow608;
					m_btShow708 = m_bShow708;
				}
			}
		}
	}

	if (t_bChannelIndexChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				int t_nField = 0;
				int t_nChannel = 0;
				t_nField = (m_ntCC608ChannelIndex + 1) / 2;
				t_nChannel = (m_ntCC608ChannelIndex + 1) % 2;
				HRESULT hr=m_pCCGrapgBuilder->m_pSet->SetCC608Channel(t_nField, t_nChannel);
				if (SUCCEEDED(hr)){
					m_nCC608ChannelIndex = m_ntCC608ChannelIndex;
				}
				else{
					m_ntCC608ChannelIndex = m_nCC608ChannelIndex;
				}
			}
		}
	}

	if (t_bFontChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				HRESULT hr = m_pCCGrapgBuilder->m_pSet->SetFont(m_wcstFontPath);
				if (SUCCEEDED(hr)){
					memcpy(m_wcsFontPath, m_wcstFontPath, 256 * sizeof(WCHAR));
				}
				else{
					memcpy(m_wcstFontPath, m_wcsFontPath, 256 * sizeof(WCHAR));
				}
			}
		}
	}

	if (t_bFontSizeChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				HRESULT hr = m_pCCGrapgBuilder->m_pSet->SetFontSize(m_ntFontRadio);
				if (SUCCEEDED(hr)){
					m_nFontRadio = m_ntFontRadio;
				}
				else{
					m_ntFontRadio = m_nFontRadio;
				}
			}
		}
	}

	if (t_bBackColorChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				bool t_nSet = m_ntBackSel == 0 ? false : true;
				HRESULT hr = m_pCCGrapgBuilder->m_pSet->SetBacColor(t_nSet, m_ntBackR, m_ntBackG, m_ntBackB, m_ntBackA);
				if (SUCCEEDED(hr)){
					m_nBackSel = m_ntBackSel;
					m_nBackR = m_ntBackR;
					m_nBackG = m_ntBackG;
					m_nBackB = m_ntBackB;
					m_nBackA = m_ntBackA;
				}
				else{
					m_ntBackSel = m_nBackSel;
					m_ntBackR = m_nBackR;
					m_ntBackG = m_nBackG;
					m_ntBackB = m_nBackB;
					m_ntBackA = m_nBackA;
				}
			}
		}
	}

	if (t_bForeColorChange){
		if (m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_pSet){
				bool t_bSet = m_ntForeSel == 0 ? false : true;
				HRESULT hr = m_pCCGrapgBuilder->m_pSet->SetFtcColor(t_bSet, m_ntForeR, m_ntForeG, m_ntForeB, m_ntForeA);
				if (SUCCEEDED(hr)){
					m_nForeSel = m_ntForeSel;
					m_nForeR = m_ntForeR;
					m_nForeG = m_ntForeG;
					m_nForeB = m_ntForeB;
					m_nForeA = m_ntForeA;
				}
				else{
					m_ntForeSel = m_nForeSel;
					m_ntForeR = m_nForeR;
					m_ntForeG = m_nForeG;
					m_ntForeB = m_nForeB;
					m_ntForeA = m_nForeA;
				}
			}
		}
	}

	if (t_bSizeChange){
		if (!m_pCCGrapgBuilder->m_bConnectCC){
			if (m_pCCGrapgBuilder->m_bConnect){
				if (m_pCCGrapgBuilder->m_pSet){
					HRESULT hr = m_pCCGrapgBuilder->m_pSet->SetPicSize(m_ntWidth, m_ntHeight);
					if (SUCCEEDED(hr)){
						m_nWith = m_ntWidth;
						m_nHeight = m_ntHeight;
					}
					else{
						m_ntWidth = m_nWith;
						m_ntHeight = m_nHeight;
					}
				}
			}
		}
	}
}


void CMWCCPlayer_DShowDlg::OnBnClickedButCancel()
{
	
}


void CMWCCPlayer_DShowDlg::OnBnClickedCheckCc708()
{
	CButton *t_check708 = (CButton *)m_pcwndCheck708;
	if (t_check708->GetCheck())
		m_btShow708 = true;
	else
		m_btShow708 = false;
}


void CMWCCPlayer_DShowDlg::OnBnClickedCheckCc608()
{
	CButton *t_check608 = (CButton *)m_pcwndCheck608;
	if (t_check608->GetCheck()){
		m_btShow608 = true;
		m_pcwndChannel608->EnableWindow(1);
	}
	else{
		m_btShow608 = false;
		m_pcwndChannel608->EnableWindow(0);
	}
}


void CMWCCPlayer_DShowDlg::OnCbnSelchangeComboCcChannel()
{
	CComboBox *t_cb = (CComboBox*)m_pcwndChannel608;
	m_ntCC608ChannelIndex = t_cb->GetCurSel();
}


void CMWCCPlayer_DShowDlg::OnBnClickedButOpen()
{
	OPENFILENAME ofn;
	TCHAR szFileName[MAX_PATH] = _T("");
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = this->m_hWnd;
	ofn.lpstrFilter = _T("Font Files (*.ttf)\0*.ttf\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = _T("xml");
	ofn.lpstrInitialDir = _T(".\\");
	if (GetOpenFileName(&ofn))
	{
		memset(m_wcstFontPath, 0, 256 * sizeof(WCHAR));
		memcpy(m_wcstFontPath, ofn.lpstrFile, sizeof(WCHAR)*(256));
		CEdit *t_edit = (CEdit *)m_pcwndFontFilePath;
		t_edit->SetWindowTextW(m_wcstFontPath);
	}
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditFontSize()
{
	m_ntFontRadio = GetDlgItemInt(IDC_EDIT_FONT_SIZE);
}


void CMWCCPlayer_DShowDlg::OnCbnSelchangeComboBackColor()
{
	CComboBox *t_selback = (CComboBox *)m_pcwndBackCombo;
	m_ntBackSel=t_selback->GetCurSel();
	if (m_ntBackSel == 0){
		m_pcwndBackR->EnableWindow(0);
		m_pcwndBackG->EnableWindow(0);
		m_pcwndBackB->EnableWindow(0);
		m_pcwndBackA->EnableWindow(0);
	}
	else{
		m_pcwndBackR->EnableWindow(1);
		m_pcwndBackG->EnableWindow(1);
		m_pcwndBackB->EnableWindow(1);
		m_pcwndBackA->EnableWindow(1);
	}
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditBackR()
{
	m_ntBackR=GetDlgItemInt(IDC_EDIT_BACK_R);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditBackG()
{
	m_ntBackG = GetDlgItemInt(IDC_EDIT_BACK_G);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditBackB()
{
	m_ntBackB = GetDlgItemInt(IDC_EDIT_BACK_B);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditBackA()
{
	m_ntBackA = GetDlgItemInt(IDC_EDIT_BACK_A);
}


void CMWCCPlayer_DShowDlg::OnCbnSelchangeComboForeColor()
{
	CComboBox *t_selfore = (CComboBox *)m_pcwndForeComno;
	m_ntForeSel = t_selfore->GetCurSel();
	if (m_ntForeSel == 0){
		m_pcwndForeR->EnableWindow(0);
		m_pcwndForeG->EnableWindow(0);
		m_pcwndForeB->EnableWindow(0);
		m_pcwndForeA->EnableWindow(0);
	}
	else{
		m_pcwndForeR->EnableWindow(1);
		m_pcwndForeG->EnableWindow(1);
		m_pcwndForeB->EnableWindow(1);
		m_pcwndForeA->EnableWindow(1);
	}
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditForeR()
{
	m_ntForeR = GetDlgItemInt(IDC_EDIT_FORE_R);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditForeG()
{
	m_ntForeG = GetDlgItemInt(IDC_EDIT_FORE_G);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditForeB()
{
	m_ntForeB = GetDlgItemInt(IDC_EDIT_FORE_B);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditForeA()
{
	m_ntForeA = GetDlgItemInt(IDC_EDIT_FORE_A);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditCcWidth()
{
	m_ntWidth = GetDlgItemInt(IDC_EDIT_CC_WIDTH);
}


void CMWCCPlayer_DShowDlg::OnEnChangeEditCcHeight()
{
	m_ntHeight = GetDlgItemInt(IDC_EDIT_CC_HEIGHT);
}

void CMWCCPlayer_DShowDlg::FreshUIConnectCC(bool t_bConnectCC)
{
	BOOL t_enable = t_bConnectCC == true ? 1 : 0;
	if (t_enable == 0){
		m_pcwndCheck708->EnableWindow(t_enable);
		m_pcwndCheck608->EnableWindow(t_enable);
		m_pcwndChannel608->EnableWindow(t_enable);
		m_pcwndFontFilePath->EnableWindow(t_enable);
		m_pcwndFontOpen->EnableWindow(t_enable);
		m_pcwndFontSize->EnableWindow(t_enable);
		m_pcwndBackCombo->EnableWindow(t_enable);
		m_pcwndBackR->EnableWindow(t_enable);
		m_pcwndBackG->EnableWindow(t_enable);
		m_pcwndBackB->EnableWindow(t_enable);
		m_pcwndBackA->EnableWindow(t_enable);
		m_pcwndForeComno->EnableWindow(t_enable);
		m_pcwndForeR->EnableWindow(t_enable);
		m_pcwndForeG->EnableWindow(t_enable);
		m_pcwndForeB->EnableWindow(t_enable);
		m_pcwndForeA->EnableWindow(t_enable);
		m_pcwndWidth->EnableWindow(t_enable);
		m_pcwndHeight->EnableWindow(t_enable);
	}
	else{
		m_pcwndCheck708->EnableWindow(t_enable);
		m_pcwndCheck608->EnableWindow(t_enable);
		CButton *t_chec608 = (CButton *)m_pcwndCheck608;
		if (t_chec608->GetCheck()){
			m_pcwndChannel608->EnableWindow(t_enable);
		}
		else{
			m_pcwndChannel608->EnableWindow(0);
		}
		m_pcwndFontFilePath->EnableWindow(t_enable);
		m_pcwndFontOpen->EnableWindow(t_enable);
		m_pcwndFontSize->EnableWindow(t_enable);
		m_pcwndBackCombo->EnableWindow(t_enable);
		if (m_nBackSel == 0){
			m_pcwndBackR->EnableWindow(0);
			m_pcwndBackG->EnableWindow(0);
			m_pcwndBackB->EnableWindow(0);
			m_pcwndBackA->EnableWindow(0);
		}
		else{
			m_pcwndBackR->EnableWindow(t_enable);
			m_pcwndBackG->EnableWindow(t_enable);
			m_pcwndBackB->EnableWindow(t_enable);
			m_pcwndBackA->EnableWindow(t_enable);
		}
		m_pcwndForeComno->EnableWindow(t_enable);
		if (m_nForeSel == 0){
			m_pcwndForeR->EnableWindow(0);
			m_pcwndForeG->EnableWindow(0);
			m_pcwndForeB->EnableWindow(0);
			m_pcwndForeA->EnableWindow(0);
		}
		else{
			m_pcwndForeR->EnableWindow(t_enable);
			m_pcwndForeG->EnableWindow(t_enable);
			m_pcwndForeB->EnableWindow(t_enable);
			m_pcwndForeA->EnableWindow(t_enable);
		}
		bool t_bConnected = false;
		m_pCCGrapgBuilder->m_pSet->GetConnected(&t_bConnected);
		if (t_bConnected){
			m_pcwndWidth->EnableWindow(0);
			m_pcwndHeight->EnableWindow(0);
		}
		else{
			m_pcwndWidth->EnableWindow(t_enable);
			m_pcwndHeight->EnableWindow(t_enable);
		}
	}

}
void CMWCCPlayer_DShowDlg::FreshUIConnect(bool t_bConnect)
{
	BOOL t_enable = t_bConnect == true ? 1 : 0;
	m_pcwndRadioCC->EnableWindow(t_enable);
	m_pcwndRadioWithoutCC->EnableWindow(t_enable);
	m_pcwndRadioSetCC->EnableWindow(t_enable);
	m_pcwndButRun->EnableWindow(t_enable);
	m_pcwndButPause->EnableWindow(t_enable);
	m_pcwndButStop->EnableWindow(t_enable);
}

void CMWCCPlayer_DShowDlg::OnBnClickedRadioSetCc()
{
	ReviseCCSet();
	CButton *t_ra = (CButton*)m_pcwndRadioCC;
	CButton *t_ra1 = (CButton*)m_pcwndRadioWithoutCC;
	CButton *t_ra2 = (CButton*)m_pcwndRadioSetCC;
	bool t_bcheck = t_ra2->GetCheck() == 0 ? false : true;
	if (t_bcheck != m_bSetCCChecked)
	{
		if (t_bcheck == true){
			if (m_pCCGrapgBuilder->m_bConnectCC){
				m_pCCGrapgBuilder->DisconectFiltersWithCC();
			}
			BOOL t_enable = 1;
			m_pcwndCheck708->EnableWindow(t_enable);
			m_pcwndCheck608->EnableWindow(t_enable);
			m_pcwndChannel608->EnableWindow(t_enable);
			m_pcwndFontFilePath->EnableWindow(t_enable);
			m_pcwndFontOpen->EnableWindow(t_enable);
			m_pcwndFontSize->EnableWindow(t_enable);

			m_pcwndBackCombo->EnableWindow(t_enable);
			CComboBox *t_cback = (CComboBox*)m_pcwndBackCombo;
			int t_nBackSel = t_cback->GetCurSel();
			if (t_nBackSel == 0){
				m_pcwndBackR->EnableWindow(0);
				m_pcwndBackG->EnableWindow(0);
				m_pcwndBackB->EnableWindow(0);
				m_pcwndBackA->EnableWindow(0);
			}
			else{
				m_pcwndBackR->EnableWindow(1);
				m_pcwndBackG->EnableWindow(1);
				m_pcwndBackB->EnableWindow(1);
				m_pcwndBackA->EnableWindow(1);
			}

			m_pcwndForeComno->EnableWindow(t_enable);
			CComboBox *t_cfore = (CComboBox*)m_pcwndForeComno;
			int t_nForeSel = t_cfore->GetCurSel();
			if (t_nForeSel == 0){
				m_pcwndForeR->EnableWindow(0);
				m_pcwndForeG->EnableWindow(0);
				m_pcwndForeB->EnableWindow(0);
				m_pcwndForeA->EnableWindow(0);
			}
			else{
				m_pcwndForeR->EnableWindow(1);
				m_pcwndForeG->EnableWindow(1);
				m_pcwndForeB->EnableWindow(1);
				m_pcwndForeA->EnableWindow(1);
			}


			m_pcwndWidth->EnableWindow(t_enable);
			m_pcwndHeight->EnableWindow(t_enable);

			m_pcwndButRun->EnableWindow(0);
			m_pcwndButPause->EnableWindow(0);
			m_pcwndButStop->EnableWindow(0);
			t_ra->SetCheck(0);
			t_ra1->SetCheck(0);
			t_ra2->SetCheck(1);
			m_bCCChecked = false;
			m_bWithoutCCChecked = false;
			m_bSetCCChecked = true;
		}
		else{
			m_bSetCCChecked = false;
			t_ra2->SetCheck(0);
		}
	}

}

void CMWCCPlayer_DShowDlg::ReviseCCSet(){
	if (m_nDevIndex != m_ntDevIndex){
		m_ntDevIndex = m_nDevIndex;
		CComboBox *t_cdev = (CComboBox*)m_pcwndDeviceCombo;
		t_cdev->SetCurSel(m_nDevIndex);
	}
	if (m_btShow608 != m_bShow608 || m_btShow708 != m_bShow708){
		CButton *t_but608 = (CButton*)m_pcwndCheck608;
		CComboBox *t_ch608 = (CComboBox*)m_pcwndChannel608;
		m_btShow608 = m_bShow608;
		m_ntCC608ChannelIndex = m_nCC608ChannelIndex;
		if (m_bShow608){
			t_but608->SetCheck(1);
			t_ch608->SetCurSel(m_nCC608ChannelIndex);
		}
		else{
			t_but608->SetCheck(0);
			t_ch608->SetCurSel(m_nCC608ChannelIndex);
		}

		CButton *t_but708 = (CButton*)m_pcwndCheck708;
		m_btShow708 = m_bShow708;
		if (m_bShow708)
			t_but708->SetCheck(1);
		else
			t_but708->SetCheck(0);
	}

	if (wcscmp(m_wcsFontPath, m_wcstFontPath) != 0){
		memcpy(m_wcstFontPath, m_wcsFontPath, sizeof(WCHAR)* 256);
		CEdit *t_edit = (CEdit*)m_pcwndFontFilePath;
		t_edit->SetWindowTextW(m_wcstFontPath);
	}

	if (m_ntFontRadio != m_nFontRadio){
		m_ntFontRadio = m_nFontRadio;
		SetDlgItemInt(IDC_EDIT_FONT_SIZE, m_nFontRadio);
	}
	if (m_nBackSel != m_ntBackSel || m_nBackR != m_nBackR || m_ntBackG != m_nBackG || m_ntBackB != m_nBackB || m_nBackA != m_ntBackA){
		m_ntBackSel = m_nBackSel;
		CComboBox *t_back = (CComboBox*)m_pcwndBackCombo;
		t_back->SetCurSel(m_nBackSel);
		m_ntBackR = m_nBackR;
		SetDlgItemInt(IDC_EDIT_BACK_R, m_nBackR);
		m_ntBackG = m_nBackG;
		SetDlgItemInt(IDC_EDIT_BACK_G, m_nBackG);
		m_ntBackB = m_nBackB;
		SetDlgItemInt(IDC_EDIT_BACK_B, m_nBackB);
		m_ntBackA = m_nBackA;
		SetDlgItemInt(IDC_EDIT_BACK_A, m_nBackA);
	}
	if (m_nForeSel != m_ntForeSel || m_nForeR != m_ntForeR || m_nForeG != m_ntForeG || m_nForeB != m_ntForeB || m_nForeA != m_ntForeA){
		m_ntForeSel = m_nForeSel;
		CComboBox *t_fore = (CComboBox*)m_pcwndForeComno;
		t_fore->SetCurSel(m_nForeSel);
		m_ntForeR = m_nForeR;
		SetDlgItemInt(IDC_EDIT_FORE_R, m_nForeR);
		m_ntForeG = m_nForeG;
		SetDlgItemInt(IDC_EDIT_FORE_G, m_nForeG);
		m_ntForeB = m_nForeB;
		SetDlgItemInt(IDC_EDIT_FORE_B, m_nForeB);
		m_ntForeA = m_nForeA;
		SetDlgItemInt(IDC_EDIT_FORE_A, m_nForeA);
	}
	if (m_ntWidth != m_nWith || m_ntHeight != m_nHeight){
		m_nWith = m_ntWidth;
		SetDlgItemInt(IDC_EDIT_CC_WIDTH, m_nWith);
		m_ntHeight = m_nHeight;
		SetDlgItemInt(IDC_EDIT_CC_HEIGHT, m_nHeight);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditFontSize()
{
	m_ntFontRadio = GetDlgItemInt(IDC_EDIT_FONT_SIZE);
	if (!(m_ntFontRadio >= 50 && m_ntFontRadio <= 100)){
		SetDlgItemInt(IDC_EDIT_FONT_SIZE, 100);
		m_ntFontRadio = 100;
		MessageBox(L"Font size is out of range!\nIt's now is setted to 100.\nIt's arrange is 50 to 100.\n", L"Font size warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackR()
{
	m_ntBackR = GetDlgItemInt(IDC_EDIT_BACK_R);
	if (!(m_ntBackR >= 0 && m_ntBackR <= 255)){
		SetDlgItemInt(IDC_EDIT_BACK_R, 255);
		m_ntBackR = 255;
		MessageBox(L"Background color's r is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Background color's r warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackG()
{
	m_ntBackG = GetDlgItemInt(IDC_EDIT_BACK_G);
	if (!(m_ntBackG >= 0 && m_ntBackG <= 255)){
		SetDlgItemInt(IDC_EDIT_BACK_G, 255);
		m_ntBackG = 255;
		MessageBox(L"Background color's g is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Background color's g warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackB()
{
	m_ntBackB = GetDlgItemInt(IDC_EDIT_BACK_B);
	if (!(m_ntBackB >= 0 && m_ntBackB <= 255)){
		SetDlgItemInt(IDC_EDIT_BACK_B, 255);
		m_ntBackB = 255;
		MessageBox(L"Background color's b is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Background color's b warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditBackA()
{
	m_ntBackA = GetDlgItemInt(IDC_EDIT_BACK_A);
	if (!(m_ntBackA >= 0 && m_ntBackA <= 255)){
		SetDlgItemInt(IDC_EDIT_BACK_A, 255);
		m_ntBackA = 255;
		MessageBox(L"Background color's a is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Background color's a warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeR()
{
	m_ntForeR = GetDlgItemInt(IDC_EDIT_FORE_R);
	if (!(m_ntForeR >= 0 && m_ntForeR <= 255)){
		SetDlgItemInt(IDC_EDIT_FORE_R, 255);
		m_ntForeR = 255;
		MessageBox(L"Foreground color's r is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Fore color's r warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeG()
{
	m_ntForeG = GetDlgItemInt(IDC_EDIT_FORE_G);
	if (!(m_ntForeG >= 0 && m_ntForeG <= 255)){
		SetDlgItemInt(IDC_EDIT_FORE_G, 255);
		m_ntForeG = 255;
		MessageBox(L"Foreground color's g is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Fore color's g warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeB()
{
	m_ntForeB = GetDlgItemInt(IDC_EDIT_FORE_B);
	if (!(m_ntForeB >= 0 && m_ntForeB <= 255)){
		SetDlgItemInt(IDC_EDIT_FORE_B, 255);
		m_ntForeB = 255;
		MessageBox(L"Foreground color's b is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Fore color's b warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditForeA()
{
	m_ntForeA = GetDlgItemInt(IDC_EDIT_FORE_A);
	if (!(m_ntForeA >= 0 && m_ntForeA <= 255)){
		SetDlgItemInt(IDC_EDIT_FORE_A, 255);
		m_ntForeA = 255;
		MessageBox(L"Foreground color's a is out of range!\nIt's now is setted to 255.\nIt's arrange is 0 to 255.\n", L"Fore color's a warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditCcWidth()
{
	m_ntWidth = GetDlgItemInt(IDC_EDIT_CC_WIDTH);
	if (!(m_ntWidth >= 160 && m_ntWidth <= 1920)){
		SetDlgItemInt(IDC_EDIT_CC_WIDTH, 720);
		m_ntWidth = 720;
		MessageBox(L"CC graph's width is out of range!\nIt's now is setted to 720.\nIt's arrange is 160 to 1920.\n", L"CC graph's width warning", MB_OK);
	}
}


void CMWCCPlayer_DShowDlg::OnEnKillfocusEditCcHeight()
{
	m_ntHeight = GetDlgItemInt(IDC_EDIT_CC_HEIGHT);
	if (!(m_ntHeight >= 90 && m_ntHeight <= 1080)){
		SetDlgItemInt(IDC_EDIT_CC_HEIGHT, 480);
		m_ntHeight = 480;
		MessageBox(L"CC graph's height is out of range!\nIt's now is setted to 480.\nIt's arrange is 90 to 1080.\n", L"CC graph's height warning", MB_OK);
	}
}
