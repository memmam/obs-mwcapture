/************************************************************************************************/
// MWMMDevice.h : interface of the MWMMDevice class

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

#ifndef MWMMDEVICE_H
#define MWMMDEVICE_H

#include "MWMultiDSDefine.h"
#include "Mmdeviceapi.h"


class MWMMDevice:public IMMNotificationClient{
public:
	MWMMDevice();
	~MWMMDevice();

public:
	mw_multi_ds_status_e initilize();

	mw_multi_ds_status_e deinitilize();

	mw_multi_ds_status_e enum_device();

	mw_multi_ds_status_e get_default_device(mw_mmdevice_t &t_device);

	mw_multi_ds_status_e get_default_device(IMMDevice **t_pp_device);

	mw_multi_ds_status_e get_select_device(int t_n_select,IMMDevice **t_pp_device);

	mw_multi_ds_status_e get_devices(int &t_n_num,mw_mmdevice_t *t_p_device);

	mw_multi_ds_status_e register_notification();

	mw_multi_ds_status_e set_default_device_changed_callback(LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK t_callback,void *t_p_param);

	mw_multi_ds_status_e unregister_notification();


	//IMMNotificationClient Impl
	HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( 
		/* [annotation][in] */ 
		__in  LPCWSTR pwstrDeviceId,
		/* [annotation][in] */ 
		__in  DWORD dwNewState);

	HRESULT STDMETHODCALLTYPE OnDeviceAdded( 
		/* [annotation][in] */ 
		__in  LPCWSTR pwstrDeviceId);

	HRESULT STDMETHODCALLTYPE OnDeviceRemoved( 
		/* [annotation][in] */ 
		__in  LPCWSTR pwstrDeviceId);

	HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( 
		/* [annotation][in] */ 
		__in  EDataFlow flow,
		/* [annotation][in] */ 
		__in  ERole role,
		/* [annotation][in] */ 
		__in  LPCWSTR pwstrDefaultDeviceId);

	HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( 
		/* [annotation][in] */ 
		__in  LPCWSTR pwstrDeviceId,
		/* [annotation][in] */ 
		__in  const PROPERTYKEY key);

	//IUnknown Impl
	HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);

	ULONG STDMETHODCALLTYPE AddRef( void);

	ULONG STDMETHODCALLTYPE Release( void);

protected:
	LONG m_l_ref;

	IMMDeviceEnumerator *m_p_device_enumrator;
	IMMDeviceCollection *m_p_device_collection;


	int m_n_device_count;
	mw_mmdevice_t m_arr_device[128];

	LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK m_func_device_changed_callback;
	void *m_p_caller;
	bool m_b_registernotification;

};


#endif