
#ifndef CTCPUPLOAD_H
#define CTCPUPLOAD_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <signal.h>
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
#include <errno.h>


using namespace std;

class CTcpUpload
{
public:
    CTcpUpload();
    ~CTcpUpload();
    
    int OpenUpload(int userid, const char *ip, int port);
    int CloseUpload();
    int Upload(const char *data, int datalen);
    
private:
    
    int sendn(const char *data, int len);
    
private:
    int    m_userID;
    int    m_socket;
    char  *m_pSenbuf;
    
    
};

#endif // CTCPUPLOAD_H