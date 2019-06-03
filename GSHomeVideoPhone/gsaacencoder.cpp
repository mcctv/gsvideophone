#include "gsaacencoder.h"

GSAACEncoder::GSAACEncoder()
{

    m_pAudioC     = NULL;
    m_pAudioCtx   = NULL;
    m_pAudioFrame = NULL;
    m_pPcmBuffer  = NULL;
    m_pAacBuffer  = NULL;
    m_cbAac       = NULL;
	m_pUser       = NULL;

    m_nPcmBufferSize = 0;
    m_nPcmPos = 0;

}

GSAACEncoder::~GSAACEncoder()
{

}

int GSAACEncoder::OpenAACEncoder(void *puser, AUDIO_ENCODE_CB cb)
{
    m_cbAac = cb;
	m_pUser = puser;
    av_register_all();
    avcodec_register_all();

    m_pAudioC = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if(m_pAudioC == NULL)
    {
        return -1;
    }

    m_pAudioCtx = avcodec_alloc_context3(m_pAudioC);
    if(m_pAudioCtx == NULL)
    {
        return -1;
    }

    avcodec_get_context_defaults3( m_pAudioCtx, m_pAudioC);
    m_pAudioCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    m_pAudioCtx->codec_id = AV_CODEC_ID_AAC;
    m_pAudioCtx->bit_rate = 32000;
    m_pAudioCtx->sample_rate = 44100;
    m_pAudioCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    m_pAudioCtx->channels = 1;

    if (avcodec_open2(m_pAudioCtx, m_pAudioC, NULL) < 0)
    {
        return -1;
    }

    m_pAudioFrame =  avcodec_alloc_frame();
    m_pAudioFrame->nb_samples     = m_pAudioCtx->frame_size;
    m_pAudioFrame->format         = m_pAudioCtx->sample_fmt;
    m_pAudioFrame->channel_layout = m_pAudioCtx->channel_layout;

    m_nPcmBufferSize = av_samples_get_buffer_size(NULL, m_pAudioCtx->channels, m_pAudioCtx->frame_size,
                                                  m_pAudioCtx->sample_fmt, 0);
    m_pPcmBuffer = new unsigned char[m_nPcmBufferSize];
    if (!m_pPcmBuffer)
    {
        return -1;
    }
    m_pAacBuffer = new unsigned char[m_nPcmBufferSize];
    int ret = avcodec_fill_audio_frame(m_pAudioFrame, m_pAudioCtx->channels, m_pAudioCtx->sample_fmt,
                                       (const uint8_t*)m_pPcmBuffer, m_nPcmBufferSize, 0);
    if (ret < 0)
    {
        return -1;
    }


    return 0;

}

int GSAACEncoder::CloseAACEncoder()
{

    if(m_pAudioCtx != NULL)
    {
        avcodec_close(m_pAudioCtx);
        av_free(m_pAudioCtx);
        m_pAudioCtx = NULL;
    }

    if(m_pAudioFrame != NULL)
    {
        avcodec_free_frame(&m_pAudioFrame);
        m_pAudioFrame = NULL;
    }

    if(m_pPcmBuffer != NULL)
    {
        delete [] m_pPcmBuffer;
        m_pPcmBuffer = NULL;
    }
    if(m_pAacBuffer != NULL)
    {
        delete [] m_pAacBuffer;
        m_pAacBuffer = NULL;
    }


	

    return 0;
}

int GSAACEncoder::AACEncode(char *in, int inlen)
{
    int iRet = 0;
    int pos = 0;
    while( pos+m_nPcmBufferSize-m_nPcmPos < inlen)
    {
        memcpy(m_pPcmBuffer+m_nPcmPos,in+pos, m_nPcmBufferSize-m_nPcmPos);
        pos += m_nPcmBufferSize-m_nPcmPos;
        m_nPcmPos = 0;

        iRet = AACEncodeFrame();
        if(iRet < 0)
        {
            return -1;
        }
    }
    if(pos < inlen)
    {
        memcpy(m_pPcmBuffer+m_nPcmPos, in+pos,inlen-pos);
        m_nPcmPos += inlen-pos;
    }

    return 0;
}

int  GSAACEncoder::AACEncodeFrame()
{

    AVPacket pkt;
    int iRet = 0;
    int iGot = 0;

    av_init_packet(&pkt);
    pkt.data = m_pAacBuffer;
    pkt.size = m_nPcmBufferSize;
    iRet = avcodec_encode_audio2(m_pAudioCtx, &pkt, m_pAudioFrame, &iGot);
    if (iRet < 0)
    {
       return -1;
    }
    if (iGot)
    {
       if(m_cbAac != NULL)
       {
           m_cbAac(m_pUser , (char*)pkt.data, pkt.size);

       }
    }
    av_free_packet(&pkt);

    return 0;
}
