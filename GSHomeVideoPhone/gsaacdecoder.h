#ifndef GSAACDECODER_H
#define GSAACDECODER_H
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

typedef void (*AUDIO_DECODE_CB)(void *puser,  char* data, int len);

class GSAACDecoder
{
public:
    GSAACDecoder();
    ~GSAACDecoder();
    int OpenAACDecoder(void *puser, AUDIO_DECODE_CB cb);
    int CloseAACDecoder();
    int AACDecode(char *in, int inlen);

private:
    AUDIO_DECODE_CB  m_PcmCB;
	void            *m_pUser;
    AVCodec         *m_pAudioC;
    AVCodecContext  *m_pAudioCtx;
    AVFrame         *m_pAudioFrame;



};

#endif // GSAACDECODER_H
