/*
 * @file        threadKey.cpp
 * @brief       线程私有数据
 * @author      Christian Prochnow <cproch@seculogix.de>
 * @copyright   xgd
*/
#include <base/threadKey.hpp>
#include <base/exception.hpp>
#include <base/charset.hpp>
#include <base/toolkit.hpp>

#ifdef Z_OS_WIN32
#include <Windows.h>
#include <memory>

using namespace aps;
using namespace std;

struct ThreadKeyImpl::key_handle_t
{
    DWORD tls;
    key_handle_t()
    {
        tls = TLS_OUT_OF_INDEXES;
    }
};

ThreadKeyImpl::ThreadKeyImpl():m_handle(new key_handle_t)
{
	m_handle->tls = TlsAlloc();
	if(m_handle->tls == TLS_OUT_OF_INDEXES)
	{
		int r = GetLastError();
		throw Exception(Z_SOURCEINFO,r,Toolkit::formatError(r));
	}
}

ThreadKeyImpl::~ThreadKeyImpl()
{
    if (m_handle->tls!=TLS_OUT_OF_INDEXES)
        TlsFree(m_handle->tls);
    delete m_handle;
}

void ThreadKeyImpl::set(void *ptr)throw()
{
	if(m_handle->tls != TLS_OUT_OF_INDEXES)
		TlsSetValue(m_handle->tls,ptr);
}

void* ThreadKeyImpl::get()throw()
{
	if(m_handle->tls!=TLS_OUT_OF_INDEXES)
		return TlsGetValue(m_handle->tls);
	return NULL;
}

#endif