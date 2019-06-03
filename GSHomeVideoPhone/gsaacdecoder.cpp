#include "gsaacdecoder.h"

GSAACDecoder::GSAACDecoder()
{
    m_PcmCB       = NULL;
    m_pAudioC     = NULL;
    m_pAudioCtx   = NULL;
    m_pAudioFrame = NULL;
	m_pUser       = NULL;

}

GSAACDecoder::~GSAACDecoder()
{



}

int GSAACDecoder::OpenAACDecoder(void *puser, AUDIO_DECODE_CB cb)
{
	m_pUser = puser;
    m_PcmCB = cb;
    av_register_all();
    avcodec_register_all();

    m_pAudioC = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if(m_pAudioC == NULL)
    {
        return -1;
    }
    m_pAudioCtx = avcodec_alloc_context3(m_pAudioC);
    if(m_pAudioCtx == NULL)
    {
        return -1;
    }
    int iRet  = avcodec_open2(m_pAudioCtx, m_pAudioC, NULL);
    if (iRet  < 0)
    {
        return -1;
    }

    m_pAudioFrame = avcodec_alloc_frame();
    if(m_pAudioFrame == NULL)
    {
        return -1;
    }
    return 0;
}

int GSAACDecoder::CloseAACDecoder()
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
    return 0;
}

int GSAACDecoder::AACDecode(char *in, int inlen)
{
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = (unsigned char*)in;
    avpkt.size = inlen;
    int iGot = 0;
    int iDataSize = 0;
    int len = 0;

    avcodec_get_frame_defaults(m_pAudioFrame);

    len = avcodec_decode_audio4(m_pAudioCtx, m_pAudioFrame, &iGot, &avpkt);
    if (len < 0)
    {
        return -1;
    }
    if (iGot > 0)
    {

        iDataSize= av_samples_get_buffer_size(NULL, m_pAudioCtx->channels,
                                                           m_pAudioFrame->nb_samples,
                                                           m_pAudioCtx->sample_fmt, 1);
        if(m_PcmCB != NULL)
        {
            m_PcmCB(m_pUser, (char*)m_pAudioFrame->data[0],iDataSize);
        }

    }

    av_free_packet(&avpkt);

    return 0;
}
