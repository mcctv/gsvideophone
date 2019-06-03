#ifndef GSVIDEOENCODER_H
#define GSVIDEOENCODER_H


#include <stdio.h>
#include <iostream>

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

typedef void (*VIDEO_ENCODE_CB)(void *pUser,  char *data, int len);

class GSVideoEncoder
{
public:
    GSVideoEncoder();
    ~GSVideoEncoder();

    int OpenEncoder(VIDEO_ENCODE_CB cb, void *pUser, int width, int height, int fps, int bitrate, int gop);
    int CloseEncoder();
    int EncodeFrame(char *data);

private:
    AVCodec         *m_pVideoCodec;
    AVCodecContext  *m_pVideoCodecCtx;
    AVFrame         *m_pYuv420spFrame;
    AVFrame         *m_pYuv420pFrame;
    SwsContext      *m_pScxtYuv;
    int              m_nSizeYuv420p;
    uint8_t         *m_pBufYuv420p;
    int              m_nSizeH264;
    uint8_t         *m_pBufH264;

    int64_t          m_nFrameVideo;
    VIDEO_ENCODE_CB  m_cb;
	void            *m_pUser;


};

#endif // GSVIDEOENCODER_H
