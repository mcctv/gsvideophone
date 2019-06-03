#ifndef CTCPDOWNLOAD_H
#define CTCPDOWNLOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <errno.h>

using namespace std;

typedef void (*TCP_DOWNLOAD_CB) (void *puser, char* data, int len);

class CTcpDownload
{
public:
    CTcpDownload();
    ~CTcpDownload();
    int OpenDownload(void *puser, int uerid, TCP_DOWNLOAD_CB cb,const char *ip, int port);
    int CloseDownload();
    
	friend void* DownLoadThread(void* p);

private:
    int Download();
    int Recvn(char* buf, int size);

	

private:
    int         m_nUserID;
    int         m_nSock;
    
    char       *m_pBufSend;
    char       *m_pBufRecv;
    char       *m_pbuf264;
    
	void       *m_pUser;
    bool        m_bcancle;
	
	sockaddr_in  m_addr;
    pthread_t    m_thDownload;
    
    TCP_DOWNLOAD_CB   m_cb;
   
};

#endif // CTCPDOWNLOAD_H
