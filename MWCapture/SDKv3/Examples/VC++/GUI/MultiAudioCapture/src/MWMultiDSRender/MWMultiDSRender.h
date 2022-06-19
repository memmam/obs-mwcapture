/************************************************************************************************/
// MWMultiDSRender.h : interface of the MWMultiDSRender class

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

#ifndef MWMULRIDSRENDER_H
#define MWMULRIDSRENDER_H

#include "windows.h"

#include <mmsystem.h>
#include "dsound.h"

#include "MWMMDevice.h"
#include "initguid.h"
#include "math.h"

#include "MWMultiDSBuffer.h"

#ifndef WAVE_FORMAT_IEEE_FLOAT
#define WAVE_FORMAT_IEEE_FLOAT 3
#endif

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM 1
#endif

DEFINE_GUID( _KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, WAVE_FORMAT_IEEE_FLOAT, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );
DEFINE_GUID( _KSDATAFORMAT_SUBTYPE_PCM, WAVE_FORMAT_PCM, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 );

typedef struct _ds_list{
	int m_n_count;
	char **m_pp_ids;
	char **m_pp_names;
} mw_ds_list_t;


#ifdef HAVE_ATTRIBUTE_PACKED
#   define ATTR_PACKED __attribute__((__packed__))
#elif defined(__SUNPRO_C) || defined(_MSC_VER)
#   pragma pack(1)
#   define ATTR_PACKED
#elif defined(__APPLE__)
#   pragma pack(push, 1)
#   define ATTR_PACKED
#else
#   error FIXME
#endif
typedef struct
	ATTR_PACKED
	_waveformatextensible{
		WAVEFORMATEX m_format;
		union{
			uint16_t m_w_validbitspersample;		/* bits of precision  */
			uint16_t m_w_samplesperblock;			/* valid if wBitsPerSample==0 */
			uint16_t m_w_reserved;					/* If neither applies, set to zero. */
		}m_samples;

		uint32_t m_dw_channel_mask;					/* Channels present */

		GUID m_subformat;
}mw_waveformatextensible,*pmw_waveformatextensible;

#define MW_MAX_FRAME_BUFFER_COUNT	128
#define MW_MIN_FRAME_BUFFER_COUNT	3

#define MW_MAX_DSOUND_FRAMES		10
#define MW_DEF_DSOUND_FRAMEs		10


typedef HRESULT (WINAPI *MWDirectSoundEnumerate)(LPDSENUMCALLBACKW, LPVOID);
typedef HRESULT (WINAPI *MWDirectSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN);

class MWMultiDSRender{
public:
	MWMultiDSRender();
	~MWMultiDSRender();

public:
	mw_multi_ds_status_e initilize_dsound();

	mw_multi_ds_status_e register_default_device_changed(LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK t_callback,void *t_p_param);

	void deinitilize_dsound();

	mw_multi_ds_status_e create_stream(mw_audio_format_t *t_p_fmt);

	mw_multi_ds_status_e destory_stream();

	mw_multi_ds_status_e run_stream();

	void stop_stream();

	mw_multi_ds_status_e put_frame(uint8_t *t_p_data,uint32_t t_u32_size);

	mw_multi_ds_status_e play_stream(mw_audio_block_t *t_p_block);

	mw_multi_ds_status_e pause_stream(bool t_b_pause);

	mw_multi_ds_status_e flush_stream(bool t_b_drain);

	mw_multi_ds_status_e volume_set(float t_f_volume);

	mw_multi_ds_status_e mute_set(bool t_b_mute);

	mw_multi_ds_status_e device_select(int t_n_i);

	mw_multi_ds_status_e get_current_device_caps(int *t_p_channels);

	mw_multi_ds_status_e get_time(mtime_t *t_time);

protected:

	mw_multi_ds_status_e init_dsound();

	mw_multi_ds_status_e start_dsound(mw_audio_format_t *t_p_fmt);

	mw_multi_ds_status_e create_dsbuffer_pcm(mw_audio_format_t *t_p_fmt,bool b_probe);

	mw_multi_ds_status_e create_dsbuffer(mw_audio_format_t *t_p_fmt,bool b_probe);

	mw_multi_ds_status_e fill_buffer(mw_audio_block_t *t_p_buffer);

	mw_multi_ds_status_e flush(bool t_b_drain);


	static DWORD WINAPI AudioThread(LPVOID lpvParam);
	DWORD audiothread_proc();
	DWORD audiothread_proc_r();

protected:
	HINSTANCE m_h_dsound;

	int m_n_dev_index;

	LPDIRECTSOUND m_dev_obj;
	LPDIRECTSOUNDBUFFER m_p_dsbuffer;
	LPDIRECTSOUNDNOTIFY m_p_notify;

	size_t m_i_write;
	size_t m_i_read;
	size_t m_i_framesize;
	int64_t m_i_data;
	int64_t m_i_last_read;
	int64_t m_i_bytes_per_sample;
	int64_t m_i_samplerate;

	bool m_b_playing;							//play state
	DWORD m_dw_threadid;
	HANDLE m_h_thread;
	HANDLE m_h_event_exit;
	HANDLE m_h_event_ready;

	bool m_b_mute;
	float m_f_vol;
	LONG m_l_mb;

	MWMultiDSBuffer m_ds_buffer;

	MWMMDevice m_mmdevice;						//mmdevice enum and check devices

	int m_n_device_count;						//current device and
	mw_mmdevice_t m_arr_device[128];
	mw_mmdevice_t m_default_device;

	uint32_t								m_u32_buffer_limit;
	uint32_t								m_u32_buffer_size;
	uint32_t								m_u32_dbuffer_size;
};


#endif