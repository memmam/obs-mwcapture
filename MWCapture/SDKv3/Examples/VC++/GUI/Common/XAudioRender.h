#ifndef XAUDIORENDER_H
#define XAUDIORENDER_H

#include <windows.h>
#include <xaudio2.h>
#include <x3daudio.h>
#include <mmsystem.h>
#include <queue>

using namespace std;
//--------------------------------------------------------------------------------------
// Callback structure
//--------------------------------------------------------------------------------------
struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
	STDMETHOD_( void, OnVoiceProcessingPassStart )( UINT32 )
	{
	}
	STDMETHOD_( void, OnVoiceProcessingPassEnd )()
	{
	}
	STDMETHOD_( void, OnStreamEnd )()
	{
	}
	STDMETHOD_( void, OnBufferStart )( void* )
	{
	}
	STDMETHOD_( void, OnBufferEnd )( void* )
	{
		SetEvent( hBufferEndEvent );
	}
	STDMETHOD_( void, OnLoopEnd )( void* )
	{
	}
	STDMETHOD_( void, OnVoiceError )( void*, HRESULT )
	{
	}

	HANDLE hBufferEndEvent;

	StreamingVoiceContext() : hBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )
	{
	}
	virtual ~StreamingVoiceContext()
	{
		CloseHandle( hBufferEndEvent );
	}
};

typedef struct _ouptcell {
	int m_n_des;
	int m_n_sour;
	float m_f_vol;
}soutputcell;

#define MAX_BUFFER_COUNT 10
#define SAMPLES_PER_FRAME 192

class XAudioRender{
public:
	XAudioRender();
	~XAudioRender();

public:
	BOOL Initialize();
	void Deinitialize();

	BOOL CreateAudio(int t_n_samplerate,int t_n_channels,int t_n_bitdepth);
	void DestroyAudio();

	BOOL PutFrame(const BYTE * pbFrame, int cbFrame);

	BOOL SetOutPutMatrix(soutputcell *t_arr_output, int t_n_num);
protected:
	IXAudio2* m_p_xaudio2;
	UINT32 m_ui_flags;

	IXAudio2MasteringVoice* m_p_master_voice;
	IXAudio2SourceVoice* m_p_source_voice;
	XAUDIO2_BUFFER m_x2_buffer;
	StreamingVoiceContext m_voice_context;

	HINSTANCE m_h_lib;

	queue<unsigned char*> m_queue_buffer;
	unsigned char* m_p_buffer_arr[MAX_BUFFER_COUNT];

	queue<DWORD> m_queue_destion_channels;
	int m_n_source_channels;
	float *m_arr_f_outputmatrix;

	bool m_b_playing;

};

#endif