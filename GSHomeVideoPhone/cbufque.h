#ifndef CBUFQUE_H_
#define CBUFQUE_H_
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <pthread.h>

using namespace std;

struct BUFNODE
{
	char *pbuf;
	int  nlen;
	int  nmaxlen;

	BUFNODE()
	{
		pbuf = NULL;
		nlen = 0;
		nmaxlen = 0;
	}

	~BUFNODE()
	{
		if(pbuf != NULL)
		{
			delete[] pbuf;
			pbuf = NULL;
		}
		nlen = 0;
		nmaxlen = 0;
	}

	int initbuf(int bufsize)
	{
		nmaxlen = bufsize;
		pbuf = new char[bufsize];
		if(pbuf == NULL)
		{
			nmaxlen = 0;
			return -1;
		}
		nlen = 0;
		return  0;
	}
};

class cbufque
{
public:
	cbufque();
	~cbufque();

	int create_bufque(int bufsize =2048, int quesize=2048);
	int destroy_bufque();
	
	BUFNODE* alloc_node();
	int push_node(BUFNODE* node);

	BUFNODE* front_node();
	BUFNODE* pop_node();
	int free_node(BUFNODE* node);

	int get_freenode_count();
	int get_usednode_count();
	int get_quelen();
	int reset_node();
private:
	int m_nqueuelen;
	int m_nbuflen;

	queue<BUFNODE*> m_queused;
	queue<BUFNODE*> m_quefree;
	
	pthread_mutex_t m_mutexused;
	pthread_mutex_t m_mutexfree;

};


#endif
