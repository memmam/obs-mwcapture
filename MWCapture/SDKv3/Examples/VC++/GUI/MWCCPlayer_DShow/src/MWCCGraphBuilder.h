/************************************************************************************************/
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

#ifndef MWCCGRAPHBUILDER_H
#define MWCCGRAPHBUILDER_H

#include <vector>

#include "dshow.h"
#include "D3d9.h"
#include "Vmr9.h"
#include "mw_cc708_dshow_decoder_uid.h"
#include "mw_iidecoder_set.h"
#include <string>
#pragma comment(lib, "strmiids.lib")

using namespace std;

class MWCCGraphBuilder {
public:
	MWCCGraphBuilder();
	~MWCCGraphBuilder();

public:
	IGraphBuilder *m_pGraphBuilder;
	IMediaControl *m_pMediaControl;
	IMediaEvent *m_pMediaEvent;

	IBaseFilter *m_pVMRRender9;
	IVMRFilterConfig9 *m_pVMR9Config;
	IVMRFilterConfig *m_pConfig;
	IVMRWindowlessControl *m_pWindowlessControl;
	IVMRWindowlessControl9 *m_pVMR9WindowlessControl;
	IPin *m_pVMR9PinPreview;
	IPin *m_pVMR9PinCC;

	vector<IBaseFilter *> m_vecDevices;
	vector<IPropertyBag *> m_vecPropBag;
	vector<wstring> m_vec_DeviceName;
	vector<string> m_vec_strDeviceName;
	IBaseFilter *m_pSDIIn;
	IPin *m_pDevPinPreview;
	IPin *m_pDevPinCC;

	IBaseFilter *m_pCCDecoder;
	IIDecoderSet *m_pSet;
	IPin *m_pCCPinIn;
	IPin *m_pCCPinOut;

	bool m_bWithCC;
	bool m_bHasCC;
	bool m_bConnect;
	bool m_bConnectCC;

public:
	HRESULT InitVMR9(HWND hwndApp);
	HRESULT EnumDevices();
	HRESULT CreateCCDecoder();

	HRESULT ConnectFilters(int i);
	void DisconnectFilters();
	HRESULT ConnectFiltersWithCC();
	void DisconectFiltersWithCC();
};

#endif