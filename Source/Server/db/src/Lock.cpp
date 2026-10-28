#include "stdafx.h"
#include "Lock.h"

CLock::CLock()
{
}

CLock::~CLock()
{
}

void CLock::Initialize()
{
	m_bLocked = false;
	pthread_mutex_init(&m_lock, NULL);
}

void CLock::Destroy()
{
	assert(!m_bLocked && "lock didn't released");
	pthread_mutex_destroy(&m_lock);
}

int CLock::Trylock()
{
	return pthread_mutex_trylock(&m_lock);
}

void CLock::Lock()
{
	pthread_mutex_lock(&m_lock);
	m_bLocked = true;
}

void CLock::Unlock()
{
	assert(m_bLocked && "lock didn't issued");
	m_bLocked = false;
	pthread_mutex_unlock(&m_lock);
}
