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
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
using namespace std;

extern "C"{
#include "LibMWMp4/mw_mp4.h"
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
}

#include "LibMWVenc/mw_venc.h"

#define CAPTURE_PIX_FMT    AV_PIX_FMT_NV12//AV_PIX_FMT_RGB32
#define CAPTURE_SAMPLE_FMT AV_SAMPLE_FMT_S16
#define SCALE_FLAGS SWS_BICUBIC

class CMp4FileSave
{
public:
	CMp4FileSave(void);
	~CMp4FileSave(void);

public:
	void InitMWVenc();
	void DeinitMWVenc();
    BOOL Init(
		char* pFileName, 
		int cx, 
		int cy, 
		int nFrameDuation, 
		int nSamplerate, 
		int nChannels);
	BOOL InitW(
		wchar_t* pFileName, 
		int cx, int cy, 
		int nFrameDuation, 
		int nSamplerate, 
		int nChannels);
	BOOL InitW(
		wchar_t* pFileName, 
		int cx, 
		int cy, 
		int nFrameDuation, 
		int nSamplerate, 
		int nChannels,
		int nIndex);
	
	void Destroy();

	int WriteVideoFrame(unsigned char* pBuffer,uint64_t u64TimeStamp);
	int WriteAudioframe(unsigned char* pBuffer, int cbSize,uint64_t u64TimeStamp);

	vector<mw_venc_gpu_info_t> GetGPUInfo();

public:
	static void cb_venc(
		void * user_ptr, 
		const uint8_t * p_frame, 
		uint32_t frame_len, 
		mw_venc_frame_info_t *p_frame_info)
	{
		CMp4FileSave* t_p_this = (CMp4FileSave*)user_ptr;
		if(t_p_this)
			t_p_this->cb_venc_proc(
				p_frame,
				frame_len,
				p_frame_info);
	}

protected:
	void cb_venc_proc(
		const uint8_t * p_frame, 
		uint32_t frame_len, 
		mw_venc_frame_info_t *p_frame_info);

	int WriteVideoFrameFFMPEG(unsigned char* pBuffer,uint64_t u64TimeStamp);
	int WriteVideoFrameVENC(unsigned char* pBuffer,uint64_t u64TimeStamp);

protected:

	int					m_cx;
	int					m_cy;
	int					m_nSampleRate;
    int                 m_nChannels;
    int                 m_nFps;
    int                 m_bSetVideo;
    int                 m_bSetAudio;
    mw_mp4_handle_t     m_hMp4File;
    AVCodecContext      *m_pVideoEncCtx;
    AVCodecContext      *m_pAudioEncCtx;
    SwrContext          *m_pAudioSwrCtx;
    AVFrame             *m_pAFrame;
	CMutex				m_muxWrite;

	uint8_t*			m_p_audio_data;
	uint8_t*			m_p_encode_data;
	uint32_t			m_u32_data_size;

	char				m_cs_file_name[256];
	char				m_cs_file_name_base[256];
	clock_t				m_clock_start;
	uint32_t			m_u32_file_index;

	wchar_t				m_wcs_file_name[256];
	wchar_t				m_wcs_file_name_base[256];

	bool				m_b_wchar;

	uint32_t			m_u32_audio_num;
	uint32_t			m_u32_audio_pkt_num;

	vector<mw_venc_gpu_info_t>	m_vec_gpu_info;
	mw_venc_param_t				m_param;
	mw_venc_handle_t			m_h_venc;
protected:
    int CreateVideoEncoder();
	int CreateVideoEncoder(int nIndex);
    int CreateAudioEncoder();
};
