/************************************************************************************************/
// MWMultiDSRender.cpp : implementation of the MWMultiDSRender class

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
#include "MWMultiDSRender.h"

#include "string.h"

#pragma comment(lib, "Dxguid.lib")

static int CALLBACK DeviceEnumCallback(LPGUID guid, LPCWSTR desc, LPCWSTR mod,
				       LPVOID data);

/* Speaker setup override options list */
static const char *const g_speaker_list[] = {
	"Windows default", "Mono", "Stereo", "Quad", "5.1", "7.1"};

#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE /* Microsoft */
#endif

#define DS_BUF_SIZE (3840)

#define CLOCK_FREQ INT64_C(1000000)

static const mw_audio_channel_type_t g_channel_order[] = {
	0x01, 0x02, 0x04, 0x08, 0x100, 0x200, 0x400, 0x10, 0x20, 0x40, 0x80, 0};

MWMultiDSRender::MWMultiDSRender()
{
	m_h_dsound = NULL;
	m_n_dev_index = -1;
	m_dev_obj = NULL;
	m_i_write = 0;
	m_i_data = 0;

	m_b_mute = false;
	m_f_vol = 1.0;

	m_b_playing = false;
}

MWMultiDSRender::~MWMultiDSRender() {}

mw_multi_ds_status_e MWMultiDSRender::initilize_dsound()
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	m_h_dsound = LoadLibrary(L"DSOUND.DLL");
	if (m_h_dsound == NULL) {
		return MW_AUDIO_DSOUND_LOAD_FAILED;
	}

	t_ret = m_mmdevice.initilize();
	if (t_ret != MW_AUDIO_NO_ERROR)
		return t_ret;

	t_ret = m_mmdevice.enum_device();
	if (t_ret != MW_AUDIO_NO_ERROR)
		return t_ret;

	t_ret = m_mmdevice.register_notification();
	if (t_ret != MW_AUDIO_NO_ERROR)
		return t_ret;

	t_ret = m_mmdevice.get_devices(m_n_device_count, m_arr_device);
	if (t_ret != MW_AUDIO_NO_ERROR)
		return t_ret;

	if (t_ret != MW_AUDIO_NO_ERROR)
		return t_ret;

	m_b_playing = false;

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e MWMultiDSRender::register_default_device_changed(
	LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK t_callback, void *t_p_param)
{
	return m_mmdevice.set_default_device_changed_callback(t_callback,
							      t_p_param);
}

void MWMultiDSRender::deinitilize_dsound()
{
	if (m_h_dsound != NULL) {
		FreeLibrary(m_h_dsound);
		m_h_dsound = NULL;
	}
	m_b_playing = false;
}

mw_multi_ds_status_e MWMultiDSRender::create_stream(mw_audio_format_t *t_p_fmt)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	/*Init DSound*/
	t_ret = init_dsound();
	if (t_ret != MW_AUDIO_NO_ERROR) {
		return MW_AUDIO_DSOUND_INIT_FAILED;
	}

	t_ret = start_dsound(t_p_fmt);

	m_b_playing = false;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::destory_stream()
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	m_b_playing = false;

	if (m_p_notify != NULL) {
		IDirectSoundNotify_Release(m_p_notify);
		m_p_notify = NULL;
	}
	if (m_p_dsbuffer != NULL) {
		IDirectSoundBuffer_Stop(m_p_dsbuffer);
		IDirectSoundBuffer_Release(m_p_dsbuffer);
		m_p_dsbuffer = NULL;
	}
	if (m_dev_obj != NULL) {
		IDirectSound_Release(m_dev_obj);
		m_dev_obj = NULL;
	}

	m_ds_buffer.destroy_audio_buffers();

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::run_stream()
{
	m_h_event_exit = CreateEvent(NULL, FALSE, FALSE, NULL);
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	if (m_b_playing) {
		t_ret = MW_AUDIO_ALREADY_RUN;
		return t_ret;
	}

	m_p_dsbuffer->SetCurrentPosition(0);
	m_i_write = 0;
	//fill_buffer(NULL);

	m_dw_threadid = 0;
	m_b_playing = true;
	m_h_thread =
		CreateThread(NULL, 0, AudioThread, this, 0, &m_dw_threadid);
	if (NULL == m_h_thread ||
	    (FAILED(IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0,
					    DSBPLAY_LOOPING)))) {
		stop_stream();
		return MW_AUDIO_PLAY_FAILED;
	}
	BOOL t_b_re =
		SetThreadPriority(m_h_thread, THREAD_PRIORITY_TIME_CRITICAL);

	return t_ret;
}

void MWMultiDSRender::stop_stream()
{
	if (!m_b_playing || m_p_dsbuffer == NULL)
		return;

	IDirectSoundBuffer_Stop(m_p_dsbuffer);

	m_b_playing = false;
	if (m_h_thread) {
		SetEvent(m_h_event_exit);
		WaitForSingleObject(m_h_thread, INFINITE);
		CloseHandle(m_h_thread);
		m_h_thread = NULL;
	}
}

mw_multi_ds_status_e MWMultiDSRender::put_frame(uint8_t *t_p_data,
						uint32_t t_u32_size)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	t_ret = m_ds_buffer.put_audio_frames(t_p_data, t_u32_size);

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::volume_set(float t_f_volume)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	/* Directsound doesn't support amplification, so we use software
       gain if we need it and only for this */
	float gain = t_f_volume > 1.f ? t_f_volume * t_f_volume * t_f_volume
				      : 1.f;

	/* millibels from linear amplification */
	float t_min = t_f_volume < 1.0f ? t_f_volume : 1.0f;
	float t_f_temp = 6000.f * log10f(t_min);
	LONG mb = floor(t_f_temp);

	/* Clamp to allowed DirectSound range */
	static_assert(DSBVOLUME_MIN < DSBVOLUME_MAX, "DSBVOLUME_* confused");
	if (mb > DSBVOLUME_MAX) {
		mb = DSBVOLUME_MAX;
		t_ret = MW_AUDIO_DSOUND_VOL_OVERFLOW;
	}
	if (mb <= DSBVOLUME_MIN)
		mb = DSBVOLUME_MIN;

	if (!m_b_mute && m_p_dsbuffer != NULL &&
	    IDirectSoundBuffer_SetVolume(m_p_dsbuffer, mb) != DS_OK)
		t_ret = MW_AUDIO_DSOUND_VOL_SETFAILED;

	m_l_mb = mb;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::mute_set(bool t_b_mute)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	HRESULT t_hr = DS_OK;
	m_b_mute = t_b_mute;

	if (m_p_dsbuffer != NULL)
		t_hr = IDirectSoundBuffer_SetVolume(
			m_p_dsbuffer, m_b_mute ? DSBVOLUME_MIN : m_l_mb);

	if (t_hr != DS_OK)
		t_ret = MW_AUDIO_DSOUND_VOL_MUTEFAILED;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::device_select(int t_n_i)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	//@FIXME:
	m_n_dev_index = t_n_i;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::get_current_device_caps(int *t_p_channels)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	GUID guid, *p_guid = NULL;
	DWORD t_dw_speaker_config;

	if (m_dev_obj == NULL) {
		MWDirectSoundCreate t_func_create = NULL;
		t_func_create = (MWDirectSoundCreate)GetProcAddress(
			m_h_dsound, "DirectSoundCreate");
		if (t_func_create == NULL) {
			return MW_AUDIO_DSOUND_CREATE_FAILED;
		}

		if (m_n_device_count < 0) {
			return MW_AUDIO_DSOUND_NO_DEVICE;
		}

		if (m_n_dev_index < 0) {
			t_ret = m_mmdevice.get_default_device(m_default_device);
			if (t_ret != MW_AUDIO_NO_ERROR)
				return t_ret;
			p_guid = &m_default_device.m_guid;
		} else {
			p_guid = &m_arr_device[m_n_dev_index].m_guid;
		}

		/* Create the direct sound object */
		HRESULT t_hr = S_OK;
		t_hr = t_func_create(p_guid, &m_dev_obj, NULL);
		if FAILED (t_hr) {
			return MW_AUDIO_DSOUND_CREATE_DEV_FAILED;
		}

		HRESULT hr = IDirectSound_GetSpeakerConfig(
			m_dev_obj, &t_dw_speaker_config);
		m_dev_obj->Release();
		m_dev_obj = NULL;
		if (FAILED(hr)) {
			return MW_AUDIO_DSOUND_GETSPEAKER_FAILED;
		}

	} else {
		HRESULT hr = IDirectSound_GetSpeakerConfig(
			m_dev_obj, &t_dw_speaker_config);
		if (FAILED(hr)) {
			return MW_AUDIO_DSOUND_GETSPEAKER_FAILED;
		}
	}

	switch (DSSPEAKER_CONFIG(t_dw_speaker_config)) {
	case DSSPEAKER_7POINT1:
	case DSSPEAKER_7POINT1_SURROUND:
		*t_p_channels = 8;
		break;
	case DSSPEAKER_5POINT1:
	case DSSPEAKER_5POINT1_SURROUND:
		*t_p_channels = 6;
		break;
	case DSSPEAKER_QUAD:
		*t_p_channels = 4;
		break;
	case DSSPEAKER_SURROUND:
		*t_p_channels = 4;
		break;
	case DSSPEAKER_STEREO:
		*t_p_channels = 2;
		break;
	}

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e MWMultiDSRender::get_time(mtime_t *t_time)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	DWORD read, status;
	HRESULT hr;
	mtime_t size;

	hr = IDirectSoundBuffer_GetStatus(m_p_dsbuffer, &status);
	if (hr != DS_OK)
		return MW_AUDIO_DSOUND_STATUS_FAILED;
	if (!(status & DSBSTATUS_PLAYING))
		return MW_AUDIO_DSOUND_STATUS_FAILED;

	hr = IDirectSoundBuffer_GetCurrentPosition(m_p_dsbuffer, &read, NULL);
	if (hr != DS_OK)
		return MW_AUDIO_DSOUND_POSITION_FAILED;

	size = (mtime_t)read - m_i_last_read;

	/* GetCurrentPosition cannot be trusted if the return doesn't change
     * Just return an error */
	if (size == 0)
		return MW_AUDIO_DSOUND_POSITION_ERR;
	else if (size < 0)
		size += DS_BUF_SIZE;

	m_i_data -= size;
	m_i_last_read = read;

	if (m_i_data < 0)
		/* underrun */
		flush_stream(false);

	*t_time =
		(m_i_data / m_i_bytes_per_sample) * CLOCK_FREQ / m_i_samplerate;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::play_stream(mw_audio_block_t *t_p_block)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	t_ret = fill_buffer(t_p_block);
	if (t_ret != DS_OK) {
		return t_ret;
	}

	HRESULT t_hr = DS_OK;
	/* start playing the buffer */
	t_hr = IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0, DSBPLAY_LOOPING);
	if (t_hr == DSERR_BUFFERLOST) {
		IDirectSoundBuffer_Restore(m_p_dsbuffer);
		t_hr = IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0,
					       DSBPLAY_LOOPING);
	}
	if (t_hr != DS_OK) {

	} else {
		m_b_playing = true;
	}
	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::pause_stream(bool t_b_pause)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	HRESULT t_hr = DS_OK;

	if (t_b_pause)
		t_hr = IDirectSoundBuffer_Stop(m_p_dsbuffer);
	else
		t_hr = IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0,
					       DSBPLAY_LOOPING);
	if (t_hr == DS_OK) {
		m_b_playing = !t_b_pause;
	}
	if (t_hr != DS_OK)
		t_ret = MW_AUDIO_DSOUND_PAUSE_FAILED;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSRender::flush_stream(bool t_b_drain)
{
	return flush(t_b_drain);
}

mw_multi_ds_status_e MWMultiDSRender::init_dsound()
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	GUID guid, *p_guid = NULL;

	MWDirectSoundCreate t_func_create = NULL;
	t_func_create = (MWDirectSoundCreate)GetProcAddress(
		m_h_dsound, "DirectSoundCreate");
	if (t_func_create == NULL) {
		return MW_AUDIO_DSOUND_CREATE_FAILED;
	}

	if (m_n_device_count < 0) {
		return MW_AUDIO_DSOUND_NO_DEVICE;
	}

	if (m_n_dev_index < 0) {
		t_ret = m_mmdevice.get_default_device(m_default_device);
		if (t_ret != MW_AUDIO_NO_ERROR)
			return t_ret;
		p_guid = &m_default_device.m_guid;
	} else {
		p_guid = &m_arr_device[m_n_dev_index].m_guid;
	}

	/* Create the direct sound object */
	HRESULT t_hr = S_OK;
	t_hr = t_func_create(p_guid, &m_dev_obj, NULL);
	if FAILED (t_hr) {
		return MW_AUDIO_DSOUND_CREATE_DEV_FAILED;
	}

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e MWMultiDSRender::start_dsound(mw_audio_format_t *t_p_fmt)
{
	/* Set DirectSound Cooperative level, ie what control we want over Windows
     * sound device. In our case, DSSCL_EXCLUSIVE means that we can modify the
     * settings of the primary buffer, but also that only the sound of our
     * application will be hearable when it will have the focus.
     * !!! (this is not really working as intended yet because to set the
     * cooperative level you need the window handle of your application, and
     * I don't know of any easy way to get it. Especially since we might play
     * sound without any video, and so what window handle should we use ???
     * The hack for now is to use the Desktop window handle - it seems to be
     * working */
	if (IDirectSound_SetCooperativeLevel(m_dev_obj, GetDesktopWindow(),
					     DSSCL_EXCLUSIVE)) {
	}

	const char *const *ppsz_compare = g_speaker_list;
	char *psz_speaker;
	HRESULT hr;

	DWORD t_dw_speaker_config;
	int i_channels = 2; /* Default to stereo */
	int i_orig_channels = t_p_fmt->m_n_channels;

	/* Check the speaker configuration to determine which channel
     * config should be the default */
	hr = IDirectSound_GetSpeakerConfig(m_dev_obj, &t_dw_speaker_config);
	if (FAILED(hr)) {
		t_dw_speaker_config = DSSPEAKER_STEREO;
	}
	const char *name = "Unknown";
	switch (DSSPEAKER_CONFIG(t_dw_speaker_config)) {
	case DSSPEAKER_7POINT1:
	case DSSPEAKER_7POINT1_SURROUND:
		name = "7.1";
		i_channels = 8;
		break;
	case DSSPEAKER_5POINT1:
	case DSSPEAKER_5POINT1_SURROUND:
		name = "5.1";
		i_channels = 6;
		break;
	case DSSPEAKER_QUAD:
		name = "Quad";
		i_channels = 4;
		break;
#if 0 /* Lots of people just get their settings wrong and complain that
       * this is a problem with VLC so just don't ever set mono by default. */
    case DSSPEAKER_MONO:
         name = "Mono";
         i_channels = 1;
         break;
#endif
	case DSSPEAKER_SURROUND:
		name = "Surround";
		i_channels = 4;
		break;
	case DSSPEAKER_STEREO:
		name = "Stereo";
		i_channels = 2;
		break;
	}

	if (i_channels >= i_orig_channels)
		i_channels = i_orig_channels;
	else {
		//channels overflow
	}

	switch (i_channels) {
	case 8:
		t_p_fmt->m_channel_type = MW_CHANNELS_7_1;
		break;
	case 7:
	case 6:
		t_p_fmt->m_channel_type = MW_CHANNELS_5_1;
		break;
	case 5:
	case 4:
		t_p_fmt->m_channel_type = MW_CHANNELS_2_0;
		break;
	default:
		t_p_fmt->m_channel_type = MW_CHANNELS_2_0;
		break;
	}

	hr = create_dsbuffer_pcm(t_p_fmt, false);
	if (hr != DS_OK) {
		return MW_AUDIO_DSOUND_DXDEVICE_FAILED;
	}

	m_b_playing = false;
	m_i_write = 0;
	m_i_last_read = 0;
	m_i_data = 0;

	m_u32_buffer_limit = t_p_fmt->m_n_sample_rate * t_p_fmt->m_n_channels *
			     t_p_fmt->m_n_bitspersample / 8 *
			     t_p_fmt->m_n_timeuint * 10 / 1000;
	m_u32_buffer_size = t_p_fmt->m_n_sample_rate * t_p_fmt->m_n_channels *
			    t_p_fmt->m_n_bitspersample / 8;
	m_ds_buffer.create_audio_buffers(m_u32_buffer_size, 20,
					 m_u32_buffer_limit);

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e
MWMultiDSRender::create_dsbuffer_pcm(mw_audio_format_t *t_p_fmt, bool b_probe)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	t_ret = create_dsbuffer(t_p_fmt, b_probe);

	return t_ret;
}

mw_multi_ds_status_e
MWMultiDSRender::create_dsbuffer(mw_audio_format_t *t_p_fmt, bool b_probe)
{
	mw_waveformatextensible t_waveformat;
	DSBUFFERDESC dsbdesc;
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;

	/* First set the sound buffer format */
	t_waveformat.m_dw_channel_mask = 0;

	for (unsigned i = 0; g_channel_order[i]; i++)
		if (t_p_fmt->m_channel_type & g_channel_order[i])
			t_waveformat.m_dw_channel_mask |= g_channel_order[i];

	switch (t_p_fmt->m_audio_type) {
	case MW_AUDIO_FORMAT_PCM_08:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_16;
	case MW_AUDIO_FORMAT_PCM_16:
		t_waveformat.m_format.wBitsPerSample = 16;
		t_waveformat.m_format.wFormatTag = WAVE_FORMAT_PCM;
		t_waveformat.m_subformat = _KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case MW_AUDIO_FORMAT_PCM_20:
	case MW_AUDIO_FORMAT_PCM_24:
		t_waveformat.m_format.wBitsPerSample = 24;
		t_waveformat.m_format.wFormatTag = WAVE_FORMAT_PCM;
		t_waveformat.m_subformat = _KSDATAFORMAT_SUBTYPE_PCM;
		break;
	default:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_32;
		t_waveformat.m_format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		t_waveformat.m_subformat = _KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		break;
	}

	// @FIXME channels may be dev channel num
	t_waveformat.m_format.nChannels = t_p_fmt->m_n_channels;
	t_waveformat.m_format.nSamplesPerSec = t_p_fmt->m_n_sample_rate;
	t_waveformat.m_format.nBlockAlign =
		t_waveformat.m_format.wBitsPerSample / 8 *
		t_waveformat.m_format.nChannels;
	t_waveformat.m_format.nAvgBytesPerSec =
		t_waveformat.m_format.nSamplesPerSec *
		t_waveformat.m_format.nBlockAlign;

	t_waveformat.m_samples.m_w_validbitspersample =
		t_p_fmt->m_n_bitspersample;

	/* Then fill in the direct sound descriptor */
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags =
		DSBCAPS_GETCURRENTPOSITION2   /* Better position accuracy */
		| DSBCAPS_GLOBALFOCUS         /* Allows background playing */
		| DSBCAPS_CTRLVOLUME          /* Allows volume control */
		| DSBCAPS_CTRLPOSITIONNOTIFY; /* Allow position notifications */

	/* Only use the new WAVE_FORMAT_EXTENSIBLE format for multichannel audio */
	if (t_waveformat.m_format.nChannels <= 2) {
		t_waveformat.m_format.cbSize = 0;
	} else {
		t_waveformat.m_format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		// @FIXME reset cbSize
		t_waveformat.m_format.cbSize =
			sizeof(mw_waveformatextensible) - sizeof(WAVEFORMATEX);

		/* Needed for 5.1 on emu101k */
		dsbdesc.dwFlags |= DSBCAPS_LOCHARDWARE;
	}
	m_u32_dbuffer_size = t_p_fmt->m_n_sample_rate * t_p_fmt->m_n_channels *
			     t_p_fmt->m_n_bitspersample / 8 *
			     t_p_fmt->m_n_timeuint * 10 /
			     1000; /* buffer size */
	dsbdesc.dwBufferBytes = m_u32_dbuffer_size * 10;
	dsbdesc.lpwfxFormat = (WAVEFORMATEX *)&t_waveformat;

	HRESULT t_hr = IDirectSound_CreateSoundBuffer(m_dev_obj, &dsbdesc,
						      &m_p_dsbuffer, NULL);
	if (FAILED(t_hr)) {
		if (!(dsbdesc.dwFlags & DSBCAPS_LOCHARDWARE)) {
			t_ret = MW_AUDIO_DSOUND_CREATE_DSBUFFER_FAILED;
			return t_ret;
		}

		/* Try without DSBCAPS_LOCHARDWARE */
		dsbdesc.dwFlags &= ~DSBCAPS_LOCHARDWARE;
		t_hr = IDirectSound_CreateSoundBuffer(m_dev_obj, &dsbdesc,
						      &m_p_dsbuffer, NULL);
		if (FAILED(t_hr)) {
			t_ret = MW_AUDIO_DSOUND_CREATE_DSBUFFER_FAILED;
			return t_ret;
		}
		if (!b_probe) {
		}
	}

	/* Stop here if we were just probing */
	if (b_probe) {
		IDirectSoundBuffer_Release(m_p_dsbuffer);
		m_p_dsbuffer = NULL;
		return MW_AUDIO_NO_ERROR;
	}

	t_hr = IDirectSoundBuffer_QueryInterface(
		m_p_dsbuffer, IID_IDirectSoundNotify, (void **)&m_p_notify);
	if (t_hr != DS_OK) {
		m_p_notify = NULL;
	}

	m_h_event_ready = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_h_event_ready == NULL) {
		return MW_AUDIO_DSOUND_CREATE_DSBUFFER_FAILED;
	}

	DSBPOSITIONNOTIFY t_arr_dsposnotify[128];
	for (int i = 0; i < 10; i++) {
		t_arr_dsposnotify[i].dwOffset =
			i * m_u32_dbuffer_size + (m_u32_dbuffer_size - 1);
		t_arr_dsposnotify[i].hEventNotify = m_h_event_ready;
	}

	t_hr = IDirectSoundNotify_SetNotificationPositions(m_p_notify, 10,
							   t_arr_dsposnotify);
	if (FAILED(t_hr)) {
	}

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e MWMultiDSRender::fill_buffer(mw_audio_block_t *t_p_buffer)
{
	size_t towrite = (t_p_buffer != NULL) ? t_p_buffer->m_u32_size
					      : m_u32_dbuffer_size;
	void *p_write_position, *p_wrap_around;
	unsigned long l_bytes1, l_bytes2;
	HRESULT dsresult;

	/* Before copying anything, we have to lock the buffer */
	dsresult = IDirectSoundBuffer_Lock(
		m_p_dsbuffer,      /* DS buffer */
		m_i_write,         /* Start offset */
		towrite,           /* Number of bytes */
		&p_write_position, /* Address of lock start */
		&l_bytes1,      /* Count of bytes locked before wrap around */
		&p_wrap_around, /* Buffer address (if wrap around) */
		&l_bytes2,      /* Count of bytes after wrap around */
		0);             /* Flags: DSBLOCK_FROMWRITECURSOR is buggy */
	if (dsresult == DSERR_BUFFERLOST) {
		IDirectSoundBuffer_Restore(m_p_dsbuffer);
		dsresult = IDirectSoundBuffer_Lock(m_p_dsbuffer, m_i_write,
						   towrite, &p_write_position,
						   &l_bytes1, &p_wrap_around,
						   &l_bytes2, 0);
	}
	if (dsresult != DS_OK) {
		if (t_p_buffer != NULL) {
			//block_Release( p_buffer );
			//release buffer
		}
		return MW_AUDIO_DSOUND_DSBUFFER_LOCK_FAILED;
	}

	if (t_p_buffer == NULL) {
		memset(p_write_position, 0, l_bytes1);
		memset(p_wrap_around, 0, l_bytes2);
	} else {

		memcpy(p_write_position, t_p_buffer->m_p_data, l_bytes1);
		if (p_wrap_around && l_bytes2)
			memcpy(p_wrap_around, t_p_buffer->m_p_data + l_bytes1,
			       l_bytes2);

		if ((l_bytes1 + l_bytes2) < t_p_buffer->m_u32_size) {
		}
	}

	/* Now the data has been copied, unlock the buffer */
	IDirectSoundBuffer_Unlock(m_p_dsbuffer, p_write_position, l_bytes1,
				  p_wrap_around, l_bytes2);

	m_i_write += towrite;
	m_i_write %= (m_u32_dbuffer_size * 10);
	m_i_data += towrite;

	return MW_AUDIO_NO_ERROR;
}

mw_multi_ds_status_e MWMultiDSRender::flush(bool t_b_drain)
{
	mw_multi_ds_status_e t_ret = MW_AUDIO_NO_ERROR;
	HRESULT ret = IDirectSoundBuffer_Stop(m_p_dsbuffer);
	if (ret == DS_OK && !t_b_drain) {
		m_i_data = 0;
		m_i_last_read = m_i_write;
		IDirectSoundBuffer_SetCurrentPosition(m_p_dsbuffer, m_i_write);
		m_b_playing = false;
	}
	if (ret != DS_OK)
		t_ret = MW_AUDIO_DSOUND_FLUSH_FAILED;

	return t_ret;
}

DWORD WINAPI MWMultiDSRender::AudioThread(LPVOID lpvParam)
{
	if (lpvParam == NULL)
		return 0;

	MWMultiDSRender *t_p_this = (MWMultiDSRender *)lpvParam;
	return t_p_this->audiothread_proc_r();
}

DWORD MWMultiDSRender::audiothread_proc()
{
	int t_n_frame = 0;

	DSBPOSITIONNOTIFY t_arr_dsposnotify[10];
	for (int i = 0; i < 10; i++) {
		t_arr_dsposnotify[i].dwOffset =
			i * m_u32_buffer_limit + (m_u32_buffer_limit - 1);
		t_arr_dsposnotify[i].hEventNotify = m_h_event_ready;
	}
	HRESULT t_hr =
		m_p_notify->SetNotificationPositions(10, t_arr_dsposnotify);
	if (FAILED(t_hr)) {
	}
	while (m_b_playing) {
		mw_audio_block_t *t_p_block = NULL;
		mw_multi_ds_status_e t_ret =
			m_ds_buffer.get_audio_frames(t_p_block);
		if (t_ret != MW_AUDIO_NO_ERROR) {
			continue;
		} else {
			t_n_frame++;
			t_ret = play_stream(t_p_block);
			if (t_ret == MW_AUDIO_NO_ERROR) {

			} else {
			}
		}

		DWORD dwRet = WaitForSingleObject(m_h_event_ready, INFINITE);
	}

	return 0;
}

DWORD MWMultiDSRender::audiothread_proc_r()
{
	int t_n_frame = 0;
	HRESULT t_hr = DS_OK;

	/* start playing the buffer */
	t_hr = IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0, DSBPLAY_LOOPING);
	if (t_hr == DSERR_BUFFERLOST) {
		IDirectSoundBuffer_Restore(m_p_dsbuffer);
		t_hr = IDirectSoundBuffer_Play(m_p_dsbuffer, 0, 0,
					       DSBPLAY_LOOPING);
	}
	HANDLE array_event[] = {m_h_event_ready, m_h_event_exit};

	while (m_b_playing) {
		mw_audio_block_t *t_p_block = NULL;
		mw_multi_ds_status_e t_ret =
			m_ds_buffer.get_audio_frames(t_p_block);
		if (t_ret != MW_AUDIO_NO_ERROR) {
			continue;
		} else {
			t_n_frame++;
			t_ret = fill_buffer(t_p_block);
			if (t_ret == MW_AUDIO_NO_ERROR) {
			} else {
			}
		}
		DWORD t_dw_ret =
			WaitForMultipleObjects(2, array_event, FALSE, INFINITE);
		if (t_dw_ret == WAIT_OBJECT_0) {

		} else {
			break;
		}
	}
	return 0;
}
