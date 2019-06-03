#include "gsvideoencoder.h"

GSVideoEncoder::GSVideoEncoder()
{
    m_pVideoCodec     = NULL;
    m_pVideoCodecCtx  = NULL;
    m_pYuv420spFrame  = NULL;
    m_pYuv420pFrame   = NULL;
    m_pScxtYuv        = NULL;
    m_nFrameVideo     = 0;
    m_cb = NULL;
	m_pUser = NULL;
}

GSVideoEncoder::~GSVideoEncoder()
{

}

int GSVideoEncoder::OpenEncoder(VIDEO_ENCODE_CB cb, void *pUser, int width, int height, int fps, int bitrate, int gop)
{
    m_cb = cb;
	m_pUser = pUser;
    m_pVideoCodec     = NULL;
    m_pVideoCodecCtx  = NULL;
    m_pYuv420spFrame  = NULL;
    m_pYuv420pFrame   = NULL;
    m_pScxtYuv        = NULL;
    m_nFrameVideo     = 0;

    av_register_all();
    avcodec_register_all();

    m_pVideoCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if(m_pVideoCodec == NULL)
    {
        return -1;
    }

    m_pVideoCodecCtx = avcodec_alloc_context3(m_pVideoCodec);
    if(m_pVideoCodecCtx == NULL)
    {
        return -1;
    }

    avcodec_get_context_defaults3( m_pVideoCodecCtx, m_pVideoCodec);
    m_pVideoCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    m_pVideoCodecCtx->codec_id = AV_CODEC_ID_H264;
    m_pVideoCodecCtx->bit_rate = bitrate;
    m_pVideoCodecCtx->width  = width;
    m_pVideoCodecCtx->height = height;

    AVRational rational ;
    rational.num = 1;
    rational.den = fps;
    m_pVideoCodecCtx->time_base = rational;
    m_pVideoCodecCtx->gop_size = gop;
    m_pVideoCodecCtx->max_b_frames = 0;
    m_pVideoCodecCtx->pix_fmt =PIX_FMT_YUV420P;
    m_pVideoCodecCtx->ticks_per_frame=1;

    //av_opt_set(m_pVideoCodecCtx->priv_data, "preset", "superfast", 0);
    av_opt_set(m_pVideoCodecCtx->priv_data, "preset", "fast", 0);
   // av_opt_set(m_pVideoCodecCtx->priv_data, "profile", "baseline", 0);

    if(avcodec_open2(m_pVideoCodecCtx, m_pVideoCodec, NULL) < 0)
    {
        return -1;
    }

    m_pYuv420spFrame = avcodec_alloc_frame();
    m_pYuv420pFrame = avcodec_alloc_frame();

    m_pScxtYuv = sws_getContext(m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, PIX_FMT_NV21,
                                m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, PIX_FMT_YUV420P,
                                SWS_POINT,NULL,NULL,NULL);
    if(m_pScxtYuv == NULL)
    {
        return -1;
    }
     m_nSizeYuv420p      = width*height*3/2;
     m_pBufYuv420p       = new uint8_t[m_nSizeYuv420p];

     m_nSizeH264   = width*height*3/2;
     m_pBufH264    = new uint8_t[m_nSizeH264];

    
    return 0;
}

int GSVideoEncoder::CloseEncoder()
{
    if(m_pVideoCodecCtx != NULL)
    {
        avcodec_close(m_pVideoCodecCtx);
        av_free(m_pVideoCodecCtx);
        m_pVideoCodecCtx = NULL;
    }

    if(m_pYuv420spFrame != NULL)
    {
        avcodec_free_frame(&m_pYuv420spFrame);
        m_pYuv420spFrame = NULL;
    }
    if(m_pYuv420pFrame != NULL)
    {
        avcodec_free_frame(&m_pYuv420pFrame);
        m_pYuv420pFrame = NULL;
    }
    if(m_pBufYuv420p != NULL)
    {
        delete m_pBufYuv420p;
        m_pBufYuv420p = NULL;
    }
    if(m_pBufH264 != NULL)
    {
        delete m_pBufH264;
        m_pBufH264 = NULL;
    }

    if(m_pScxtYuv != NULL)
    {
        sws_freeContext(m_pScxtYuv);
        m_pScxtYuv = NULL;
    }

    m_nFrameVideo = 0;
    return 0;
}

int GSVideoEncoder::EncodeFrame(char *data)
{
    int iret = 0;
    int picture = 0;
    avpicture_fill((AVPicture*)m_pYuv420spFrame,(unsigned char*)data,PIX_FMT_NV21,m_pVideoCodecCtx->width,m_pVideoCodecCtx->height);
    avpicture_fill((AVPicture*)m_pYuv420pFrame, m_pBufYuv420p ,PIX_FMT_YUV420P,m_pVideoCodecCtx->width,m_pVideoCodecCtx->height);
    sws_scale(m_pScxtYuv, m_pYuv420spFrame->data, m_pYuv420spFrame->linesize,0,m_pVideoCodecCtx->height,m_pYuv420pFrame->data,m_pYuv420pFrame->linesize);

    AVPacket  avpkt;
    av_init_packet(&avpkt);
    avpkt.data = m_pBufH264;
    avpkt.size = m_nSizeH264;

    m_nFrameVideo++;
    m_pYuv420pFrame->pts =m_nFrameVideo;

    avcodec_encode_video2(m_pVideoCodecCtx, &avpkt, m_pYuv420pFrame, &picture);
    if (picture)
    {
        if(m_cb != NULL)
        {
            m_cb( m_pUser ,(char*)avpkt.data, avpkt.size);
        }
    }
    av_free_packet(&avpkt);

    return iret;
}
