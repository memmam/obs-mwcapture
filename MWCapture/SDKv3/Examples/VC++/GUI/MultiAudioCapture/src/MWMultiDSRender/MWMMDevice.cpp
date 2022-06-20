/************************************************************************************************/
// MWMMDevice.cpp : implementation of the MWMMDevice class

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
#include "MWMMDevice.h"
#include "functiondiscovery.h"

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

MWMMDevice::MWMMDevice():
m_l_ref(1)
{
	m_p_device_enumrator=NULL;
	m_p_device_collection=NULL;
	m_b_registernotification=false;
	m_func_device_changed_callback=NULL;
	m_p_caller=NULL;

	m_n_device_count=0;
	for(int i=0;i<128;i++){
		memset(m_arr_device[i].m_wcs_id,0,128*sizeof(WCHAR));
		memset(m_arr_device[i].m_wcs_name,0,128*sizeof(WCHAR));
	}
}

MWMMDevice::~MWMMDevice()
{

}

mw_multi_ds_status_e MWMMDevice::initilize()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	HRESULT t_hr=CoInitializeEx(NULL,COINIT_MULTITHREADED);
	if(FAILED(t_hr)){
		if(t_hr!=RPC_E_CHANGED_MODE){
			t_ret=MW_AUDIO_COM_FAILED;
			return t_ret;
		}
	}

	t_hr=CoCreateInstance(__uuidof(MMDeviceEnumerator),NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&m_p_device_enumrator));
	if(FAILED(t_hr)){
		t_ret=MW_AUDIO_MMDEVICE_ENUM_FAILED;
		return t_ret;
	}

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::deinitilize()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	 CoUninitialize();
	 return t_ret;
}

mw_multi_ds_status_e MWMMDevice::enum_device()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	if(m_p_device_enumrator==NULL){
		t_ret=MW_AUDIO_MMDEVICE_ENUM_FAILED;
		return t_ret;
	}

	/*
	#define DEVICE_STATE_ACTIVE      0x00000001		ÒÑ¼¤»îµÄÉè±¸
	#define DEVICE_STATE_DISABLED    0x00000002		½ûÓÃµÄÉè±¸
	#define DEVICE_STATE_NOTPRESENT  0x00000004		²»´æÔÚµÄÉè±¸
	#define DEVICE_STATE_UNPLUGGED   0x00000008		Î´½ÓµçµÄÉè±¸
	#define DEVICE_STATEMASK_ALL     0x0000000f		ËùÓÐÉè±¸
	*/
	HRESULT t_hr=S_OK;
	m_p_device_collection=NULL;
	t_hr=m_p_device_enumrator->EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE,&m_p_device_collection);
	if(FAILED(t_hr)){
		t_ret=MW_AUDIO_MMDEVICE_ENUM_FAILED;
		return t_ret;
	}

	UINT t_ui_count=0;
	t_hr=m_p_device_collection->GetCount(&t_ui_count);
	if(FAILED(t_hr)){
		t_ret=MW_AUDIO_MMDEVICE_ENUM_FAILED;
		return t_ret;
	}

	m_n_device_count=0;
	for(int i=0;i<128;i++){
		memset(m_arr_device[i].m_wcs_id,0,128*sizeof(WCHAR));
		memset(m_arr_device[i].m_wcs_name,0,128*sizeof(WCHAR));
	}

	IMMDevice *t_p_device=NULL;
	LPWSTR		t_p_wstr=NULL;
	IPropertyStore	*t_p_prop=NULL;
	for(UINT i=0;i<t_ui_count;i++){
		t_hr=m_p_device_collection->Item(i,&t_p_device);
		if(FAILED(t_hr)) continue;

		//get endpoint id str
		t_hr=t_p_device->GetId(&t_p_wstr);
		if(FAILED(t_hr)) {
			SafeRelease(&t_p_device);
			continue;
		}

		t_hr=t_p_device->OpenPropertyStore(STGM_READ,&t_p_prop);
		if(FAILED(t_hr)){
			SafeRelease(&t_p_device);
			CoTaskMemFree(t_p_wstr);
			continue;
		}

		PROPVARIANT t_val;
		PropVariantInit(&t_val);
		t_hr=t_p_prop->GetValue(PKEY_Device_FriendlyName,&t_val);		//get friendly device name
		if(FAILED(t_hr)){
			SafeRelease(&t_p_device);
			SafeRelease(&t_p_prop);
			CoTaskMemFree(t_p_wstr);
			continue;
		}

		memcpy(m_arr_device[m_n_device_count].m_wcs_id,t_p_wstr,128);
		int k=0;
		WCHAR t_wc[]=L"}.{";
		for(k=0;k<128;k++){
			if(t_p_wstr[k]==t_wc[0]&&t_p_wstr[k+1]==t_wc[1]&&t_p_wstr[k+2]==t_wc[2]&&k<125)
				break;
		}
		if(k==128){

		}
		else{
			WCHAR t_wcs_guid[128]={0};
			memcpy(t_wcs_guid,t_p_wstr+k+2,126-k);
			GUID t_guid;
			if(SUCCEEDED(IIDFromString(t_wcs_guid,&t_guid))){
				m_arr_device[m_n_device_count].m_guid=t_guid;
			}
		}
		if(t_val.vt==VT_LPWSTR){
			memcpy(m_arr_device[m_n_device_count].m_wcs_name,t_val.pwszVal,128);
		}
		else{
			WCHAR t_arr_dev[]=L"Unknown";
			memcpy(m_arr_device[m_n_device_count].m_wcs_name,t_arr_dev,sizeof(t_arr_dev));
		}
		m_n_device_count++;

		PropVariantClear(&t_val);
		SafeRelease(&t_p_device);
		SafeRelease(&t_p_prop);
		CoTaskMemFree(t_p_wstr);
		
	}

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::get_default_device(mw_mmdevice_t &t_device)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	if(m_p_device_enumrator==NULL){
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		return t_ret;
	}
	IMMDevice *t_p_device=NULL;
	HRESULT t_hr=m_p_device_enumrator->GetDefaultAudioEndpoint(eRender,eMultimedia,&t_p_device);
	if(FAILED(t_hr)){
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		SafeRelease(&t_p_device);
		return t_ret;
	}

	LPWSTR		t_p_wstr=NULL;
	IPropertyStore	*t_p_prop=NULL;
	t_hr=t_p_device->GetId(&t_p_wstr);
	if(FAILED(t_hr)) {
		SafeRelease(&t_p_device);
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		return t_ret;
	}

	t_hr=t_p_device->OpenPropertyStore(STGM_READ,&t_p_prop);
	if(FAILED(t_hr)){
		SafeRelease(&t_p_device);
		CoTaskMemFree(t_p_wstr);
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		return t_ret;
	}

	PROPVARIANT t_val;
	PropVariantInit(&t_val);
	t_hr=t_p_prop->GetValue(PKEY_Device_FriendlyName,&t_val);		//get friendly device name
	if(FAILED(t_hr)){
		SafeRelease(&t_p_device);
		SafeRelease(&t_p_prop);
		CoTaskMemFree(t_p_wstr);
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		return t_ret;
	}

	memcpy(t_device.m_wcs_id,t_p_wstr,128);
	int k=0;
	WCHAR t_wc[]=L"}.{";
	for(k=0;k<128;k++){
		if(t_p_wstr[k]==t_wc[0]&&t_p_wstr[k+1]==t_wc[1]&&t_p_wstr[k+2]==t_wc[2]&&k<125)
			break;
	}
	if(k==128){
		t_ret=MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED;
		return t_ret;
	}
	else{
		WCHAR t_wcs_guid[128]={0};
		memcpy(t_wcs_guid,t_p_wstr+k+2,126-k);
		GUID t_guid;
		if(SUCCEEDED(IIDFromString(t_wcs_guid,&t_guid))){
			t_device.m_guid=t_guid;
		}
	}
	if(t_val.vt==VT_LPWSTR){
		memcpy(t_device.m_wcs_name,t_val.pwszVal,128);
	}
	else{
		WCHAR t_arr_dev[]=L"Unknown";
		memcpy(t_device.m_wcs_name,t_arr_dev,sizeof(t_arr_dev));
	}

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::get_default_device(IMMDevice **t_pp_device)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	HRESULT t_hr=m_p_device_enumrator->GetDefaultAudioEndpoint(eRender,eMultimedia,t_pp_device);
	if(FAILED(t_hr)) t_ret=MW_AUDIO_WASAPI_DEV_ERR;

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::get_select_device(int t_n_select,IMMDevice **t_pp_device)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	if(!(t_n_select>=0&&t_n_select<m_n_device_count)){
		t_ret=MW_AUDIO_WASAPI_DEV_ERR;
		return t_ret;
	}

	HRESULT t_hr=m_p_device_enumrator->GetDevice(m_arr_device[t_n_select].m_wcs_id,t_pp_device);
	if(FAILED(t_hr)) t_ret=MW_AUDIO_WASAPI_DEV_ERR;

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::get_devices(int &t_n_num,mw_mmdevice_t *t_p_device)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	t_n_num=m_n_device_count;
	for(int i=0;i<t_n_num;i++){
		t_p_device[i]=m_arr_device[i];
	}

	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::register_notification()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	if(m_p_device_enumrator==NULL){
		t_ret=MW_AUDIO_REGISTER_CALLBACK_FAILED;
		return t_ret;
	}

	HRESULT t_hr=S_OK;
	t_hr=m_p_device_enumrator->RegisterEndpointNotificationCallback(this);
	if(FAILED(t_hr)){
		t_ret=MW_AUDIO_REGISTER_CALLBACK_FAILED;
		m_b_registernotification=false;
	}
	else{
		m_b_registernotification=true;
	}
	return t_ret;
}

mw_multi_ds_status_e MWMMDevice::set_default_device_changed_callback(LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK t_callback,void *t_p_param)
{
	if(m_b_registernotification){
		m_func_device_changed_callback=t_callback;
		m_p_caller=t_p_param;
		return MW_AUDIO_NO_ERROR;
	}
	else{
		m_func_device_changed_callback=NULL;
		m_p_caller=NULL;
		return MW_AUDIO_WASAPI_DEV_NOTIFY_ERROR;
	}
}

mw_multi_ds_status_e MWMMDevice::unregister_notification()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	m_func_device_changed_callback=NULL;
	m_p_caller=NULL;
	if(m_p_device_enumrator==NULL){
		t_ret=MW_AUDIO_UNREGISTER_CALLBACK_FAILED;
		return t_ret;
	}

	HRESULT t_hr=S_OK;
	t_hr=m_p_device_enumrator->UnregisterEndpointNotificationCallback(this);
	if(FAILED(t_hr)){
		m_b_registernotification=true;
		t_ret=MW_AUDIO_UNREGISTER_CALLBACK_FAILED;
	}
	else{
		m_b_registernotification=false;
	}
	return t_ret;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::OnDeviceStateChanged(/* [annotation][in] */ __in LPCWSTR pwstrDeviceId, /* [annotation][in] */ __in DWORD dwNewState)
{
	HRESULT t_hr=S_OK;

	return t_hr;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::OnDeviceAdded(/* [annotation][in] */ __in LPCWSTR pwstrDeviceId)
{
	HRESULT t_hr=S_OK;

	return t_hr;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::OnDeviceRemoved(/* [annotation][in] */ __in LPCWSTR pwstrDeviceId)
{
	HRESULT t_hr=S_OK;

	return t_hr;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::OnDefaultDeviceChanged(/* [annotation][in] */ __in EDataFlow flow, /* [annotation][in] */ __in ERole role, /* [annotation][in] */ __in LPCWSTR pwstrDefaultDeviceId)
{
	HRESULT t_hr=S_OK;

	mw_device_role t_device_role;
	switch(role){
	case eConsole:
		if(m_func_device_changed_callback!=NULL&&m_p_caller!=NULL){
			m_func_device_changed_callback(MW_MM_CONSOLE,m_p_caller);
		}
		break;
	case eMultimedia:
		if(m_func_device_changed_callback!=NULL&&m_p_caller!=NULL){
			m_func_device_changed_callback(MW_MM_MULTIMEDIA,m_p_caller);
		}
		break;
	case eCommunications:
		if(m_func_device_changed_callback!=NULL&&m_p_caller!=NULL){
			m_func_device_changed_callback(MW_MM_COMMUNITION,m_p_caller);
		}
		break;
	default:
		t_hr=S_FALSE;
		break;
	}

	return t_hr;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::OnPropertyValueChanged(/* [annotation][in] */ __in LPCWSTR pwstrDeviceId, /* [annotation][in] */ __in const PROPERTYKEY key)
{
	HRESULT t_hr=S_OK;
	if(key==PKEY_DeviceInterface_FriendlyName){
		//OutputDebugString(L"OnPropertyValueChanged PKEY_DeviceInterface_FriendlyName\n");
	}
	else if(key==PKEY_Device_DeviceDesc){
		//OutputDebugString(L"OnPropertyValueChanged PKEY_Device_DeviceDesc\n");
	}
	else if(key==PKEY_Device_FriendlyName){
		//OutputDebugString(L"OnPropertyValueChanged PKEY_Device_DeviceDesc\n");
	}
	else{
		//OutputDebugString(L"OnPropertyValueChanged PKEY_Device_DeviceDesc\n");
		//LPOLESTR t_guid;
		//StringFromCLSID(key.fmtid,&t_guid);

	}
	

	return t_hr;
}

HRESULT STDMETHODCALLTYPE MWMMDevice::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (IID_IUnknown == riid)  
	{  
		AddRef();  
		*ppvObject = (IUnknown*)this;  
	}  
	else if (__uuidof(IMMNotificationClient) == riid)  
	{  
		AddRef();  
		*ppvObject = (IMMNotificationClient*)this;  
	}  
	else  
	{  
		*ppvObject = NULL;  
		return E_NOINTERFACE;  
	}  
	return S_OK;
}

ULONG STDMETHODCALLTYPE MWMMDevice::AddRef(void)
{
	return InterlockedIncrement(&m_l_ref); 
}

ULONG STDMETHODCALLTYPE MWMMDevice::Release(void)
{
	ULONG t_ul_ref = InterlockedDecrement(&m_l_ref);  
	if (0 == t_ul_ref)  
	{  
		delete this;  
	}  
	return t_ul_ref;  
}
