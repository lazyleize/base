#ifdef Z_OS_WIN32
#pragma comment(lib, "Shlwapi.lib")

#include <Windows.h>
#ifdef _DEBUG 
#include <crtdbg.h>
#endif

struct timeval;

namespace aps 
{
    int gettimeofday(struct timeval *tp, void *tzp);
}

#ifndef snprintf
#define snprintf _snprintf
#endif

#pragma warning( disable : 4290 )


#endif // !#ifdef Z_OS_WIN32

