
#ifndef GSHOMEMANAGER_H_
#define GSHOMEMANAGER_H_

#include "ctcpdownload.h"
#include "ctcpupload.h"
#include "gsaacdecoder.h"
#include "gsaacencoder.h"
#include "gsvideodecoder.h"
#include "gsvideoencoder.h"


class GSHomeManager
{
public:
	GSHomeManager(void);
	~GSHomeManager(void);

	//
	friend void download_video_cb(void *puser, char* data, int len);
	friend void decode_video_cb(void *puser, int w, int h,  char* data);

	friend void download_audio_cb(void *puser, char* data, int len);
	friend void decode_audio_cb(void *puser,char* data, int len);
	
	friend void encode_audio_cb(void *puser,char* data, int len);
	friend void encode_video_cb(void *puser,char* data, int len);


	int OpenVideoDecoder(void *puser, VIDEO_DECODE_CB cb, int userid, const char *ip, int port);
	int CloseVideoDecoder();

	int OpenAudioDecoder(void *puser, AUDIO_DECODE_CB cb, int userid, const char *ip, int port);
	int CloseAudioDecoder();

	int OpenAudioEncoder( int userid, const  char *ip, int port);
	int CloseAudioEncoder();
	int AduioEncoder(char *data, int len);

	int OpenVideoEncoder( int userid, const  char *ip, int port);
	int CloseVideoEncoder();
	int VideoEncoder(char *data, int len);

private:
	CTcpDownload   *m_downloadVideo;
	GSVideoDecoder *m_decoderVideo;
	VIDEO_DECODE_CB m_videoDecoderCB;
	void           *m_videoDecoderUser;

	CTcpDownload   *m_downloadAudio;
	GSAACDecoder   *m_decoderAudio;
	AUDIO_DECODE_CB m_audioDecoderCB;
	void           *m_audioDecoderUser;

	GSAACEncoder   *m_encoderAudio;
	CTcpUpload     *m_audioUpload;

	GSVideoEncoder *m_encoderVideo;
	CTcpUpload     *m_videoUpload;

	

};

#endif

