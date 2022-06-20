#ifndef MWCEA708DECODERPROP_H
#define MWCEA708DECODERPROP_H
////////////////////////////////////////////////////////////////////////////////
// CONFIDENTIAL and PROPRIETARY software of Magewell Electronics Co., Ltd.
// Copyright (c) 2011-2020 Magewell Electronics Co., Ltd. (Nanjing)
// All rights reserved.
// This copyright notice MUST be reproduced on all authorized copies.
////////////////////////////////////////////////////////////////////////////////

#include <streams.h>
#include "mw_iidecoder_set.h"

class MWCEA708DecoderProp : public CBasePropertyPage {
public:
	static CUnknown *WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	~MWCEA708DecoderProp();

private:
	INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam,
				 LPARAM lParam);
	HRESULT OnConnect(IUnknown *pUnknown);
	HRESULT OnDisconnect();
	HRESULT OnActivate();
	HRESULT OnDeactivate();
	HRESULT OnApplyChanges();

	void GetControlValues(); //it may be fixed

	MWCEA708DecoderProp(LPUNKNOWN lpunk, HRESULT *phr);

	BOOL m_bIsInitialized; //used to ignore startup message

	IIDecoderSet *m_pDecoderSet;
	WCHAR m_wcsFontPath[256];
	WCHAR m_wcsFontPathT[256];

	int m_nFontSize;
	int m_nFontSize_T;

	int m_nWidth;
	int m_nHeight;

	int m_nWidthT;
	int m_nHeightT;

	int m_nField;
	int m_nChannel[3];

	int m_nFieldT;
	int m_nChannelT[3];

	void *dlg;

	bool m_bSetBacColor;
	int m_nSetBacColor;
	int m_nBac_r;
	int m_nBac_g;
	int m_nBac_b;
	int m_nBac_a;

	bool m_bSetBacColor_T;
	int m_nSetBacColor_T;
	int m_nBac_r_T;
	int m_nBac_g_T;
	int m_nBac_b_T;
	int m_nBac_a_T;

	bool m_bSetFonColor;
	int m_nSetFonColor;
	int m_nFtc_r;
	int m_nFtc_g;
	int m_nFtc_b;
	int m_nFtc_a;

	bool m_bSetFonColor_T;
	int m_nSetFonColor_T;
	int m_nFtc_r_T;
	int m_nFtc_g_T;
	int m_nFtc_b_T;
	int m_nFtc_a_T;

	bool m_bShow708;
	bool m_bShow708_T;

	bool m_bShow608;
	bool m_bShow608_T;
};
#endif