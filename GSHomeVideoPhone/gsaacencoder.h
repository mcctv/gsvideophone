#ifndef GSAACENCODER_H
#define GSAACENCODER_H

#include <iostream>
#include <stdio.h>
#include <string.h>

extern "C"
{
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/avutil.h"
    #include "libavutil/rational.h"
    #include "libavutil/mathematics.h"
    #include "libswscale/swscale.h"
    #include "libavutil/opt.h"
}

using namespace std;

typedef void (*AUDIO_ENCODE_CB)(void *puser,  char* data, int len);

class GSAACEncoder
{

public:
    GSAACEncoder();
    ~GSAACEncoder();
    int OpenAACEncoder(void *puser, AUDIO_ENCODE_CB cb);
    int CloseAACEncoder();
    int AACEncode(char *in, int inlen);
    int AACEncodeFrame();

private:
    AVCodec         *m_pAudioC;
    AVCodecContext  *m_pAudioCtx;
    AVFrame         *m_pAudioFrame;
    unsigned char   *m_pPcmBuffer;
    unsigned char   *m_pAacBuffer;
    AUDIO_ENCODE_CB  m_cbAac;
	void            *m_pUser;
    int              m_nPcmBufferSize;
    int              m_nPcmPos;


};

#endif // GSAACENCODER_H
