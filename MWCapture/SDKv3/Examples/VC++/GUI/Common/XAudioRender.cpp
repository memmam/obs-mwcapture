#include "stdafx.h"
#include "XAudioRender.h"
#include "Unknwn.h"


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

typedef   HRESULT    (_stdcall *Func) (REFCLSID , REFIID , LPVOID*);  
Func   g_DllGetClassObject=NULL; 

XAudioRender::XAudioRender()
{
	m_p_xaudio2=NULL;
	m_ui_flags=0;
	m_arr_f_outputmatrix = NULL;
	m_b_playing = false;
	for (int i = 0;i < MAX_BUFFER_COUNT;i++) {
		m_p_buffer_arr[i] = NULL;
	}
	m_h_lib=NULL;
	m_h_lib=LoadLibrary(L"XAudio2_7.dll");
}

XAudioRender::~XAudioRender()
{
	for (int i = 0;i < MAX_BUFFER_COUNT;i++) {
		if (m_p_buffer_arr[i] != NULL) {
			delete m_p_buffer_arr[i];
			m_p_buffer_arr[i] = NULL;
		}
	}
	if(m_h_lib!=NULL)
		FreeLibrary(m_h_lib);
}
#define _WIN32_WINNT 0x0601 /*_WIN32_WINNT_WIN7*/

BOOL XAudioRender::Initialize()
{
	//initialize com
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
	m_p_xaudio2=NULL;
	m_ui_flags=0;
	HRESULT hr=S_FALSE;
	hr=XAudio2Create(&m_p_xaudio2,m_ui_flags);
	if(FAILED(hr)){
		IXAudio2 *t_p_ixaudio2=NULL;
		
		if(m_h_lib==NULL){
			return FALSE;
		}
		if (g_DllGetClassObject==NULL)   
			g_DllGetClassObject= (Func) GetProcAddress(m_h_lib,"DllGetClassObject"); 
		if(g_DllGetClassObject!=NULL){
			IClassFactory* pClassFactory = NULL;
			HRESULT hr = g_DllGetClassObject(__uuidof(XAudio2), IID_IClassFactory,(LPVOID*)&pClassFactory);
			if (S_OK == hr){
				hr = pClassFactory->CreateInstance(NULL, __uuidof(IXAudio2), (LPVOID*)&t_p_ixaudio2);
				if (S_OK == hr){
					hr=t_p_ixaudio2->Initialize(m_ui_flags);
					m_p_xaudio2=t_p_ixaudio2;
					if(FAILED(hr))
						return FALSE;
				}
			}
			else
				return FALSE;
		} 
	}
	m_p_master_voice=NULL;
	hr=m_p_xaudio2->CreateMasteringVoice(&m_p_master_voice);
	if(FAILED(hr)){
		return FALSE;
	}

	DWORD t_dw_channelmask;
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
	hr=m_p_master_voice->GetChannelMask(&t_dw_channelmask);
#else
	UINT t_ui_count=0;
	m_p_xaudio2->GetDeviceCount(&t_ui_count);
	if(t_ui_count<=0)
		return FALSE;

	for(int i=0;i<t_ui_count;i++){
		XAUDIO2_DEVICE_DETAILS t_device_details;
		m_p_xaudio2->GetDeviceDetails(i,&t_device_details);
		if(t_device_details.Role==GlobalDefaultDevice){
			t_dw_channelmask=t_device_details.OutputFormat.dwChannelMask;
			break;
		}
	}

#endif
	queue<DWORD> t_empty;
	swap(m_queue_destion_channels, t_empty);
	switch (t_dw_channelmask)
	{
	case SPEAKER_MONO://SPEAKER_FRONT_CENTER
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		break;
	case SPEAKER_STEREO:       //   (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		break;
	case SPEAKER_2POINT1:      //   (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		break;
	case SPEAKER_SURROUND:     //   (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		m_queue_destion_channels.push(SPEAKER_BACK_CENTER);
		break;
	case SPEAKER_QUAD:       //(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_BACK_LEFT);
		m_queue_destion_channels.push(SPEAKER_BACK_RIGHT);
		break;
	case SPEAKER_4POINT1:    //      (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		m_queue_destion_channels.push(SPEAKER_BACK_LEFT);
		m_queue_destion_channels.push(SPEAKER_BACK_RIGHT);
		break;
	case SPEAKER_5POINT1:    //     (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		m_queue_destion_channels.push(SPEAKER_BACK_LEFT);
		m_queue_destion_channels.push(SPEAKER_BACK_RIGHT);
		break;
	case SPEAKER_7POINT1:    //      (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		m_queue_destion_channels.push(SPEAKER_BACK_LEFT);
		m_queue_destion_channels.push(SPEAKER_BACK_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT_OF_CENTER);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT_OF_CENTER);
		break;
	case SPEAKER_5POINT1_SURROUND:// (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		m_queue_destion_channels.push(SPEAKER_SIDE_LEFT);
		m_queue_destion_channels.push(SPEAKER_SIDE_RIGHT);
		break;
	case SPEAKER_7POINT1_SURROUND:// (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT)
		m_queue_destion_channels.push(SPEAKER_FRONT_LEFT);
		m_queue_destion_channels.push(SPEAKER_FRONT_RIGHT);
		m_queue_destion_channels.push(SPEAKER_FRONT_CENTER);
		m_queue_destion_channels.push(SPEAKER_LOW_FREQUENCY);
		m_queue_destion_channels.push(SPEAKER_BACK_LEFT);
		m_queue_destion_channels.push(SPEAKER_BACK_RIGHT);
		m_queue_destion_channels.push(SPEAKER_SIDE_LEFT);
		m_queue_destion_channels.push(SPEAKER_SIDE_RIGHT);
		break;
	default:
		break;
	}
	return TRUE;
}

void XAudioRender::Deinitialize()
{
	if(m_p_master_voice){
	m_p_master_voice->DestroyVoice();
	}
	SAFE_RELEASE(m_p_xaudio2);

	CoUninitialize();
}

BOOL XAudioRender::CreateAudio(int t_n_samplerate, int t_n_channels, int t_n_bitdepth) {
	HRESULT hr = S_FALSE;
	WAVEFORMATEX t_fmt;
	t_fmt.wFormatTag = WAVE_FORMAT_PCM;
	t_fmt.nChannels = t_n_channels;
	t_fmt.nSamplesPerSec = t_n_samplerate;
	t_fmt.wBitsPerSample = t_n_bitdepth;
	t_fmt.nBlockAlign = t_fmt.wBitsPerSample*t_fmt.nChannels / 8;
	t_fmt.nAvgBytesPerSec = t_fmt.nBlockAlign*t_fmt.nSamplesPerSec;
	t_fmt.cbSize = 0;
	hr = m_p_xaudio2->CreateSourceVoice(&m_p_source_voice, &t_fmt, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voice_context, NULL, NULL);
	if (FAILED(hr)) {
		return FALSE;
	}

	for (int i = 0; i < MAX_BUFFER_COUNT; i++) {
		m_p_buffer_arr[i] = new unsigned char[SAMPLES_PER_FRAME * 8 * 4];
		m_queue_buffer.push(m_p_buffer_arr[i]);
	}

	if (m_arr_f_outputmatrix != NULL) {
		delete []m_arr_f_outputmatrix;
		m_arr_f_outputmatrix = NULL;
	}
	m_n_source_channels = t_n_channels;
	m_arr_f_outputmatrix = new float[m_queue_destion_channels.size()*m_n_source_channels];
	for (int i = 0; i < m_queue_destion_channels.size()*m_n_source_channels; i++) {
		m_arr_f_outputmatrix[i] = 0.0f;
	}
	for (int i = 0; i < m_n_source_channels; i++) {
		for (int j = 0; j < m_queue_destion_channels.size(); j++) {
			if (i == j) {
				m_arr_f_outputmatrix[j*m_n_source_channels + i] = 1.0f;
			}
		}
	}
	m_p_source_voice->SetOutputMatrix(m_p_master_voice, m_n_source_channels, m_queue_destion_channels.size(), m_arr_f_outputmatrix, XAUDIO2_COMMIT_NOW);
	m_p_source_voice->Start(0, XAUDIO2_COMMIT_NOW);
	m_b_playing = true;
	return TRUE;
}

void XAudioRender::DestroyAudio()
{
	m_b_playing = false;
	XAUDIO2_VOICE_STATE state;
	while(true)
	{
		m_p_source_voice->GetState( &state );
		if( !state.BuffersQueued )
			break;

		WaitForSingleObject( m_voice_context.hBufferEndEvent, INFINITE );
	}

	//
	// Clean up
	//
	m_p_source_voice->Stop( 0 );
	m_p_source_voice->DestroyVoice();
	
	if (m_arr_f_outputmatrix != NULL) {
		delete[]m_arr_f_outputmatrix;
		m_arr_f_outputmatrix = NULL;
	}
}

BOOL XAudioRender::PutFrame(const BYTE * pbFrame, int cbFrame)
{
	XAUDIO2_VOICE_STATE state;

	while(true){
		m_p_source_voice->GetState( &state );
		if( state.BuffersQueued < MAX_BUFFER_COUNT - 1 )
			break;
		WaitForSingleObject( m_voice_context.hBufferEndEvent, INFINITE );
	}


	unsigned char* t_temp=m_queue_buffer.front();
	m_queue_buffer.pop();
	memcpy(t_temp, pbFrame, cbFrame);
	m_queue_buffer.push(t_temp);
	//
	// At this point we have a buffer full of audio and enough room to submit it, so
	// let's submit it and get another read request going.
	//
	//memcpy(m_p_b_audio_frame, pbFrame, cbFrame);
	XAUDIO2_BUFFER buf = {0};
	buf.AudioBytes = cbFrame;
	buf.pAudioData = t_temp;
	m_p_source_voice->SubmitSourceBuffer( &buf );

	return TRUE;
}

BOOL XAudioRender::SetOutPutMatrix(soutputcell *t_arr_output, int t_n_num) {
	if (m_b_playing == false)
		return FALSE;

	if (t_n_num > (m_n_source_channels*m_queue_destion_channels.size()))
		return FALSE;

	BOOL t_re = TRUE;
	for (int i = 0; i < t_n_num; i++) {
		int t_n_des=t_arr_output[i].m_n_des;
		int t_n_sour=t_arr_output[i].m_n_sour;
		float t_f_vol = t_arr_output[i].m_f_vol;
		if (!(t_n_des >= 0 && t_n_des < m_queue_destion_channels.size())) {
			t_re = FALSE;
			continue;
		}
		if (!(t_n_sour >= 0 && t_n_sour < m_n_source_channels)) {
			t_re = FALSE;
			continue;
		}
		if (!(t_f_vol >= 0.0f&&t_f_vol <= 1.0f)) {
			t_re = FALSE;
			t_f_vol = 0.0f;
		}
		m_arr_f_outputmatrix[t_n_des*m_n_source_channels + t_n_sour] = t_f_vol;
		m_p_source_voice->SetOutputMatrix(m_p_master_voice, m_n_source_channels, m_queue_destion_channels.size(), m_arr_f_outputmatrix, XAUDIO2_COMMIT_NOW);
	}

	return t_re;
}