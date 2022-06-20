/************************************************************************************************/
// Mp4FileSave.cpp : implementation of the CMp4FileSave class

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
#include "Mp4FileSave.h"

int GetH264Xps(unsigned char *pStream, int nLen, uint8_t **pSps,
	       int16_t *pSpsLen, uint8_t **pPps, int16_t *pPpsLen);

CMp4FileSave::CMp4FileSave(void)
{
	m_cx = 0;
	m_cy = 0;
	m_nSampleRate = 0;
	m_nChannels = 2;
	m_nFps = 30;
	m_bSetVideo = 0;
	m_bSetAudio = 0;
	m_hMp4File = NULL;
	m_pVideoEncCtx = NULL;
	m_pAudioEncCtx = NULL;
	m_pAudioSwrCtx = NULL;
	m_pAFrame = NULL;

	m_p_audio_data = NULL;
	m_p_encode_data = NULL;

	m_p_audio_data = new uint8_t[192000 * 8 * 4];
	m_p_encode_data = new uint8_t[1024 * 8];
	m_u32_data_size = 0;

	m_b_wchar = false;

	m_h_venc = NULL;
	InitMWVenc();
}

CMp4FileSave::~CMp4FileSave(void)
{
	if (m_p_audio_data) {
		delete m_p_audio_data;
		m_p_audio_data = NULL;
	}

	if (m_p_encode_data) {
		delete m_p_encode_data;
		m_p_encode_data = NULL;
	}

	DeinitMWVenc();
}

void CMp4FileSave::InitMWVenc()
{
	mw_venc_status_t t_stat = mw_venc_init();
	if (t_stat != MW_VENC_STATUS_SUCCESS)
		return;
	int32_t t_i32_num = mw_venc_get_gpu_num();
	for (int i = 0; i < t_i32_num; i++) {
		mw_venc_gpu_info_t t_info = {0};
		t_stat = mw_venc_get_gpu_info_by_index(i, &t_info);
		if (t_stat != MW_VENC_STATUS_SUCCESS)
			continue;
		m_vec_gpu_info.push_back(t_info);
	}
}

void CMp4FileSave::DeinitMWVenc()
{
	mw_venc_deinit();
}

int CMp4FileSave::CreateAudioEncoder()
{
	if (m_p_encode_data == NULL)
		m_p_audio_data = new uint8_t[192000 * 8 * 4];

	if (m_p_encode_data == NULL)
		m_p_encode_data = new uint8_t[1024 * 8];

	if (m_p_audio_data == NULL || m_p_encode_data == NULL)
		return -1;

	int i;
	int need_us_swr = 0;
	AVCodec *pAudioCodec;
	pAudioCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!pAudioCodec) {
		return -1;
	}
	m_pAudioEncCtx = avcodec_alloc_context3(pAudioCodec);
	if (!m_pAudioEncCtx) {
		return -2;
	}

	m_pAudioEncCtx->bit_rate = 64000;

	//support sample_fmt
	m_pAudioEncCtx->sample_fmt = CAPTURE_SAMPLE_FMT;
	if (pAudioCodec->sample_fmts) {
		m_pAudioEncCtx->sample_fmt = pAudioCodec->sample_fmts[0];
		for (i = 0; pAudioCodec->sample_fmts[i] != AV_SAMPLE_FMT_NONE;
		     i++) {
			if (pAudioCodec->sample_fmts[i] == CAPTURE_SAMPLE_FMT) {
				m_pAudioEncCtx->sample_fmt = CAPTURE_SAMPLE_FMT;
			}
		}
		if (m_pAudioEncCtx->sample_fmt != CAPTURE_SAMPLE_FMT) {
			need_us_swr = 1;
		}
	}
	//support sample_rate
	m_pAudioEncCtx->sample_rate = m_nSampleRate;
	if (pAudioCodec->supported_samplerates) {
		m_pAudioEncCtx->sample_rate =
			pAudioCodec->supported_samplerates[0];
		for (i = 0; pAudioCodec->supported_samplerates[i]; i++) {
			if (pAudioCodec->supported_samplerates[i] ==
			    m_nSampleRate) {
				m_pAudioEncCtx->sample_rate = m_nSampleRate;
				break;
			}
		}
		if (m_pAudioEncCtx->sample_rate != m_nSampleRate) {
			need_us_swr = 1;
		}
	}

	//support channels
	m_pAudioEncCtx->channels = m_nChannels;
	m_pAudioEncCtx->channel_layout =
		av_get_default_channel_layout(m_nChannels);
	if (pAudioCodec->channel_layouts) {
		m_pAudioEncCtx->channel_layout =
			pAudioCodec->channel_layouts[0];
		m_pAudioEncCtx->channels = av_get_channel_layout_nb_channels(
			m_pAudioEncCtx->channel_layout);
		for (i = 0; pAudioCodec->channel_layouts[i]; i++) {
			int channels = av_get_channel_layout_nb_channels(
				pAudioCodec->channel_layouts[i]);
			if (channels == m_nChannels) {
				m_pAudioEncCtx->channels = m_nChannels;
				m_pAudioEncCtx->channel_layout =
					pAudioCodec->channel_layouts[i];
			}
		}
		if (m_pAudioEncCtx->channels != m_nChannels) {
			need_us_swr = 1;
		}
	}

	if (avcodec_open2(m_pAudioEncCtx, pAudioCodec, NULL) < 0) {
		return -3;
	}
	/* create resampler context */
	m_pAFrame = av_frame_alloc();
	if (!m_pAFrame) {
		return -4;
	}
	m_pAFrame->format = m_pAudioEncCtx->sample_fmt;
	m_pAFrame->channel_layout = m_pAudioEncCtx->channel_layout;
	m_pAFrame->sample_rate = m_pAudioEncCtx->sample_rate;
	m_pAFrame->nb_samples = 1024;
	if (need_us_swr) {
		m_pAudioSwrCtx = swr_alloc();
		if (!m_pAudioSwrCtx) {
			return -5;
		}
		/* set options */
		av_opt_set_int(m_pAudioSwrCtx, "in_channel_count", m_nChannels,
			       0);
		av_opt_set_int(m_pAudioSwrCtx, "in_sample_rate", m_nSampleRate,
			       0);
		av_opt_set_sample_fmt(m_pAudioSwrCtx, "in_sample_fmt",
				      CAPTURE_SAMPLE_FMT, 0);
		av_opt_set_int(m_pAudioSwrCtx, "out_channel_count",
			       m_pAudioEncCtx->channels, 0);
		av_opt_set_int(m_pAudioSwrCtx, "out_sample_rate",
			       m_pAudioEncCtx->sample_rate, 0);
		av_opt_set_sample_fmt(m_pAudioSwrCtx, "out_sample_fmt",
				      m_pAudioEncCtx->sample_fmt, 0);

		/* initialize the resampling context */
		if (swr_init(m_pAudioSwrCtx) < 0) {
			return -6;
		}
		av_frame_get_buffer(m_pAFrame, 0);
	}

	m_u32_data_size = 0;

	return 0;
}

void CMp4FileSave::cb_venc_proc(const uint8_t *p_frame, uint32_t frame_len,
				mw_venc_frame_info_t *p_frame_info)
{
	// split mp4 file
	// per hour
	if (m_clock_start > 0) {
		uint64_t t_u64_now = clock();
		if (t_u64_now - m_clock_start >
		    CLOCKS_PER_SEC * 60 * 60) { // split file per hour
			m_muxWrite.Lock();
			if (p_frame_info->frame_type ==
				    mw_venc_frame_type_t::MW_VENC_FRAME_TYPE_IDR ||
			    p_frame_info->frame_type ==
				    mw_venc_frame_type_t::MW_VENC_FRAME_TYPE_I) {
				if (m_hMp4File) {
					mw_mp4_close(m_hMp4File);
					m_hMp4File = NULL;
				}
				if (m_hMp4File == NULL) {
					if (m_b_wchar) {
						wchar_t t_wcs_file_name[512] = {
							0};
						m_u32_file_index++;
						wsprintf(t_wcs_file_name,
							 L"%s_%d.mp4",
							 m_wcs_file_name_base,
							 m_u32_file_index);
						m_hMp4File = mw_mp4_open_ex(
							t_wcs_file_name);
						m_bSetVideo = 0;
						m_bSetAudio = 0;
					} else {
						char t_cs_file_name[512] = {0};
						m_u32_file_index++;
						sprintf(t_cs_file_name,
							"%s_%d.mp4",
							m_cs_file_name_base,
							m_u32_file_index);
						m_hMp4File = mw_mp4_open(
							t_cs_file_name);
						m_bSetVideo = 0;
						m_bSetAudio = 0;
					}
				}
			}
			m_muxWrite.Unlock();
		}
	}

	if (0 == m_bSetVideo) {
		mw_mp4_video_info_t video_track_info;
		memset(&video_track_info, 0, sizeof(mw_mp4_video_info_t));
		video_track_info.codec_type = MW_MP4_VIDEO_TYPE_H264;
		video_track_info.timescale = 1000;
		video_track_info.width = m_param.width;
		video_track_info.height = m_param.height;
		video_track_info.h264.sps = NULL;
		video_track_info.h264.sps_size = 0;
		video_track_info.h264.pps = NULL;
		video_track_info.h264.pps_size = 0;

		if (GetH264Xps((unsigned char *)p_frame, frame_len,
			       &video_track_info.h264.sps,
			       &video_track_info.h264.sps_size,
			       &video_track_info.h264.pps,
			       &video_track_info.h264.pps_size)) {
			OutputDebugString(L"error extradata\n");
			return;
		}
		m_muxWrite.Lock();
		mw_mp4_status_t t_stat =
			mw_mp4_set_video(m_hMp4File, &video_track_info);
		if (MW_MP4_STATUS_SUCCESS == t_stat) {
			OutputDebugString(L"Set Video track successfully\n");
		} else {
			OutputDebugString(L"Set Video track failed\n");
		}
		m_muxWrite.Unlock();
		m_bSetVideo = 1;

		m_clock_start = clock();
	}

	m_muxWrite.Lock();
	//uint64_t t_u64_timestamp = GetTickCount();
	mw_mp4_status_t t_stat = mw_mp4_write_video(
		m_hMp4File, p_frame, frame_len, p_frame_info->pts);
	if (MW_MP4_STATUS_SUCCESS != t_stat)
		OutputDebugString(L"write video frame failed\n");
	m_muxWrite.Unlock();
}

int CMp4FileSave::WriteVideoFrameFFMPEG(unsigned char *pBuffer,
					uint64_t u64TimeStamp)
{
	int got_packet;
	AVFrame Frame = {0};

	AVPacket pkt = {0};
	if (!m_pVideoEncCtx) {
		OutputDebugStringA("WriteVideoFrame - no video enc\n");
		return -1;
	}
	Frame.format = m_pVideoEncCtx->pix_fmt;
	Frame.width = m_pVideoEncCtx->width;
	Frame.height = m_pVideoEncCtx->height;
	Frame.pts = u64TimeStamp;

	/* a hack to avoid data copy with some raw video muxers */
	av_init_packet(&pkt);
	//nv12
	Frame.data[0] = pBuffer;
	Frame.data[1] =
		pBuffer + m_pVideoEncCtx->width * m_pVideoEncCtx->height;
	Frame.linesize[0] = m_pVideoEncCtx->width;
	Frame.linesize[1] = m_pVideoEncCtx->width;

	avcodec_encode_video2(m_pVideoEncCtx, &pkt, &Frame, &got_packet);
	if ((0 == got_packet) || (0 == pkt.size)) {
		return 0;
	}
	if (NULL == m_hMp4File) {
		OutputDebugStringA("mp4 file doesn't exist\n");
		return -1;
	}

	// split mp4 file
	// per hour
	if (m_clock_start > 0) {
		uint64_t t_u64_now = clock();
		if (t_u64_now - m_clock_start >
		    CLOCKS_PER_SEC * 60 * 60) { // split file per hour
			m_muxWrite.Lock();
			if (pkt.flags & AV_PKT_FLAG_KEY) {
				if (m_hMp4File) {
					mw_mp4_close(m_hMp4File);
					m_hMp4File = NULL;
				}
				if (m_hMp4File == NULL) {
					if (m_b_wchar) {
						wchar_t t_wcs_file_name[512] = {
							0};
						m_u32_file_index++;
						wsprintf(t_wcs_file_name,
							 L"%s_%d.mp4",
							 m_wcs_file_name_base,
							 m_u32_file_index);
						m_hMp4File = mw_mp4_open_ex(
							t_wcs_file_name);
						m_bSetVideo = 0;
						m_bSetAudio = 0;
					} else {
						char t_cs_file_name[512] = {0};
						m_u32_file_index++;
						sprintf(t_cs_file_name,
							"%s_%d.mp4",
							m_cs_file_name_base,
							m_u32_file_index);
						m_hMp4File = mw_mp4_open(
							t_cs_file_name);
						m_bSetVideo = 0;
						m_bSetAudio = 0;
					}
				}
			}
			m_muxWrite.Unlock();
		}
	}

	if (0 == m_bSetVideo) {
		mw_mp4_video_info_t video_track_info;
		memset(&video_track_info, 0, sizeof(mw_mp4_video_info_t));
		video_track_info.codec_type = MW_MP4_VIDEO_TYPE_H264;
		video_track_info.timescale = 1000;
		video_track_info.width = m_pVideoEncCtx->width;
		video_track_info.height = m_pVideoEncCtx->height;
		video_track_info.h264.sps = NULL;
		video_track_info.h264.sps_size = 0;
		video_track_info.h264.pps = NULL;
		video_track_info.h264.pps_size = 0;

		if (GetH264Xps(pkt.data, pkt.size, &video_track_info.h264.sps,
			       &video_track_info.h264.sps_size,
			       &video_track_info.h264.pps,
			       &video_track_info.h264.pps_size)) {
			OutputDebugString(L"error extradata\n");
			return -2;
		}
		m_muxWrite.Lock();
		mw_mp4_status_t t_stat =
			mw_mp4_set_video(m_hMp4File, &video_track_info);
		if (MW_MP4_STATUS_SUCCESS == t_stat) {
			OutputDebugString(L"Set Video track successfully\n");
		} else {
			OutputDebugString(L"Set Video track failed\n");
		}
		m_muxWrite.Unlock();
		m_bSetVideo = 1;

		m_clock_start = clock();
	}

	m_muxWrite.Lock();
	//uint64_t t_u64_timestamp = GetTickCount();
	mw_mp4_status_t t_stat =
		mw_mp4_write_video(m_hMp4File, pkt.data, pkt.size, pkt.pts);
	if (MW_MP4_STATUS_SUCCESS != t_stat)
		OutputDebugString(L"write video frame failed\n");

	av_free_packet(&pkt);

	m_muxWrite.Unlock();
	return 0;
}

int CMp4FileSave::WriteVideoFrameVENC(unsigned char *pBuffer,
				      uint64_t u64TimeStamp)
{
	if (m_h_venc)
		mw_venc_put_frame_ex(m_h_venc, pBuffer, u64TimeStamp);

	return 0;
}

int CMp4FileSave::CreateVideoEncoder()
{
	AVCodec *pVideoCodec;
	pVideoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pVideoCodec) {
		OutputDebugStringA("Could not find encoder for h264\n");
		return -1;
	}

	m_pVideoEncCtx = avcodec_alloc_context3(pVideoCodec);
	if (!m_pVideoEncCtx) {
		printf("Could not alloc an encoding context\n");
		return -2;
	}
	m_pVideoEncCtx->codec_id = AV_CODEC_ID_H264;
	m_pVideoEncCtx->bit_rate = 4000000;
	m_pVideoEncCtx->width = m_cx;
	m_pVideoEncCtx->height = m_cy;

	av_opt_set(m_pVideoEncCtx->priv_data, "preset", "ultrafast", 0);
	av_opt_set(m_pVideoEncCtx->priv_data, "tune", "zerolatency", 0);
	AVRational base = {1, m_nFps};
	m_pVideoEncCtx->time_base = base;

	m_pVideoEncCtx->gop_size =
		60; /* emit one intra frame every twelve frames at most */
	m_pVideoEncCtx->pix_fmt =
		CAPTURE_PIX_FMT; //may not support nv12   pVideoCodec->pix_fmts and sws
	if (avcodec_open2(m_pVideoEncCtx, pVideoCodec, NULL) < 0) {
		OutputDebugStringA("Could not open video codec\n");
		return -3;
	}
	return 0;
}

int CMp4FileSave::CreateVideoEncoder(int nIndex)
{
	//
	mw_venc_get_default_param(&m_param);

	m_param.width = m_cx;
	m_param.height = m_cy;
	m_param.amd_mem_reserved = mw_venc_amd_mem_type_t::MW_VENC_AMD_MEM_AUTO;
	m_param.code_type = mw_venc_code_type_t::MW_VENC_CODE_TYPE_H264;
	m_param.fourcc = mw_venc_fourcc_t::MW_VENC_FOURCC_NV12;
	m_param.fps.num = m_nFps;
	m_param.fps.den = 1;
	m_param.gop_pic_size = 60;
	m_param.profile = mw_venc_profile_t::MW_VENC_PROFILE_H264_BASELINE;
	m_param.rate_control.target_bitrate = 8192;
	m_param.rate_control.mode =
		mw_venc_rate_control_mode_t::MW_VENC_RATECONTROL_CBR;
	m_param.targetusage =
		mw_venc_targetusage_t::MW_VENC_TARGETUSAGE_BEST_SPEED;

	m_h_venc = mw_venc_create_by_index(nIndex - 1, &m_param, cb_venc, this);
	if (m_h_venc == NULL)
		return 1;

	return 0;
}

int CMp4FileSave::Init(char *pFileName, int cx, int cy, int nFrameDuation,
		       int nSamplerate, int nChannels)
{
	m_b_wchar = false;

	m_u32_file_index = 0;
	m_clock_start = 0;
	m_u32_audio_num = 0;
	m_u32_audio_pkt_num = 0;

	m_bSetVideo = 0;
	m_bSetAudio = 0;

	memset(m_cs_file_name, 0, 256);
	memset(m_cs_file_name_base, 0, 256);
	char *t_p_pos = strstr(pFileName, ".mp4");
	if (t_p_pos != NULL) {
		int t_n_len = t_p_pos - pFileName;
		memcpy(m_cs_file_name_base, pFileName, t_p_pos - pFileName);
		sprintf(m_cs_file_name, "%s.mp4", m_cs_file_name_base);
	} else {
		sprintf(m_cs_file_name, "%s.mp4", pFileName);
		sprintf(m_cs_file_name_base, "%s", pFileName);
	}

	av_register_all();
	if (nSamplerate && nChannels) {
		m_nSampleRate = nSamplerate;
		m_nChannels = nChannels;
		CreateAudioEncoder();
	}
	if (nFrameDuation && cx && cy) {
		m_cx = cx;
		m_cy = cy;
		m_nFps = 1000 / nFrameDuation;
		CreateVideoEncoder();
	}
	if (m_pAudioEncCtx || m_pVideoEncCtx) {
		m_hMp4File = mw_mp4_open(m_cs_file_name);
	} else {
		return -1;
	}
	return 0;
}

BOOL CMp4FileSave::InitW(wchar_t *pFileName, int cx, int cy, int nFrameDuation,
			 int nSamplerate, int nChannels)
{
	m_b_wchar = true;

	m_u32_file_index = 0;
	m_clock_start = 0;
	m_u32_audio_num = 0;
	m_u32_audio_pkt_num = 0;

	m_bSetVideo = 0;
	m_bSetAudio = 0;

	memset(m_wcs_file_name, 0, 256 * sizeof(wchar_t));
	memset(m_wcs_file_name_base, 0, 256 * sizeof(wchar_t));
	wchar_t *t_p_pos = wcsstr(pFileName, L".mp4");
	if (t_p_pos != NULL) {
		int t_n_len = (t_p_pos - pFileName) * sizeof(wchar_t);
		memcpy(m_wcs_file_name_base, pFileName, t_n_len);
		wsprintf(m_wcs_file_name, L"%s.mp4", m_wcs_file_name_base);
	} else {
		wsprintf(m_wcs_file_name, L"%s.mp4", pFileName);
		wsprintf(m_wcs_file_name_base, L"%s", pFileName);
	}

	av_register_all();
	if (nSamplerate && nChannels) {
		m_nSampleRate = nSamplerate;
		m_nChannels = nChannels;
		CreateAudioEncoder();
	}
	if (nFrameDuation && cx && cy) {
		m_cx = cx;
		m_cy = cy;
		m_nFps = 1000 / nFrameDuation;
		CreateVideoEncoder();
	}
	if (m_pAudioEncCtx || m_pVideoEncCtx) {
		m_hMp4File = mw_mp4_open_ex(m_wcs_file_name);
	} else {
		Destroy();
		return FALSE;
	}
	return TRUE;
}

BOOL CMp4FileSave::InitW(wchar_t *pFileName, int cx, int cy, int nFrameDuation,
			 int nSamplerate, int nChannels, int nIndex)
{
	if (nIndex < 0 || nIndex > m_vec_gpu_info.size())
		return FALSE;

	if (nIndex == 0) { // for ffmpeg cpu
		return InitW(pFileName, cx, cy, nFrameDuation, nSamplerate,
			     nChannels);
	}

	m_b_wchar = true;

	m_u32_file_index = 0;
	m_clock_start = 0;
	m_u32_audio_num = 0;
	m_u32_audio_pkt_num = 0;

	m_bSetVideo = 0;
	m_bSetAudio = 0;

	memset(m_wcs_file_name, 0, 256 * sizeof(wchar_t));
	memset(m_wcs_file_name_base, 0, 256 * sizeof(wchar_t));
	wchar_t *t_p_pos = wcsstr(pFileName, L".mp4");
	if (t_p_pos != NULL) {
		int t_n_len = (t_p_pos - pFileName) * sizeof(wchar_t);
		memcpy(m_wcs_file_name_base, pFileName, t_n_len);
		wsprintf(m_wcs_file_name, L"%s.mp4", m_wcs_file_name_base);
	} else {
		wsprintf(m_wcs_file_name, L"%s.mp4", pFileName);
		wsprintf(m_wcs_file_name_base, L"%s", pFileName);
	}

	av_register_all();
	if (nSamplerate && nChannels) {
		m_nSampleRate = nSamplerate;
		m_nChannels = nChannels;
		CreateAudioEncoder();
	}
	if (nFrameDuation && cx && cy) {
		m_cx = cx;
		m_cy = cy;
		m_nFps = 1000 / nFrameDuation;
		CreateVideoEncoder(nIndex);
	}
	if (m_pAudioEncCtx || m_h_venc) {
		m_hMp4File = mw_mp4_open_ex(m_wcs_file_name);
	} else {
		Destroy();
		return FALSE;
	}

	return TRUE;
}

unsigned char *find_start_code(unsigned char *p_stream, uint32_t stream_len,
			       uint32_t *p_zero_num_prev_start_code)
{
	unsigned char *p_find_zero;
	unsigned char *p_temp = p_stream;
	unsigned char *p_end = p_stream + stream_len;
	*p_zero_num_prev_start_code = 0;
	p_temp += 2;
	while (p_temp < p_end) {
		if (*p_temp > 1) {
			p_temp += 3;
			continue;
		}
		if (*p_temp == 0) {
			p_temp++;
			continue;
		}
		if (p_temp[-1] || p_temp[-2]) {
			p_temp += 3;
			continue;
		}
		p_find_zero = p_temp - 3;
		p_temp -= 2;

		while (p_find_zero >= p_stream) {
			if (*p_find_zero) {
				break;
			}
			p_find_zero--;
			(*p_zero_num_prev_start_code)++;
		}
		return p_temp;
	}
	return NULL;
}
int GetH264Xps(unsigned char *pStream, int nLen, uint8_t **pSps,
	       int16_t *pSpsLen, uint8_t **pPps, int16_t *pPpsLen)
{
	uint32_t nZeroNumPrevStartCode = 0;
	*pSpsLen = 0;
	*pPpsLen = 0;
	unsigned char *pStartCode =
		find_start_code(pStream, nLen, &nZeroNumPrevStartCode);
	nLen -= pStartCode - pStream;
	while (pStartCode && ((*pSpsLen == 0) || (*pPpsLen == 0))) {
		unsigned char *pTempStartCode = find_start_code(
			pStartCode + 3, nLen - 3, &nZeroNumPrevStartCode);
		;
		if ((pStartCode[3] & 0x1f) == 7) {
			*pSps = pStartCode + 3;
			if (NULL == pTempStartCode) {
				*pSpsLen = (int16_t)(nLen - 3);
			} else {
				*pSpsLen =
					(int16_t)(pTempStartCode - pStartCode -
						  nZeroNumPrevStartCode - 3);
			}
		} else if ((pStartCode[3] & 0x1f) == 8) {
			*pPps = pStartCode + 3;
			if (NULL == pTempStartCode) {
				*pPpsLen = (int16_t)(nLen - 3);
			} else {
				*pPpsLen =
					(int16_t)(pTempStartCode - pStartCode -
						  nZeroNumPrevStartCode - 3);
			}
		}
		nLen -= pTempStartCode - pStartCode;
		pStartCode = pTempStartCode;
	}
	if ((*pSpsLen != 0) && (*pPpsLen != 0)) {
		return 0;
	}
	return -1;
}

int CMp4FileSave::WriteVideoFrame(unsigned char *pBuffer, uint64_t u64TimeStamp)
{
	if (m_pVideoEncCtx)
		return WriteVideoFrameFFMPEG(pBuffer, u64TimeStamp);
	else if (m_h_venc)
		return WriteVideoFrameVENC(pBuffer, u64TimeStamp);
	else
		return -1;
}
int GetAacProfile(unsigned char *pExtraData, int nLen, uint8_t *pAacProfile)
{
	if ((0 == nLen) || (NULL == pExtraData)) {
		printf("error aac ex\n");
		return -1;
	}
	*pAacProfile = ((pExtraData[0] & 0xf8) >> 3) - 1;
	return 0;
}
int CMp4FileSave::WriteAudioframe(unsigned char *pBuffer, int cbSize,
				  uint64_t u64TimeStamp)
{
	memcpy(m_p_audio_data + m_u32_data_size, pBuffer, cbSize);
	m_u32_data_size += cbSize;

	m_u32_audio_num++;

	if (m_u32_data_size < 1024 * 4)
		return 0;
	else {
		memcpy(m_p_encode_data, m_p_audio_data, 1024 * 4);
		m_u32_data_size -= (1024 * 4);
		memcpy(m_p_audio_data, m_p_audio_data + 1024 * 4,
		       m_u32_data_size);
		pBuffer = m_p_encode_data;
		cbSize = 1024 * 4;
	}

	int got_packet = 0;
	AVPacket pkt = {0};
	if (!m_pAudioEncCtx) {
		return -1;
	}
	if (m_pAudioSwrCtx) {
		uint8_t *pInData[8] = {0};
		pInData[0] = pBuffer;
		if (swr_convert(m_pAudioSwrCtx, m_pAFrame->data, cbSize * 4,
				(const uint8_t **)pInData, cbSize / 4) < 0) {
			OutputDebugString(L"Error while converting\n");
			return -1;
		}
	} else {
		m_pAFrame->data[0] = pBuffer;
	}
	//s16
	m_pAFrame->nb_samples = cbSize / 4;
	m_pAFrame->format = m_pAudioEncCtx->sample_fmt;
	m_pAFrame->channel_layout = m_pAudioEncCtx->channel_layout;
	m_pAFrame->sample_rate = m_nSampleRate;
	//m_pAFrame->pts = u64TimeStamp;

	avcodec_encode_audio2(m_pAudioEncCtx, &pkt, m_pAFrame, &got_packet);
	if ((0 == got_packet) || (0 == pkt.size)) {
		OutputDebugString(L"no packet\n");
		return 0;
	}
	if (NULL == m_hMp4File) {
		OutputDebugString(L"no file\n");
		return -1;
	}

	m_u32_audio_pkt_num++;

	if (0 == m_bSetAudio) {
		mw_mp4_audio_info_t audio_track_info;
		audio_track_info.codec_type = MW_MP4_AUDIO_TYPE_AAC;
		audio_track_info.timescale = 1000;
		audio_track_info.channels = m_pAudioEncCtx->channels;
		audio_track_info.sample_rate = m_pAudioEncCtx->sample_rate;
		audio_track_info.profile = 0;

		if (GetAacProfile(m_pAudioEncCtx->extradata,
				  m_pAudioEncCtx->extradata_size,
				  &(audio_track_info.profile))) {
			OutputDebugString(L"error aac\n");
			return -2;
		}

		m_muxWrite.Lock();
		mw_mp4_status_t t_stat =
			mw_mp4_set_audio(m_hMp4File, &audio_track_info);
		if (MW_MP4_STATUS_SUCCESS == t_stat) {
			OutputDebugString(L"Set Audio track successfully\n");
		} else {
			OutputDebugString(L"Set Audio track failed\n");
		}

		m_u32_audio_num = 0;
		m_u32_audio_pkt_num = 0;
		m_bSetAudio = 1;

		m_muxWrite.Unlock();
	}

	m_muxWrite.Lock();

	if (m_bSetAudio == 1) {
		//uint64_t t_u64_timestamp = GetTickCount();
		mw_mp4_status_t t_stat = mw_mp4_write_audio(
			m_hMp4File, pkt.data, pkt.size, u64TimeStamp);
		//mw_mp4_status_t t_stat = mw_mp4_write_audio(m_hMp4File, pkt.data, pkt.size, pkt.pts);

		if (MW_MP4_STATUS_SUCCESS != t_stat)
			OutputDebugString(L"write audio frame failed\n");
	}

	av_free_packet(&pkt);

	m_muxWrite.Unlock();

	return 0;
}

vector<mw_venc_gpu_info_t> CMp4FileSave::GetGPUInfo()
{
	return m_vec_gpu_info;
}

void CMp4FileSave::Destroy()
{
	m_muxWrite.Lock();
	if (m_hMp4File) {
		mw_mp4_close(m_hMp4File);
		m_hMp4File = NULL;
	}
	m_muxWrite.Unlock();

	if (m_pVideoEncCtx)
		avcodec_free_context(&m_pVideoEncCtx);
	if (m_pAudioEncCtx)
		avcodec_free_context(&m_pAudioEncCtx);
	if (m_pAFrame)
		av_frame_free(&m_pAFrame);
	if (m_pAudioSwrCtx)
		swr_free(&m_pAudioSwrCtx);

	if (m_h_venc) {
		mw_venc_destory(m_h_venc);
		m_h_venc = NULL;
	}
}
