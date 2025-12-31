/*
 * @file    tcpSocket.cpp
 * @brief   socket操作简单封装
 * @author  sunu<pansunyou@gmail.com>
 *
*/
#include <base/tcpSocket.hpp>
#include <base/charset.hpp>
#include <base/toolkit.hpp>
#include <base/logger.hpp>

#ifndef Z_OS_WIN32
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#else
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#include <sys/types.h>
typedef int socklen_t;
#endif

#include <errno.h>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <errno.h>

static bool recvEx(int fd, void*pBuffer, int toReadLen, int* readedBytes, int timeout, bool& bConnected);

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

using namespace aps;

using namespace std;

#ifdef Z_OS_WIN32
class CSocketInit
{
public:
    CSocketInit();
    ~CSocketInit();
};

CSocketInit::CSocketInit()
{
    WSADATA wsaData;
    int Ret;
    if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
    {
        TraceX(eERROR, "WSAStartup failed with error %d\n", Ret);
        exit(-1);
    }
}

CSocketInit::~CSocketInit()
{ 
    WSACleanup();
}

static CSocketInit socketIniter;
#endif 

TcpSocket::TcpSocket()
{
    setErrorMsg("");
    socketFd = INVALID_SOCKET;
    mIsConnected = false;
}

TcpSocket::TcpSocket(int _socketFd)
{
    socketFd = _socketFd;
}

TcpSocket::~TcpSocket()
{
    disConnect();
}

void TcpSocket::disConnect()
{
    this->close();
}


//! 清空内部socketFd
void TcpSocket::clearFd()
{
    socketFd=INVALID_SOCKET;
    mIsConnected = false;
}


void TcpSocket::close()
{
    mIsConnected = false;
    if (socketFd!=INVALID_SOCKET)
    {
#ifdef WIN32
        ::closesocket(socketFd);
#else
        ::close(socketFd);
#endif
    }
}


bool TcpSocket::send(int fd, const void*pBuffer0, int dataLen)
{
    Z_ASSERT(pBuffer0!=NULL);
    
    int sendTotalLen=0;
    if(fd == INVALID_SOCKET)
        return false;
    
    char *pBuffer = (char*)pBuffer0;
    const int packSize = 512;
    for( ; (int)sendTotalLen<dataLen ; )
    {
        int tmpSize = packSize;
        if ( (int)(sendTotalLen+tmpSize)>=dataLen )
            tmpSize = dataLen - sendTotalLen;
        
        int sendLen = ::send(fd, pBuffer+sendTotalLen, tmpSize, 0);
        if (sendLen<=0)
            return false;
        
        sendTotalLen += sendLen;
    }
    
    return true;
}


bool TcpSocket::send(const void*pBuffer, int sendLen)
{
    if (!mIsConnected)
        return false;
        
    bool r = TcpSocket::send(socketFd, pBuffer, sendLen);
    if (!r)
        setErrorMsg();
    return r;
}


bool TcpSocket::isValid()
{
    return TcpSocket::isValid(socketFd);
}

/// @brief 是否连接
bool TcpSocket::isConnected()
{
    return mIsConnected;
}

/// @brief 手工设置连接
void TcpSocket::setConnected(bool b)
{
    mIsConnected = b;
}

/// @brief 检测socketFd值是否非法
bool TcpSocket::isValid(int fd)
{
    return INVALID_SOCKET!=fd;
}


bool TcpSocket::recv(int fd, void*pBuffer, int toReadLen, int* readedBytes, int timeout)
{
    bool bConnect = true;
    recvEx(fd, pBuffer, toReadLen, readedBytes, timeout, bConnect);
    return bConnect;
}


bool TcpSocket::recv(void*pBuffer, int toReadLen, int* readedBytes, int timeout)
{
    if (!mIsConnected)
        return false;
    recvEx(socketFd, pBuffer, toReadLen, readedBytes, timeout, mIsConnected);
    return mIsConnected;
}


//! 本地址[连接时用]
string TcpSocket::getLocalAddr()
{
    sockaddr_in addrMy;
    memset(&addrMy,0,sizeof(addrMy));
    socklen_t len = sizeof(addrMy);
    int ret = getsockname(socketFd, (sockaddr*)&addrMy, &len);
    if (ret != 0)
        return string("127.0.0.1");

    return inet_ntoa(addrMy.sin_addr);
}


//! 本地端口[连接时用]
int TcpSocket::getLocalPort()
{
    sockaddr_in addrMy;
    memset(&addrMy,0,sizeof(addrMy));
    socklen_t len = sizeof(addrMy);
    int ret = getsockname(socketFd, (sockaddr*)&addrMy, &len);
    if (ret != 0)
        return -1;
    return ntohs(addrMy.sin_port);
 }


// 
bool TcpSocket::connect(string hostIP, int port, int timeout_)
{
    mIsConnected = false;
    mAddr = hostIP;
    mPort = port;
 
    sockaddr_in remoteHostAddr;
    if(socketFd == INVALID_SOCKET)
        resetSocket();
    
    memset(&remoteHostAddr, 0, sizeof(remoteHostAddr));
    remoteHostAddr.sin_port = htons(port);
    remoteHostAddr.sin_family = AF_INET;
    remoteHostAddr.sin_addr.s_addr = inet_addr(hostIP.c_str());

#ifdef WIN32
    unsigned   long   flag;
	flag = 1;
#endif
    bool bUseTimeout = true;
    bool bBlocking = true;
    
    if (timeout_>0)
    {
#ifndef WIN32
        int flags = fcntl(socketFd, F_GETFL, 0); 
        if (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK)!=0)
#else 
        if (ioctlsocket(socketFd, FIONBIO, &flag)!=0)
#endif 
            bUseTimeout = false;
        else 
            bBlocking = false;
    }
    else
    {
        bUseTimeout = false;
    }

    if (bUseTimeout)
    {
        ::connect(socketFd, (sockaddr*)&remoteHostAddr, sizeof(sockaddr_in));
        struct timeval timeout;
        timeout.tv_sec  = timeout_ / 1000 ; // 秒
        timeout.tv_usec = (timeout_ % 1000) * 1000 ; // 微秒
        
        fd_set mask;
        FD_ZERO(&mask);   
        FD_SET(socketFd, &mask);

        int r = select(socketFd+1, NULL, &mask, NULL, &timeout);
        
        if (r==-1)
        {
            this->close();
            setErrorMsg("select error");
            return false;
        }
        else if (r==0)
        {
            this->close();
            setErrorMsg("Connect timeout");
            return false;
        }
    }
    else 
    {
        if (::connect(socketFd, (sockaddr*)&remoteHostAddr, sizeof(sockaddr_in)) == -1)
        {   
            this->close();
            setErrorMsg();
            return false;
        }
    }
    
    // 改回阻塞模式
    if (bBlocking==false)
    {
#ifdef WIN32
        flag = 0;
#endif
#ifndef WIN32
        int flags = fcntl(socketFd, F_GETFL, 0); 
        fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
#else 
        ioctlsocket(socketFd, FIONBIO, &flag);
#endif 
    }

    mIsConnected = true;
    return true;
}


bool TcpSocket::bind(int port)
{
    memset(&mSrvAddr, 0x00, sizeof(mSrvAddr));
    
    mSrvAddr.sin_family = AF_INET;
    mSrvAddr.sin_addr.s_addr = INADDR_ANY;
    mSrvAddr.sin_port = htons ( port );
    
    if (socketFd==INVALID_SOCKET)
        resetSocket();

    TcpSocket::setAddrReuse(socketFd);
    int intVal = ::bind(socketFd, (struct sockaddr *)&mSrvAddr, sizeof(mSrvAddr ) );
    if (intVal==-1)
    {
        setErrorMsg();
        return false;
    }
    
    if (::listen(socketFd, 512)==-1)
    {
        setErrorMsg("TcpSocket::bind listen fail");
        return false;
    }
    
    return true;
}


TcpSocket *TcpSocket::accept()
{
    sockaddr_in clientAddr;

    for(;;)
    {
        memset(&clientAddr, 0x00, sizeof(clientAddr));
        int addrLen = sizeof( clientAddr );
        int newSocket = ::accept(socketFd, (sockaddr * )&clientAddr, ( socklen_t * ) &addrLen );

        if( -1 == newSocket && EINTR == errno )
        {
            //fprintf(stderr, "errno=%d\n", errno);
            continue;
        }
        
        if (newSocket!=INVALID_SOCKET)
        {
            TcpSocket *pNew = new TcpSocket(newSocket);
            pNew->mAddr = inet_ntoa(clientAddr.sin_addr);
            pNew->mPort = ntohs(clientAddr.sin_port);
            pNew->mIsConnected = true;
            return pNew;
        }
    }

    return NULL;
}

/**
* @brief 获取错误信息
*/
const char* TcpSocket::what()
{
    return mErrMsg.c_str();
}

string TcpSocket::getRemoteAddr()
{   
    return mAddr;
}

int TcpSocket::getRemotePort()
{   
    return mPort;
}

void TcpSocket::setErrorMsg(string errmsg)
{
    mErrMsg = errmsg;
}


void TcpSocket::setErrorMsg()
{       
#ifdef Z_OS_WIN32
    mErrMsg = Toolkit::formatError(WSAGetLastError());
#else
    mErrMsg = strerror(errno);
#endif
    Z_LOG_X(eTRACE) << "Error: " << mErrMsg;
}


int TcpSocket::resetSocket()
{
    this->close();
    socketFd = TcpSocket::newSocket();
    if(socketFd == INVALID_SOCKET)
        setErrorMsg();
    return socketFd;
}

//! 创建socket
int TcpSocket::newSocket()
{
    int s = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return s;
}


int& TcpSocket::getSockFd()
{
    return socketFd;
}


bool TcpSocket::setAddrReuse( int fd )
{
#ifndef WIN32
    int flag = 1;
    int len = sizeof(flag);
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len) != 0)
    {
        int err = errno;
        Z_LOG_X(eWARNING) << "set SO_REUSEADDR fail! "<< Toolkit::formatError(err);
        return false;
    }
#endif 
    return true;
}


bool TcpSocket::setBlock( int fd, bool block)
{
#ifndef _WIN32
    return fcntl(fd, F_SETFL, block ? 0 : O_NONBLOCK) != -1;
#else
    return false;
#endif 
}


bool TcpSocket::setBlock(bool block)
{
    return setBlock(socketFd, block);
}

bool recvEx(int fd, void*pBuffer0, int toReadLen, int* readedBytes, int timeout, bool& bConnected)
{
    Z_ASSERT(pBuffer0!=NULL);
    Z_ASSERT(readedBytes!=NULL);

    *readedBytes = 0;
    if(fd == INVALID_SOCKET)
        return false;
    
    ///
    /// @todo
    /// 整个读取的时间应该控制在timeout毫秒之内.
    /// 而不是单个操作.
    ///
    
    int recvTotalLen = 0;

    char *pBuffer = (char*)pBuffer0;
    const int packSize = 512;
    for( ;(int)recvTotalLen< toReadLen; )
    {
        int tmpSize = packSize;
        if ( (int)(recvTotalLen+tmpSize)>=toReadLen )
            tmpSize = toReadLen - recvTotalLen;

        int recvSize = ::recv(fd, pBuffer+recvTotalLen, tmpSize, 0);
        if (recvSize<=0)  // recvSize==0 对方已断开连接, 这种情况应该返回失败.
        {
            bConnected = false;
            break;
        }
        recvTotalLen += recvSize;
    }
    
    *readedBytes = recvTotalLen;
    return bConnected;
}


/**
 * @brief 获取可用的端口
 * @param port : 输出，返回可用的端口号
 * @return 若成功则返回true，否则放回false
 */
bool TcpSocket::getFreePort(int &port)
{
    bool result = true;

    // 1. 创建一个socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 创建一个sockaddr，并将它的端口号设为0
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 若port指定为0,则调用bind时，系统会为其指定一个可用的端口号
    addr.sin_port = 0;        
    
    // 3. 绑定
    int ret = ::bind(sock, (struct sockaddr *)&addr, sizeof addr);

    if (0 != ret) {
        result = false;
        return result;
    }

    // 4. 利用getsockname获取
    struct sockaddr_in connAddr;
    socklen_t len = sizeof connAddr;
    ret = getsockname(sock, (struct sockaddr *)&connAddr, &len);

    if (0 != ret){
        result = false;
        return result;
    }

    port = ntohs(connAddr.sin_port); // 获取端口号
    
#ifndef _WIN32
    if ( 0 != ::close(sock) )
#else
    if ( 0 != ::closesocket(sock) )
#endif 
        result = false;
    return result;
}


