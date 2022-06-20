/************************************************************************************************/
// MWMultiDSDefine.h : definition of MWMultiDSRender module

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

#ifndef MWMULTIDSDEFINE_H
#define MWMULTIDSDEFINE_H

typedef enum _device_role {
	MW_MM_CONSOLE = 0x00,
	MW_MM_MULTIMEDIA,
	MW_MM_COMMUNITION,
} mw_device_role;

typedef enum _device_state {
	MW_MM_ACTIVE = 0x00,
	MW_MM_DISABLED,
	MW_MM_NOTPRESENT,
	MW_MM_UNPLUGGED,
} mw_device_state;

typedef void (*LPFN_MM_DEFAULTDEVICE_CHANGED_CALLBACK)(mw_device_role t_role,
						       void *t_p_param);

typedef enum _multi_ds_status {
	MW_AUDIO_NO_ERROR = 0x00,
	MW_AUDIO_INVALID_PARAM,

	MW_AUDIO_DSOUND_LOAD_FAILED = 0x10,
	MW_AUDIO_DSOUND_CREATE_FAILED,
	MW_AUDIO_DSOUND_NO_DEVICE,
	MW_AUDIO_DSOUND_CREATE_DEV_FAILED,
	MW_AUDIO_DSOUND_GETSPEAKER_FAILED,
	MW_AUDIO_DSOUND_INIT_FAILED,
	MW_AUDIO_DSOUND_CREATE_DSBUFFER_FAILED,
	MW_AUDIO_DSOUND_DSBUFFER_LOCK_FAILED,
	MW_AUDIO_DSOUND_DXDEVICE_FAILED,
	MW_AUDIO_DSOUND_CLEARDATA_FAILED,
	MW_AUDIO_DSOUND_VOL_OVERFLOW,
	MW_AUDIO_DSOUND_VOL_SETFAILED,
	MW_AUDIO_DSOUND_VOL_MUTEFAILED,
	MW_AUDIO_DSOUND_STATUS_FAILED,
	MW_AUDIO_DSOUND_POSITION_FAILED,
	MW_AUDIO_DSOUND_POSITION_ERR,
	MW_AUDIO_DSOUND_FLUSH_FAILED,
	MW_AUDIO_DSOUND_PAUSE_FAILED,
	MW_AUDIO_ALREADY_RUN,
	MW_AUDIO_DSOUND_FILL_FAILED,
	MW_AUDIO_PLAY_FAILED,

	MW_AUDIO_BUF_SIZE_OVERFLOW,
	MW_AUDIO_BUF_NUM_OVERFLOW,
	MW_AUDIO_BUF_NULL,
	MW_AUDIO_BUF_SIZE_ERROR,
	MW_AUDIO_BUF_OVERRUN,
	MW_AUDIO_BUF_EMPTY,

	MW_AUDIO_COM_FAILED,
	MW_AUDIO_MMDEVICE_ENUM_FAILED,
	MW_AUDIO_REGISTER_CALLBACK_FAILED,
	MW_AUDIO_UNREGISTER_CALLBACK_FAILED,
	MW_AUDIO_MMDEVICE_GETDEFAULT_FAILED,

	MW_AUDIO_WASAPI_DEV_ERR,
	MW_AUDIO_WASAPI_DEV_NOTIFY_ERROR,
} mw_multi_ds_status_e;

typedef enum {
	MW_AUDIO_BLOCK_EMPTY = 0x00,
	MW_AUDIO_BLOCK_BUFFERING,
	MW_AUDIO_BLOCK_BUFFERD,
} mw_audio_block_state_e;

typedef struct _mmdevice {
	WCHAR m_wcs_id[128];
	WCHAR m_wcs_name[128];

	GUID m_guid;
} mw_mmdevice_t;

typedef struct _audio_block {
	uint8_t *m_p_data;
	uint32_t m_u32_size;

	mw_audio_block_state_e m_block_state;
} mw_audio_block_t;

typedef enum _audio_type {
	MW_AUDIO_FORMAT_PCM_08 = 0x00,
	MW_AUDIO_FORMAT_PCM_16,
	MW_AUDIO_FORMAT_PCM_20,
	MW_AUDIO_FORMAT_PCM_24,
	MW_AUDIO_FORMAT_PCM_32,
} mw_audio_type_e;

typedef uint32_t mw_audio_channel_type_t;
/* Values available for mw_audio_channel_type_t(audio channels) */
#define MW_CHANNEL_FRONT_LEFT 0x0001
#define MW_CHANNEL_FRONT_RIGHT 0x0002
#define MW_CHANNEL_FRONT_CENTER 0x0004
#define MW_CHANNEL_LOW_FREQ 0x0008
#define MW_CHANNEL_BACK_LEFT 0x0010
#define MW_CHANNEL_BACK_RIGHT 0x0020
#define MW_CHANNEL_FRONT_LEFT_OF_CENTER 0x0040
#define MW_CHANNEL_FRONT_RIGHT_OF_CENTER 0x0080
#define MW_CHANNEL_BACK_CENTER 0x0100
#define MW_CHANNEL_SIDE_LEFT 0x0200
#define MW_CHANNEL_SIDE_RIGHT 0x0400

#define MW_CHANNELS_1_0 (MW_CHANNEL_FRONT_CENTER)
#define MW_CHANNELS_2_0 (MW_CHANNEL_FRONT_LEFT | MW_CHANNEL_FRONT_RIGHT)
#define MW_CHANNELS_3_0 (MW_CHANNELS_2_0 | MW_CHANNELS_1_0)
#define MW_CHANNELS_3_0_SURD (MW_CHANNELS_2_0 | MW_CHANNEL_BACK_CENTER)
#define MW_CHANNELS_4_0 \
	(MW_CHANNELS_2_0 | MW_CHANNEL_BACK_LEFT | MW_CHANNEL_BACK_RIGHT)
#define MW_CHANNELS_4_0_SURD (MW_CHANNELS_3_0 | MW_CHANNELS_3_0_SURD)
#define MW_CHANNELS_5_0 (MW_CHANNELS_3_0 | MW_CHANNELS_4_0)
#define MW_CHANNELS_5_0_SIDE \
	(MW_CHANNELS_3_0 | MW_CHANNEL_SIDE_LEFT | MW_CHANNEL_SIDE_RIGHT)
#define MW_CHANNELS_5_1 (MW_CHANNELS_5_0 | MW_CHANNEL_LOW_FREQ)
#define MW_CHANNELS_6_0 (MW_CHANNELS_4_0 | MW_CHANNELS_4_0_SURD)
#define MW_CHANNELS_6_0_SIDE (MW_CHANNELS_5_0_SIDE | MW_CHANNELS_4_0_SURD)
#define MW_CHANNELS_7_0                                      \
	(MW_CHANNELS_5_0 | MW_CHANNEL_FRONT_LEFT_OF_CENTER | \
	 MW_CHANNEL_FRONT_RIGHT_OF_CENTER)
#define MW_CHANNELS_7_0_SIDE                                      \
	(MW_CHANNELS_5_0_SIDE | MW_CHANNEL_FRONT_LEFT_OF_CENTER | \
	 MW_CHANNEL_FRONT_RIGHT_OF_CENTER)
#define MW_CHANNELS_7_0_SURD (MW_CHANNELS_5_0 | MW_CHANNELS_5_0_SIDE)
#define MW_CHANNELS_7_1 (MW_CHANNELS_7_0 | MW_CHANNEL_LOW_FREQ)

typedef struct _audio_format {
	mw_audio_type_e m_audio_type;
	int m_n_sample_rate;
	int m_n_channels;
	int m_n_bitspersample;
	int m_n_timeuint; //per time unit means 10 ms
	mw_audio_channel_type_t m_channel_type;
} mw_audio_format_t;

typedef int64_t mtime_t;

#endif