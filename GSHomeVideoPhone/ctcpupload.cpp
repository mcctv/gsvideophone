#include "ctcpupload.h"

CTcpUpload::CTcpUpload():
m_userID(-1),
m_socket(-1)
{
    m_pSenbuf = new char[1500];
}

CTcpUpload::~CTcpUpload()
{
    if(m_pSenbuf != NULL)
    {
        delete[] m_pSenbuf;
        m_pSenbuf = NULL;
    }
}

int CTcpUpload::OpenUpload(int userid, const char *ip, int port)
{
    m_userID = userid;
    
    m_socket = socket(AF_INET, SOCK_STREAM, 0 );
    if(m_socket < 0)
    {
    	
        return -1;
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    
    int iRet = connect(m_socket, (sockaddr*)&addr, sizeof(addr));
    if(iRet < 0)
    {
        close(m_socket);
        m_socket = -1;
        return -1;
    }
    
    return 0;
}

int CTcpUpload::CloseUpload()
{
    if(m_socket != -1)
    {
        close(m_socket);
        m_socket = -1;
    }
    
    return 0;
}

int CTcpUpload::Upload(const char *data, int datalen)
{
    int nSend = 0;
    static unsigned short index = 0;
    while(nSend < datalen)
    {
        int len = (datalen-nSend)<1400 ? (datalen-nSend):1400;
        
        *(short*)(m_pSenbuf) = len+10;
        *(int*)(m_pSenbuf+2) = m_userID;
        m_pSenbuf[6] = (char)0xf1;
        
        *(unsigned short*)(m_pSenbuf+7) = index;
        index++;
        
        m_pSenbuf[9] = 0x02;
        if(nSend ==0)
        {
            m_pSenbuf[9] = 0x01;
        }
        if(nSend+len == datalen)
        {
            m_pSenbuf[9] = 0x03;
        }
        
        memcpy(m_pSenbuf+10, data+nSend, len);
        int isend =sendn(m_pSenbuf,len+10);
        if(isend < 0)
        {
            close(m_socket);
            return 0;
        }
        nSend += len;
    }
    
    return nSend;
}

int CTcpUpload::sendn(const char* data, int len)
{
    int nsend = 0;
    int ret = 0;
    while(nsend < len)
    {
        ret = send(m_socket, data+nsend , len-nsend ,0 );
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
        nsend += ret;
    }
    return nsend;
}
