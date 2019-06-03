#include "gsvideodecoder.h"

GSVideoDecoder::GSVideoDecoder()
{
    m_nWidth          = 0;
    m_nHeight         = 0;
    m_pVideoCodec     = NULL;
    m_pVideoCodecCtx  = NULL;
    m_pFrameYuv       = NULL;
    m_pFrameRgb       = NULL;
    m_pRgbBuf         = NULL;
    m_pScxt           = NULL;
	m_cb              = NULL;
	m_pUser           = NULL;
}

GSVideoDecoder::~GSVideoDecoder()
{

}

int GSVideoDecoder::GetWidth()
{
    return m_nWidth;
}

int GSVideoDecoder::GetHeight()
{
    return m_nHeight;
}

int GSVideoDecoder::OpenDecode( void *puser, VIDEO_DECODE_CB cb)
{
	m_pUser = puser;
	m_cb = cb;
    avcodec_register_all();
    m_pFrameYuv     =  avcodec_alloc_frame();
    m_pVideoCodecCtx= NULL;
    m_pVideoCodec   = NULL;
    m_pVideoCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!m_pVideoCodec)
    {
        return -1;
    }
    m_pVideoCodecCtx = avcodec_alloc_context3(m_pVideoCodec);

    if(m_pVideoCodec->capabilities&CODEC_CAP_TRUNCATED)
    {
        m_pVideoCodecCtx->flags|= CODEC_FLAG_TRUNCATED;
    }
    if (avcodec_open2(m_pVideoCodecCtx, m_pVideoCodec, NULL) < 0)
    {
        return -1;
    }
    return 0;
}

int GSVideoDecoder::CheckSws()
{
    if(m_pVideoCodecCtx->width == 0 ||m_pVideoCodecCtx->height == 0 || m_pVideoCodecCtx->pix_fmt == PIX_FMT_NONE)
    {
        return -1;
    }
    m_nWidth = m_pVideoCodecCtx->width;
    m_nHeight = m_pVideoCodecCtx->height;

    if(m_pFrameRgb == NULL)
    {
        m_pFrameRgb = avcodec_alloc_frame();
        if (m_pFrameRgb == NULL)
        {
            return  -1;
        }
    }

    if(m_pRgbBuf == NULL)
    {
        m_pRgbBuf = new unsigned char[m_nWidth*m_nHeight *3];
        if(m_pRgbBuf == NULL)
        {
            return -1;
        }
    }


    if(m_pScxt == NULL)
    {
        m_pScxt = sws_getContext(m_nWidth,m_nHeight, m_pVideoCodecCtx->pix_fmt,
                                 m_nWidth,m_nHeight, PIX_FMT_RGB24,
                                 SWS_POINT,NULL,NULL,
                                 NULL);
        if(m_pScxt == NULL)
        {
            return -1;
        }
        if(avpicture_fill((AVPicture*)m_pFrameRgb, m_pRgbBuf ,PIX_FMT_RGB24,m_nWidth,m_nHeight) < 0)
        {
            return -1;
        }
    }
    return 0;
}

int GSVideoDecoder::CloseDecode()
{
    if(m_pVideoCodecCtx != NULL)
    {
        avcodec_close(m_pVideoCodecCtx);
        av_free(m_pVideoCodecCtx);
        m_pVideoCodecCtx = NULL;
    }

    if(m_pFrameRgb != NULL)
    {
        avcodec_free_frame(&m_pFrameRgb);
        m_pFrameRgb = NULL;
    }
    if(m_pFrameYuv != NULL)
    {
        avcodec_free_frame(&m_pFrameYuv);
        m_pFrameYuv = NULL;
    }

    if(m_pRgbBuf != NULL)
    {
        delete []  m_pRgbBuf;
        m_pRgbBuf = NULL;
    }

    if(m_pScxt != NULL)
    {
        sws_freeContext(m_pScxt);
        m_pScxt = NULL;
    }
    m_nWidth            = 0;
    m_nHeight           = 0;

    return 0;
}


int GSVideoDecoder::DecodeFrame(char *data, int datalen)
{
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = (unsigned char*)data;
    avpkt.size = datalen;

    int len , got_picture;

    got_picture = 0;
    len = avcodec_decode_video2(m_pVideoCodecCtx, m_pFrameYuv, &got_picture, &avpkt);
    if (len < 0)
    {
        return -1;
    }
    if (got_picture)
    {
        if(CheckSws() < 0)
        {
           return -1;
        }    
        sws_scale( m_pScxt, m_pFrameYuv->data, m_pFrameYuv->linesize,0,m_pVideoCodecCtx->height , m_pFrameRgb->data,m_pFrameRgb->linesize);  
		if(m_cb != NULL)
		{
			m_cb(m_pUser , m_pVideoCodecCtx->width, m_pVideoCodecCtx->height, (char*)m_pRgbBuf);
		}
    }


    return 0;
}
