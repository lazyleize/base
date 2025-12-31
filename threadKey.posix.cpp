/*
 * @file        threadKey.cpp
 * @brief       线程私有数据
 * @author      Christian Prochnow <cproch@seculogix.de>
 * @copyright   xgd
*/
#include <base/threadKey.hpp>

#ifdef Z_OS_LINUX
#include <pthread.h>

using namespace aps;

struct ThreadKeyImpl::key_handle_t
{
    pthread_key_t key;
};

ThreadKeyImpl::ThreadKeyImpl()
: m_handle(new key_handle_t)
{
    pthread_key_create(&m_handle->key, 0);
}

ThreadKeyImpl::~ThreadKeyImpl()
{
    pthread_key_delete(m_handle->key);
    delete m_handle;
}

void ThreadKeyImpl::set(void* ptr) throw()
{
    pthread_setspecific(m_handle->key, ptr);
}

void* ThreadKeyImpl::get() throw()
{
    return pthread_getspecific(m_handle->key);
}

#endif
