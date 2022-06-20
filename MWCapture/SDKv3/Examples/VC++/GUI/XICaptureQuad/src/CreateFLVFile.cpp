#include "stdafx.h"
#include "CreateFLVFile.h"

CCreateFLVFile::CCreateFLVFile(void)
{
	m_outFmt = NULL;
	m_oc = NULL;
	m_nHaveVideo = 0;
	m_cx = 0;
	m_cy = 0;
	m_nFrameDuration = 0;
	memset(&m_stVideo, 0, sizeof(OutputStream));
}

CCreateFLVFile::~CCreateFLVFile(void) {}

inline void PrintErrorInfo(char *pStr, int ret)
{
	char chError[AV_ERROR_MAX_STRING_SIZE] = {0};
	char *pError =
		av_make_error_string(chError, AV_ERROR_MAX_STRING_SIZE, ret);
	fprintf(stderr, "%s: %s\n", pStr, pError);
}

BOOL CCreateFLVFile::Init(char *pFileName, int cx, int cy,
			  DWORD dwFrameDuration)
{
	const char *filename;
	AVCodec *video_codec;
	int ret;
	int encode_video = 0;
	AVDictionary *opt = NULL;
	int i;

	m_cx = cx;
	m_cy = cy;

	switch (dwFrameDuration) {
	case 400000: {
		m_nFrameDuration = 25;
		break;
	}
	case 333667: {
		m_nFrameDuration = 29;
		break;
	}
	case 333333: {
		m_nFrameDuration = 30;
		break;
	}
	case 166833: {
		m_nFrameDuration = 59;
		break;
	}
	case 166667: {
		m_nFrameDuration = 60;
		break;
	}
	default:
		break;
	};

	av_register_all();

	filename = pFileName;

	ret = av_dict_set(&opt, "preset", "veryfast", 0);

	avformat_alloc_output_context2(&m_oc, NULL, NULL, filename);
	if (!m_oc) {
		printf("Could not deduce output format from file extension: using MPEG.\n");
		avformat_alloc_output_context2(&m_oc, NULL, "mpeg", filename);
	}
	if (!m_oc)
		return 1;

	if ((ret = av_image_alloc(m_src_data, m_src_linesize, m_cx, m_cy,
				  CAPTURE_PIX_FMT, 16)) < 0) {
		fprintf(stderr, "Could not allocate source image\n");
	}

	m_outFmt = m_oc->oformat;

	if (m_outFmt->video_codec != AV_CODEC_ID_NONE) {
		add_stream(&m_stVideo, m_oc, &video_codec,
			   m_outFmt->video_codec);
		m_nHaveVideo = 1;
		encode_video = 1;
	}

	if (m_nHaveVideo)
		open_video(m_oc, video_codec, &m_stVideo, opt);

	av_dump_format(m_oc, 0, filename, 1);

	ret = avio_open(&m_oc->pb, filename, AVIO_FLAG_WRITE);
	if (ret < 0) {
		char chError[AV_ERROR_MAX_STRING_SIZE] = {0};
		char *pError = av_make_error_string(
			chError, AV_ERROR_MAX_STRING_SIZE, ret);
		fprintf(stderr, "Could not open '%s': %s\n", filename, pError);
		return 1;
	}

	ret = avformat_write_header(m_oc, &opt);
	if (ret < 0) {
		PrintErrorInfo("Error occurred when opening output file", ret);
		return 1;
	}
}

void CCreateFLVFile::Destroy()
{
	av_write_trailer(m_oc);

	int aa = m_oc->duration;

	close_stream(m_oc, &m_stVideo);

	avio_closep(&m_oc->pb);

	/* free the stream */
	avformat_free_context(m_oc);

	m_outFmt = NULL;
	m_oc = NULL;
	m_nHaveVideo = 0;
	m_cx = 0;
	m_cy = 0;
	memset(&m_stVideo, 0, sizeof(OutputStream));
}

int CCreateFLVFile::WriteVideoFrame(
	unsigned char *pBuffer,
	int nStride /*, AVFormatContext *oc, OutputStream *ost*/)
{
	int ret;
	AVCodecContext *c;
	AVFrame *frame;
	int got_packet = 0;
	AVPacket pkt = {0};

	c = m_stVideo.enc;

	frame = get_video_frame(&m_stVideo, pBuffer, nStride);

	/* a hack to avoid data copy with some raw video muxers */
	av_init_packet(&pkt);

	/* encode the image */
	ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
	if (ret < 0) {
		PrintErrorInfo("Error encoding video frame", ret);
		exit(1);
	}

	if (got_packet) {
		ret = write_frame(m_oc, &c->time_base, m_stVideo.st, &pkt);
	} else {
		ret = 0;
	}

	if (ret < 0) {
		PrintErrorInfo("Error while writing video frame", ret);
		exit(1);
	}

	return (frame || got_packet) ? 0 : 1;
}

/* Add an output stream. */
void CCreateFLVFile::add_stream(OutputStream *ost, AVFormatContext *oc,
				AVCodec **codec, enum AVCodecID codec_id)
{
	AVCodecContext *c;
	int i;

	/* find the encoder */
	*codec = avcodec_find_encoder(codec_id);
	if (!(*codec)) {
		fprintf(stderr, "Could not find encoder for '%s'\n",
			avcodec_get_name(codec_id));
		exit(1);
	}

	ost->st = avformat_new_stream(oc, NULL);
	if (!ost->st) {
		fprintf(stderr, "Could not allocate stream\n");
		exit(1);
	}
	ost->st->id = oc->nb_streams - 1;
	c = avcodec_alloc_context3(*codec);
	if (!c) {
		fprintf(stderr, "Could not alloc an encoding context\n");
		exit(1);
	}
	ost->enc = c;

	switch ((*codec)->type) {
	case AVMEDIA_TYPE_AUDIO: {
		c->sample_fmt = (*codec)->sample_fmts ? (*codec)->sample_fmts[0]
						      : AV_SAMPLE_FMT_FLTP;
		c->bit_rate = 64000;
		c->sample_rate = 44100;
		if ((*codec)->supported_samplerates) {
			c->sample_rate = (*codec)->supported_samplerates[0];
			for (i = 0; (*codec)->supported_samplerates[i]; i++) {
				if ((*codec)->supported_samplerates[i] == 44100)
					c->sample_rate = 44100;
			}
		}
		c->channels =
			av_get_channel_layout_nb_channels(c->channel_layout);
		c->channel_layout = AV_CH_LAYOUT_STEREO;
		if ((*codec)->channel_layouts) {
			c->channel_layout = (*codec)->channel_layouts[0];
			for (i = 0; (*codec)->channel_layouts[i]; i++) {
				if ((*codec)->channel_layouts[i] ==
				    AV_CH_LAYOUT_STEREO)
					c->channel_layout = AV_CH_LAYOUT_STEREO;
			}
		}
		c->channels =
			av_get_channel_layout_nb_channels(c->channel_layout);
		AVRational base = {1, c->sample_rate};
		ost->st->time_base = base;
	} break;

	case AVMEDIA_TYPE_VIDEO: {
		c->codec_id = codec_id;
		c->bit_rate = 400000;
		/* Resolution must be a multiple of two. */
		c->width = m_cx;
		c->height = m_cy;
		//c->thread_count = 8;
		/* timebase: This is the fundamental unit of time (in seconds) in terms
			* of which frame timestamps are represented. For fixed-fps content,
			* timebase should be 1/framerate and timestamp increments should be
			* identical to 1. */
		AVRational base = {1, m_nFrameDuration};
		ost->st->time_base = base;
		c->time_base = ost->st->time_base;

		c->gop_size =
			12; /* emit one intra frame every twelve frames at most */
		c->pix_fmt = STREAM_PIX_FMT;
		if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
			/* just for testing, we also add B frames */
			c->max_b_frames = 2;
		}
		if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
			/* Needed to avoid using macroblocks in which some coeffs overflow.
				* This does not happen with normal video, it just happens here as
				* the motion of the chroma plane does not match the luma plane. */
			c->mb_decision = 2;
		}
	} break;

	default:
		break;
	}

	/* Some formats want stream headers to be separate. */
	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void CCreateFLVFile::open_video(AVFormatContext *oc, AVCodec *codec,
				OutputStream *ost, AVDictionary *opt_arg)
{
	int ret;
	AVCodecContext *c = ost->enc;
	AVDictionary *opt = NULL;

	av_dict_copy(&opt, opt_arg, 0);

	/* open the codec */
	ret = avcodec_open2(c, codec, &opt);
	av_dict_free(&opt);
	if (ret < 0) {
		char chError[AV_ERROR_MAX_STRING_SIZE] = {0};
		char *pError = av_make_error_string(
			chError, AV_ERROR_MAX_STRING_SIZE, ret);
		fprintf(stderr, "Could not open video codec: %s\n", pError);
		exit(1);
	}

	/* allocate and init a re-usable frame */
	ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!ost->frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		exit(1);
	}

	/* If the output format is not YUV420P, then a temporary YUV420P
	* picture is needed too. It is then converted to the required
	* output format. */
	ost->tmp_frame = NULL;
	if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
		ost->tmp_frame =
			alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
		if (!ost->tmp_frame) {
			fprintf(stderr,
				"Could not allocate temporary picture\n");
			exit(1);
		}
	}
	ret = avcodec_parameters_from_context(ost->st->codecpar, c);
	if (ret < 0) {
		fprintf(stderr, "Could not copy the stream parameters\n");
		exit(1);
	}
}

void CCreateFLVFile::close_stream(AVFormatContext *oc, OutputStream *ost)
{
	avcodec_free_context(&ost->enc);
	av_frame_free(&ost->frame);
	//av_frame_free(&ost->tmp_frame);
	sws_freeContext(ost->sws_ctx);
	//swr_free(&ost->swr_ctx);
}

AVFrame *CCreateFLVFile::get_video_frame(OutputStream *ost,
					 unsigned char *pBuffer, int nStride)
{
	AVCodecContext *c = ost->enc;

	if (av_frame_make_writable(ost->frame) < 0)
		exit(1);

	if (!ost->sws_ctx) {
		ost->sws_ctx = sws_getContext(c->width, c->height,
					      CAPTURE_PIX_FMT, c->width,
					      c->height, c->pix_fmt,
					      SCALE_FLAGS, NULL, NULL, NULL);
		if (!ost->sws_ctx) {
			fprintf(stderr,
				"Could not initialize the conversion context\n");
			exit(1);
		}
	}

	m_src_data[0] = pBuffer;
	sws_scale(ost->sws_ctx, (const uint8_t *const *)m_src_data,
		  m_src_linesize, 0, c->height, ost->frame->data,
		  ost->frame->linesize);

	ost->frame->pts = ost->next_pts++;

	return ost->frame;
}

///* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
// * 'nb_channels' channels. */

int CCreateFLVFile::write_frame(AVFormatContext *fmt_ctx,
				const AVRational *time_base, AVStream *st,
				AVPacket *pkt)
{
	/* rescale output packet timestamp values from codec to stream timebase */
	av_packet_rescale_ts(pkt, *time_base, st->time_base);
	pkt->stream_index = st->index;

	/* Write the compressed frame to the media file. */
	log_packet(fmt_ctx, pkt);
	return av_interleaved_write_frame(fmt_ctx, pkt);
}

void CCreateFLVFile::log_packet(const AVFormatContext *fmt_ctx,
				const AVPacket *pkt)
{
	char strTime[AV_TS_MAX_STRING_SIZE] = {0};
	char strTime1[AV_TS_MAX_STRING_SIZE] = {0};

	AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

	TRACE("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
	      av_ts_make_string(strTime, pkt->pts),
	      av_ts_make_time_string(strTime1, pkt->pts, time_base),
	      av_ts_make_string(strTime, pkt->dts),
	      av_ts_make_time_string(strTime1, pkt->dts, time_base),
	      av_ts_make_string(strTime, pkt->duration),
	      av_ts_make_time_string(strTime1, pkt->duration, time_base),
	      pkt->stream_index);
}

AVFrame *CCreateFLVFile::alloc_picture(enum AVPixelFormat pix_fmt, int width,
				       int height)
{
	AVFrame *picture;
	int ret;

	picture = av_frame_alloc();
	if (!picture)
		return NULL;

	picture->format = pix_fmt;
	picture->width = width;
	picture->height = height;

	/* allocate the buffers for the frame data */
	ret = av_frame_get_buffer(picture, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate frame data.\n");
		exit(1);
	}

	return picture;
}
