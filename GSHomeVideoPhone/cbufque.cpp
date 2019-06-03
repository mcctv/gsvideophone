#include "cbufque.h"

cbufque::cbufque()
{
	pthread_mutex_init(&m_mutexused, NULL);
	pthread_mutex_init(&m_mutexfree, NULL);
}

cbufque::~cbufque()
{
	destroy_bufque();
	pthread_mutex_destroy(&m_mutexfree);
	pthread_mutex_destroy(&m_mutexused);
}

int cbufque::create_bufque(int bufsize, int quesize)
{
	m_nbuflen = bufsize;
	m_nbuflen = quesize;
	int size = 0;
	pthread_mutex_lock(&m_mutexfree);
	for(size = 0; size < quesize; size++)
	{
		BUFNODE *pnode = new BUFNODE;
		pnode->initbuf(bufsize);
		m_quefree.push(pnode);
	}
	pthread_mutex_unlock(&m_mutexfree);

	return 0;
}

int cbufque::destroy_bufque()
{
	pthread_mutex_lock(&m_mutexused);
	while( !m_queused.empty())
	{
		BUFNODE* pnode = m_queused.front();
		delete pnode;
		m_queused.pop();
	}
	pthread_mutex_unlock(&m_mutexused);

	pthread_mutex_lock(&m_mutexfree);
	while( !m_quefree.empty())
	{
		BUFNODE* pnode = m_quefree.front();
		delete pnode;
		m_quefree.pop();
	}
	pthread_mutex_unlock(&m_mutexfree);

	return 0;
}
	
BUFNODE* cbufque::alloc_node()
{
	BUFNODE* pnode = NULL;
	pthread_mutex_lock(&m_mutexfree);
	if(m_quefree.empty())
	{
		pnode = NULL;
	}
	else
	{
		pnode = m_quefree.front();
		m_quefree.pop();
	}
	pthread_mutex_unlock(&m_mutexfree);
	return pnode;
}

int cbufque::push_node(BUFNODE* node)
{
	pthread_mutex_lock(&m_mutexused);
	m_queused.push(node);
	pthread_mutex_unlock(&m_mutexused);
	return 0;
}

BUFNODE* cbufque::front_node()
{
	BUFNODE* pnode = NULL;
	pthread_mutex_lock(&m_mutexused);
	if(m_queused.empty())
	{
		pnode = NULL;
	}
	else
	{
		pnode = m_queused.front();
	}
	pthread_mutex_unlock(&m_mutexused);
	return pnode;
}

BUFNODE* cbufque::pop_node()
{
	BUFNODE* pnode = NULL;
	pthread_mutex_lock(&m_mutexused);
	if(m_queused.empty())
	{
		pnode = NULL;
	}
	else
	{
		pnode = m_queused.front();
		m_queused.pop();
	}
	pthread_mutex_unlock(&m_mutexused);
	return pnode;
}

int cbufque::free_node(BUFNODE* node)
{

	pthread_mutex_lock(&m_mutexfree);
	m_quefree.push(node);
	pthread_mutex_unlock(&m_mutexfree);
	return 0;
}

int cbufque::get_freenode_count()
{
	int count = 0;
	pthread_mutex_lock(&m_mutexfree);
	count = m_quefree.size();
	pthread_mutex_unlock(&m_mutexfree);
	return count;
}

int cbufque::get_usednode_count()
{
	int count = 0;
	pthread_mutex_lock(&m_mutexused);
	count = m_queused.size();
	pthread_mutex_unlock(&m_mutexused);
	return count;
}

int cbufque::get_quelen()
{
	return m_nqueuelen;
}

int cbufque::reset_node()
{
	pthread_mutex_lock(&m_mutexused);
	pthread_mutex_lock(&m_mutexfree);
	while(!m_queused.empty())
	{
		BUFNODE *pnode = m_queused.front();
		m_quefree.push(pnode);
		m_queused.pop();
	}
	pthread_mutex_unlock(&m_mutexfree);
	pthread_mutex_unlock(&m_mutexused);
	return 0;
}

