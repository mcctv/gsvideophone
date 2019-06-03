#ifndef GSVIDEODECODER_H
#define GSVIDEODECODER_H
#include <stdio.h>
#include <iostream>

using namespace std;

extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/rational.h"
#include "libavutil/mathematics.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
}

typedef void (*VIDEO_DECODE_CB)(void *puser, int w, int h, char *data);

class GSVideoDecoder
{
public:
    GSVideoDecoder();
    ~GSVideoDecoder();

    int GetWidth();
    int GetHeight();
    int OpenDecode(void *puser, VIDEO_DECODE_CB cb );
    int CheckSws();
    int CloseDecode();
    int DecodeFrame(char *data, int datalen);

private:
    int              m_nWidth;
    int              m_nHeight;
    unsigned char   *m_pRgbBuf;
    AVCodec         *m_pVideoCodec;
    AVCodecContext  *m_pVideoCodecCtx;
    AVFrame         *m_pFrameYuv;
    AVFrame         *m_pFrameRgb;
    SwsContext      *m_pScxt;

	VIDEO_DECODE_CB m_cb;
	void  *m_pUser; 
};

#endif // GSVIDEODECODER_H
