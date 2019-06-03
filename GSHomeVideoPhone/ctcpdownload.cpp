#include "ctcpdownload.h"
#include <time.h>

void* DownLoadThread(void* p)
{
	CTcpDownload *thiz = (CTcpDownload*)p;
	thiz->Download();
	return 0;
}


CTcpDownload::CTcpDownload():
    m_nUserID(-1),
    m_nSock(-1),
    m_cb(NULL),
    m_bcancle(true)
{
    m_pBufSend = new char[32];
    m_pBufRecv = new char[2048];
    m_pbuf264  = new char[1024*1024];
	m_pUser = NULL;
    
    m_thDownload = NULL;
	
	  
}

CTcpDownload::~CTcpDownload()
{
    if(m_pBufSend != NULL)
    {
        delete [] m_pBufSend;
        m_pBufSend = NULL;
    }
    if(m_pBufRecv != NULL)
    {
        delete [] m_pBufRecv;
        m_pBufRecv = NULL;
    }
    if(m_pbuf264 != NULL)
    {
        delete [] m_pbuf264;
        m_pbuf264 = NULL;
    }
}

int CTcpDownload::OpenDownload(void *puser, int uerid, TCP_DOWNLOAD_CB cb, const char *ip, int port)
{
	 m_pUser = puser;
     m_nUserID = uerid;
     m_cb     = cb;
     m_bcancle = false;

    *(unsigned short*)(m_pBufSend) = 11;
    *(int*)(m_pBufSend+2) = uerid;
    *(m_pBufSend+6) = 0x00;
    *(int*)(m_pBufSend+7) = 0x0010 ;

	
     m_nSock = socket(AF_INET, SOCK_STREAM, 0);
     if(m_nSock < 0)
     {
         return -1;
     }

     memset(&m_addr, 0, sizeof(m_addr));
	 m_addr.sin_family = AF_INET;
	 m_addr.sin_port = htons(port);
	 m_addr.sin_addr.s_addr = inet_addr(ip);
    
     pthread_create(&m_thDownload, NULL,&DownLoadThread, this);


     return 0;
}

int CTcpDownload::CloseDownload()
{
    m_bcancle = true;
    if(m_nSock != -1)
    {
        close(m_nSock);
        m_nSock = -1;
    }
    if(m_thDownload != NULL)
    {
        pthread_cancel(m_thDownload);
        pthread_join(m_thDownload, NULL);
        m_thDownload = NULL;
    }
	
    return 0;
}

int CTcpDownload::Download()
{
    
    int iret = 0;
    time_t tmlast = 0;
    
    int iRet = connect(m_nSock, (sockaddr*)&m_addr, sizeof(m_addr));
    if(iRet < 0)
    {
        close(m_nSock);
        m_nSock = -1;
        return -1;
    }

    if(send(m_nSock, m_pBufSend, 11, 0) < 0)
    {
        //return -1;
    }
    
    int  framesize = 0;
    
    while(!m_bcancle)
    {
        iret = Recvn(m_pBufRecv, 2);
        if(iret < 0)
        {
            return -1;   //something error
        }
        unsigned short  len = *(unsigned short*)m_pBufRecv;
        if(len > 1500)
        {
            return -1;  //something error
        }
        iret = Recvn(m_pBufRecv+2, len-2);
        if(iret < 0)
        {
            return -1;   //something error
        }
        
        if(m_pBufRecv[6] == (char)0xf1 )
        {
            static unsigned short lastindex = 0;
            unsigned short index = *(unsigned short*)(m_pBufRecv+7);
            if(index-lastindex != 1)
            {
                //	  lost package
            }
            lastindex = index;
            
            
            if(m_pBufRecv[9] ==  (char)0x01)
            {
                framesize = 0;
                memcpy(m_pbuf264+framesize,m_pBufRecv+10, len-10 );
                framesize +=  len-10;
                
            }
            else if(m_pBufRecv[9] ==  (char)0x03)
            {
                memcpy(m_pbuf264+framesize,m_pBufRecv+10, len-10 );
                framesize +=  len-10;
                
                if(m_cb != NULL)
                {
                    m_cb(m_pUser,m_pbuf264, framesize);
                }
                framesize = 0;
            }
            else
            {
                if(framesize > 1024*1024)
                {
                    framesize = 0;
                }
                memcpy(m_pbuf264+framesize,m_pBufRecv+10, len-10 );
                framesize +=  len-10;
            }
        }
        
        time_t tm ;
        time(&tm);
        if(tm-tmlast > 3)
        {
            if(send(m_nSock, m_pBufSend, 11, 0) < 0)
            {
               // return -1;
            }
            tmlast = tm;
        }
    }
    
    return 0;
}
int CTcpDownload::Recvn(char* buf, int size)
{
    int len = 0;
    int ret = 0;
    
    while(len < size)
    {
        ret = recv(m_nSock, buf+len , size-len ,0 );
        if (ret ==-1)
        {
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        if(ret == 0)
        {
            return -1;
        }
        len += ret;
    }
    return len;
}
