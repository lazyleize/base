/*
* @file    tcpSocket.hpp
* @brief   socket操作简单封装
* @todo    需要分开TcpClient/TcpServer
*
* @author  潘孙友<pansunyou@gmail.com>
*/
#ifndef __LIBAPS_TCPSOCKET_HPP__
#define __LIBAPS_TCPSOCKET_HPP__

#include <base/platform.hpp>
#include <sstream>
#include <string>

#ifndef _WIN32
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
#endif

namespace aps 
{
    using namespace std;

    /// @brief TcpSocket操作类
    class LIBAPS_API TcpSocket
    {
    public:
        TcpSocket();
        TcpSocket(int fd);
        virtual ~TcpSocket();

        /// @brief 关闭socket(如果合法)
        void close();
        
        ///
        /// @brief 发送数据
        bool send(const void*pBuffer, int toSendLen);
        
        /// @brief 检测socketFd值是否非法
        bool isValid();

        ///
        /// @brief 接收数据(接收时阻塞)
        /// @retVal true: 成功; false:失败
        bool recv(void*pBuffer, int toReadBytes, int* readedBytes, int timeout=3000);
        
        ///
        /// @brief 连接远程主机
        ///
        /// @param host ip
        /// @param port 端口
        /// @param timeout_ 毫秒
        /// @retVal true: 成功; false:失败
        bool connect(string hostIP, int port, int timeout_=-1);
        
        /// @brief 断开连接
        void disConnect();

        /// @brief 是否连接
        bool isConnected();

        /// @brief 手工设置连接
        void setConnected(bool b);

        /// @绑定本机端口及地址
        /// @retVal true: 成功; false:失败
        bool bind(int port);
        
        /// @brief 监听
        TcpSocket *accept();
        
        /// @brief 获取错误信息
        const char* what();
        
        /// @brief 远程地址/客户端地址
        string getRemoteAddr();
        
        /// @brief 远程端口/客户端端口
        int getRemotePort();
        
        /// @brief 本地址[连接时用]
        string getLocalAddr();
        
        /// @brief 本地端口[连接时用]
        int getLocalPort();
      
        /// @brief 创建socket
        int resetSocket();
       
        /// @brief 返回内部socketFd
        int& getSockFd();
        
        /// @brief 清空内部socketFd
        void clearFd();

        /// @brief 设置阻塞/非阻塞模式
        bool setBlock(bool block);

        ///----------------------------------------------------------------

        /// @brief 设置阻塞/非阻塞模式
        static bool setBlock( int fd, bool block);
        
        /// @brief 创建socket
        static int newSocket();

        /// @brief 端口重用
        static bool setAddrReuse(int fd);
        
        /// @brief 检测socketFd值是否非法
        static bool isValid(int fd);
        
        ///
        /// @brief 接收数据(接收时阻塞)
        ///
        /// @param pBuffer      数据缓冲区
        /// @param toReadBytes  待读字节
        /// @param readedBytes  已读取字节
        /// @param timeout      多少豪秒后超时, -1表示一直等待直至系统超时[未实现]
        /// @retVal true : 连接保持正常
        ///         false: 连接丢失
        static bool recv(int fd, void*pBuffer, int toReadBytes, int* readedBytes, int timeout=3000);
        
        ///
        /// @brief 发送数据
        ///
        /// 由于发送时无法检测对方是否已断开, 故返回值只有成功/失败.
        ///
        /// @param fd        socket
        /// @param pBuffer   数据起始.
        /// @param toSendLen 待发送数据长度.
        /// @retVal true : 成功发送toSendLen
        ///         false: 失败
        static bool send(int fd, const void*pBuffer, int toSendLen);

        ///
        /// @brief 获取可用的端口
        /// @param port : 输出，返回可用的端口号
        /// @retVal 若成功则返回true，否则放回false
        static bool getFreePort(int &port);

    protected:
    
        string  mErrMsg;
        string  mAddr;          //!< Accept后存储客户端地址/连接远程后的地址.
        int     mPort;          //!< 同上
        int     socketFd;       
        bool    mIsConnected;   
        sockaddr_in mSrvAddr;
        
        void setErrorMsg(string errmsg);
        void setErrorMsg();
    };

}

#endif // !__LIBAPS_TCPSOCKET_HPP__
