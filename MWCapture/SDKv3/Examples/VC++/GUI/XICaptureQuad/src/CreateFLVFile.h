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

extern "C" {

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

//#define STREAM_FRAME_RATE 60 /* 25 images/s */
#define STREAM_PIX_FMT AV_PIX_FMT_YUV420P /* default pix_fmt */
#define CAPTURE_PIX_FMT AV_PIX_FMT_RGB32

#define SCALE_FLAGS SWS_BICUBIC

// a wrapper around a single output AVStream
typedef struct OutputStream {
	AVStream *st;
	AVCodecContext *enc;

	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;

	AVFrame *frame;
	AVFrame *tmp_frame;

	float t, tincr, tincr2;

	struct SwsContext *sws_ctx;
	struct SwrContext *swr_ctx;
} OutputStream;
class CCreateFLVFile {
public:
	CCreateFLVFile(void);
	~CCreateFLVFile(void);

public:
	BOOL Init(char *pFileName, int cx, int cy, DWORD dwFrameDuration);
	void Destroy();

	int WriteVideoFrame(unsigned char *pBuffer, int nStride);

protected:
	OutputStream m_stVideo;

	AVOutputFormat *m_outFmt;
	AVFormatContext *m_oc;

	int m_cx;
	int m_cy;
	int m_nHaveVideo;
	int m_nFrameDuration;

	int m_src_linesize[4];
	uint8_t *m_src_data[4];

protected:
	void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec,
			enum AVCodecID codec_id);
	void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost,
			AVDictionary *opt_arg);
	void close_stream(AVFormatContext *oc, OutputStream *ost);
	AVFrame *get_video_frame(OutputStream *ost, unsigned char *pBuffer,
				 int nStride);
	int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base,
			AVStream *st, AVPacket *pkt);
	void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
	AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width,
			       int height);
};
