
#include "GSHomeManager.h"

void download_video_cb(void *puser, char* data, int len)
{
	//download 
	
	GSHomeManager *thiz = (GSHomeManager*)puser;
	if(thiz->m_decoderVideo != NULL)
	{
		thiz->m_decoderVideo->DecodeFrame(data, len);
	}	
	
}

void decode_video_cb(void *puser, int w, int h,  char* data)
{
	
	GSHomeManager *thiz = (GSHomeManager*)puser;
	if (thiz->m_videoDecoderCB != NULL)
	{
		thiz->m_videoDecoderCB(thiz->m_videoDecoderUser, w, h, data);
	}
	

}

void download_audio_cb(void *puser, char* data, int len)
{
	GSHomeManager *thiz = (GSHomeManager*)puser;
	if(thiz->m_decoderAudio != NULL)
	{
		thiz->m_decoderAudio->AACDecode(data,len);
	}	

}
void decode_audio_cb(void *puser,char* data, int len)
{
	GSHomeManager *thiz = (GSHomeManager*)puser;
	if (thiz->m_audioDecoderCB != NULL)
	{
		thiz->m_audioDecoderCB(thiz->m_audioDecoderUser, data,len);
	}
}

void encode_audio_cb(void *puser,char* data, int len)
{
	GSHomeManager *thiz = (GSHomeManager*)puser;
	//upload
	if(thiz->m_audioUpload != NULL)
	{
		thiz->m_audioUpload->Upload(data, len);
	}
}

void encode_video_cb(void *puser,char* data, int len)
{
	GSHomeManager *thiz = (GSHomeManager*)puser;
	//upload
	if(thiz->m_videoUpload != NULL)
	{
		thiz->m_videoUpload->Upload(data, len);
	}
}

GSHomeManager::GSHomeManager(void)
{
	m_downloadVideo   = NULL;
	m_decoderVideo    = NULL;
	m_videoDecoderCB  = NULL;
	m_videoDecoderUser= NULL;

	m_downloadAudio   = NULL;
	m_decoderAudio    = NULL;
	m_audioDecoderCB  = NULL;
	m_audioDecoderUser= NULL;

	m_encoderAudio     = NULL;
	m_audioUpload      = NULL;

	m_encoderVideo = NULL;
	m_videoUpload = NULL;

}
GSHomeManager::~GSHomeManager(void)
{
}

int GSHomeManager::OpenVideoDecoder(void *puser, VIDEO_DECODE_CB cb, int userid, const  char *ip, int port)
{
	m_videoDecoderCB   = cb;
	m_videoDecoderUser = puser;
	int iret = 0;

	if(m_decoderVideo != NULL)
	{
		m_decoderVideo->CloseDecode();
		delete m_decoderVideo;
		m_decoderVideo = NULL;
	}
	m_decoderVideo = new GSVideoDecoder;
	iret = m_decoderVideo->OpenDecode(this, &decode_video_cb);
	if(iret < 0)
	{
		return -1;
	}
	if(m_downloadVideo != NULL)
	{
		m_downloadVideo->CloseDownload();
		delete m_downloadVideo;
		m_downloadVideo = NULL;
	}
	m_downloadVideo = new CTcpDownload;
	iret = m_downloadVideo->OpenDownload(this, userid, &download_video_cb, ip, port);
	if(iret < 0)
	{
		return -1;
	}

	return 0;
}
int GSHomeManager::CloseVideoDecoder()
{
	if(m_downloadVideo != NULL)
	{
		m_downloadVideo->CloseDownload();
		delete m_downloadVideo;
		m_downloadVideo = NULL;
	}

	if(m_decoderVideo != NULL)
	{
		m_decoderVideo->CloseDecode();
		delete m_decoderVideo;
		m_decoderVideo = NULL;
	}

	return 0;
}

int GSHomeManager::OpenAudioDecoder(void *puser, AUDIO_DECODE_CB cb, int userid, const  char *ip, int port)
{
	m_audioDecoderCB   = cb;
	m_audioDecoderUser = puser;
	int iret = 0;

	if(m_decoderAudio != NULL)
	{
		m_decoderAudio->CloseAACDecoder();
		delete m_decoderAudio;
		m_decoderAudio = NULL;
	}
	m_decoderAudio = new GSAACDecoder;
	iret = m_decoderAudio->OpenAACDecoder(this, &decode_audio_cb);
	if(iret < 0)
	{
		return -1;
	}
	if(m_downloadAudio != NULL)
	{
		m_downloadAudio->CloseDownload();
		delete m_downloadAudio;
		m_downloadAudio = NULL;
	}
	m_downloadAudio = new CTcpDownload;
	iret = m_downloadAudio->OpenDownload(this, userid, &download_audio_cb, ip, port);
	if(iret < 0)
	{
		return -1;
	}
	return 0;
}
int GSHomeManager::CloseAudioDecoder()
{

	if(m_downloadAudio != NULL)
	{
		m_downloadAudio->CloseDownload();
		delete m_downloadAudio;
		m_downloadAudio = NULL;
	}

	if(m_decoderAudio != NULL)
	{
		m_decoderAudio->CloseAACDecoder();
		delete m_decoderAudio;
		m_decoderAudio = NULL;
	}

	return 0;
}


int GSHomeManager::OpenAudioEncoder(int userid, const  char *ip, int port)
{
	int iret = 0;
	//upload

	if(m_audioUpload != NULL)
	{
		m_audioUpload->CloseUpload();
		m_audioUpload = NULL;
	}
	m_audioUpload = new CTcpUpload;
	iret = m_audioUpload->OpenUpload(userid,ip,port);
	if(iret < 0)
	{
		return -1;
	}

	if(m_encoderAudio != NULL)
	{
		m_encoderAudio->CloseAACEncoder();
		delete m_encoderAudio;
		m_encoderAudio = NULL;
	}
	m_encoderAudio = new GSAACEncoder;
	iret = m_encoderAudio->OpenAACEncoder(this,encode_audio_cb );
	if(iret < 0)
	{
		return -1;
	}
	
	return 0;
}
int  GSHomeManager::CloseAudioEncoder()
{
	if(m_encoderAudio != NULL)
	{
		m_encoderAudio->CloseAACEncoder();
		delete m_encoderAudio;
		m_encoderAudio = NULL;
	}

	if(m_audioUpload != NULL)
	{
		m_audioUpload->CloseUpload();
		m_audioUpload = NULL;
	}
	return 0;
}
int  GSHomeManager::AduioEncoder(char *data, int len)
{
	if(m_encoderAudio != NULL)
	{
		m_encoderAudio->AACEncode(data, len);
	}

	return 0;
}

int GSHomeManager::OpenVideoEncoder( int userid, const  char *ip, int port)
{

	int iret = 0;
	//upload

	if(m_videoUpload != NULL)
	{
		m_videoUpload->CloseUpload();
		m_videoUpload = NULL;
	}
	m_videoUpload = new CTcpUpload;
	iret = m_videoUpload->OpenUpload(userid,ip,port);
	if(iret < 0)
	{
		return -1;
	}

	if(m_encoderVideo != NULL)
	{
		m_encoderVideo->CloseEncoder();
		delete m_encoderVideo;
		m_encoderVideo = NULL;
	}
	m_encoderVideo = new GSVideoEncoder;
	iret = m_encoderVideo->OpenEncoder(&encode_video_cb,this,320,240,15 ,300000,15);
	if(iret < 0)
	{
		return -1;
	}

	return 0;
}
int GSHomeManager::CloseVideoEncoder()
{
	if(m_encoderVideo != NULL)
	{
		m_encoderVideo->CloseEncoder();
		delete m_encoderVideo;
		m_encoderVideo = NULL;
	}
	if(m_videoUpload != NULL)
	{
		m_videoUpload->CloseUpload();
		m_videoUpload = NULL;
	}
	return 0;
}
int GSHomeManager::VideoEncoder(char *data, int len)
{
	if(m_encoderVideo != NULL)
	{
		m_encoderVideo->EncodeFrame(data);
	}
	return 0;
}

