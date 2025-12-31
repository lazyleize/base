# Base - C++ 跨平台基础类库

## 概述

Base 是一个功能丰富的 C++ 跨平台基础类库，提供了应用程序开发所需的核心功能模块。该库采用面向对象设计，支持 Windows 和 POSIX（Linux/Unix）平台，为开发者提供统一的 API 接口，简化跨平台开发工作。

## 主要特性

### 1. 平台抽象层
- **跨平台支持**：提供 Windows 和 POSIX 平台的统一抽象接口
- **平台检测**：自动识别编译平台并选择对应的实现
- **平台特定功能封装**：隐藏平台差异，提供一致的编程接口

### 2. 多线程与同步机制
- **线程管理**：Thread 类提供线程创建、启动、等待、取消等完整功能
- **线程池**：ThreadPool 支持线程池管理，提高并发处理效率
- **互斥锁**：Mutex 提供线程安全的互斥访问控制
- **信号量**：Semaphore 支持资源计数和同步
- **事件对象**：Event 用于线程间事件通知和同步
- **条件变量**：ConditionVar 支持更复杂的线程同步场景
- **线程局部存储**：ThreadKey 提供线程特定的数据存储

### 3. 文件系统操作
- **文件操作**：File 类提供文件读写、创建、删除、移动等操作
- **目录管理**：Directory 类支持目录遍历、创建、删除等功能
- **文件信息**：FileInfo 获取文件大小、修改时间、类型等属性
- **文件锁定**：FileLock 提供文件级别的锁定机制
- **流式处理**：Stream 接口支持流式数据读写

### 4. 网络编程
- **TCP Socket**：TcpSocket 类封装 TCP 网络通信功能
- **连接管理**：支持客户端连接、服务器监听等网络操作
- **数据传输**：提供可靠的数据发送和接收接口

### 5. 字符串与字符编码
- **字符集转换**：IConv 类支持多种字符编码之间的转换（如 GBK、UTF-8、ISO-8859-1 等）
- **UTF-8 支持**：集成 UTF-8 编码库，支持 UTF-8 字符串处理
- **字符串工具**：StrHelper 提供字符串分割、格式化等常用操作
- **EBCDIC 支持**：支持 EBCDIC 编码转换

### 6. 加密与摘要算法
- **MD5 摘要**：MD5 类提供 MD5 哈希计算
- **SHA1 摘要**：SHA1 类提供 SHA-1 哈希计算
- **CRC32 校验**：CRC32 类提供循环冗余校验
- **Base64 编码**：Base64 类支持 Base64 编码/解码
- **摘要接口**：Digest 抽象基类提供统一的摘要算法接口

### 7. 日期时间处理
- **日期类**：Date 类提供日期计算、格式化等功能
- **时间类**：Time 类提供时间操作和格式化
- **日期时间**：DateTime 类结合日期和时间，支持完整的日期时间操作
- **时间戳转换**：支持与系统时间戳的相互转换

### 8. 配置管理
- **配置基类**：AppConfig 提供统一的配置管理接口
- **INI 配置**：AppConfigIni 支持 INI 格式配置文件
- **XML 配置**：AppConfigXML 支持 XML 格式配置文件
- **配置读写**：支持配置项的读取、写入和保存

### 9. 日志系统
- **日志接口**：Logger 提供统一的日志记录接口
- **文件日志**：FileLogger 支持日志写入文件，支持日志轮转
- **控制台日志**：ConsoleLogger 支持控制台日志输出
- **日志级别**：支持不同级别的日志记录（DEBUG、INFO、WARN、ERROR 等）
- **格式化输出**：支持自定义日志前缀格式，包含时间、进程ID、线程ID、源文件等信息
- **控制台颜色**：ConsoleColor 支持彩色日志输出（Windows/POSIX）

### 10. XML 处理
- **XML 解析**：集成 TinyXML 和 TinyXML2 库
- **XML 消息**：XMLMessage 类提供便捷的 XML 数据封装和解析
- **DOM 操作**：支持 XML 文档的创建、修改和查询

### 11. JSON 处理
- **JSON 支持**：集成 jsoncpp 库，提供完整的 JSON 读写功能
- **JSON 值**：Value 类表示 JSON 数据值
- **JSON 读写**：Reader 和 Writer 类支持 JSON 解析和生成

### 12. 应用程序框架
- **控制台应用**：ConsoleApplication 提供控制台应用程序的基础框架
- **服务器应用**：ServerApplication 提供服务器应用程序的基础框架
- **初始化支持**：InitAps 提供库的初始化功能

### 13. 工具类
- **UUID 生成**：Uuid 类支持生成唯一标识符
- **内存缓冲区**：Membuffer 提供高效的内存缓冲区管理
- **异常处理**：Exception 类提供统一的异常处理机制
- **工具函数**：Toolkit 类提供二进制转十六进制、文件大小格式化、错误格式化等实用工具
- **字节序**：Byteorder 支持字节序转换
- **源信息**：SourceInfo 提供源代码位置信息

### 14. 插件系统
- **插件基类**：PluginBase 提供插件开发的基础类
- **插件管理**：支持插件的动态加载、卸载和管理
- **插件元信息**：PluginMetaInfo 描述插件接口和功能

### 15. 动态库加载
- **共享库**：SharedLibrary 类支持动态库（DLL/SO）的加载和符号解析
- **跨平台**：支持 Windows DLL 和 POSIX 共享库的统一接口

### 16. 其他功能
- **正则表达式**：Deelx 提供正则表达式支持
- **单元测试**：Unittest 提供单元测试框架支持
- **单例模式**：Singleton 模板类提供单例模式实现
- **作用域锁**：ScopedLock 提供 RAII 风格的自动锁管理

## 编译配置

- **项目类型**：静态库（Debug）/ 动态库（Release）
- **目标名称**：libaps.base
- **字符集**：多字节字符集（MultiByte）
- **编译器**：Visual Studio 2012 (v110) / 支持 POSIX 的编译器

## 命名空间

所有类都在 `aps` 命名空间中定义。

## 使用示例

```cpp
#include <all.hpp>
using namespace aps;

// 文件操作
File file("test.txt");
file.write("Hello World");

// 线程创建
Thread thread;
thread.start([](void* arg) {
    // 线程执行代码
}, nullptr);

// 日志记录
Logger::getLogger("myapp")->info("Application started");

// 配置读取
AppConfigIni config("app.ini");
string value = config.readString("section", "key", "default");
```

## 插件系统使用示例

插件系统允许您动态加载和管理插件，实现功能的模块化和扩展性。

### 1. 定义插件接口

首先，定义一个插件接口（所有插件必须继承自 `PluginBase`）：

```cpp
// IDriver.hpp - 插件接口定义
#include <plugin.hpp>
#include <string>

using namespace aps;

class IDriver : public PluginBase
{
public:
    virtual ~IDriver() {}
    
    // 插件接口方法
    virtual bool connect(const std::string& address) = 0;
    virtual void disconnect() = 0;
    virtual std::string sendCommand(const std::string& cmd) = 0;
    
    // 声明插件创建和销毁函数（必须）
    Z_DECL_SO_API
};
```

### 2. 实现插件

实现具体的插件类：

```cpp
// MyDriver.cpp - 插件实现
#include "IDriver.hpp"
#include <logger.hpp>

class MyDriver : public IDriver
{
public:
    MyDriver() {
        Z_LOG_X(eINFO) << "MyDriver created";
    }
    
    virtual ~MyDriver() {
        Z_LOG_X(eINFO) << "MyDriver destroyed";
    }
    
    virtual bool connect(const std::string& address) override {
        Z_LOG_X(eINFO) << "Connecting to " << address;
        // 实现连接逻辑
        return true;
    }
    
    virtual void disconnect() override {
        Z_LOG_X(eINFO) << "Disconnecting";
        // 实现断开逻辑
    }
    
    virtual std::string sendCommand(const std::string& cmd) override {
        Z_LOG_X(eINFO) << "Sending command: " << cmd;
        // 实现命令发送逻辑
        return "OK";
    }
};

// 实现插件创建和销毁函数（必须）
Z_IMPL_SO_API("MyDriver", MyDriver, IDriver)
```

### 3. 使用插件

在主程序中使用插件：

```cpp
// main.cpp - 使用插件
#include <all.hpp>
#include "IDriver.hpp"
#include <plugin.hpp>

using namespace aps;

int main() {
    try {
        // 创建插件工厂（指定接口类型）
        PluginFactory<IDriver> factory;
        
        // 方式1：加载单个插件文件
        factory.loadFile("./plugins/MyDriver.dll");  // Windows
        // factory.loadFile("./plugins/MyDriver.so"); // Linux
        
        // 方式2：加载目录中的所有插件
        factory.loadDir("./plugins");
        
        // 检查插件是否可用
        if (factory.provides("MyDriver")) {
            // 创建插件实例
            IDriver* driver = factory.create("MyDriver");
            
            if (driver) {
                // 使用插件
                driver->connect("192.168.1.100");
                std::string result = driver->sendCommand("test");
                driver->disconnect();
                
                // 销毁插件实例（必须调用，不能直接 delete）
                factory.destroy(driver);
            }
        }
        
        // 打印所有已加载的插件
        factory.dump();
        
        // 卸载插件文件（可选）
        // factory.unLoadFile("./plugins/MyDriver.dll");
        
    } catch (Exception& e) {
        Z_LOG_X(eERROR) << "Plugin error: " << e.what();
        return -1;
    }
    
    return 0;
}
```

### 4. 编译插件

插件需要编译为动态库：

**Windows (Visual Studio):**
```cpp
// 项目设置为 DLL，导出符号
// 确保链接到 libaps.base.lib
```

**Linux (GCC):**
```bash
g++ -shared -fPIC -o MyDriver.so MyDriver.cpp \
    -I/path/to/base/inc \
    -L/path/to/base/lib -laps.base
```

### 5. 插件系统特性

- **类型安全**：使用模板和动态类型转换确保类型安全
- **引用计数**：自动管理插件的生命周期
- **线程安全**：插件工厂内部使用互斥锁保护
- **多插件支持**：一个动态库可以包含多个插件实现
- **接口隔离**：每个插件工厂只加载特定接口的插件

### 6. 注意事项

- 插件必须继承自 `PluginBase` 和您的接口类
- 必须使用 `Z_DECL_SO_API` 和 `Z_IMPL_SO_API` 宏
- 插件实例必须通过 `factory.destroy()` 销毁，不能直接 `delete`
- 卸载插件文件前，必须确保所有插件实例已被销毁
- 插件文件需要导出 `ZFPT_plugin` 符号

## 平台支持

- **Windows**：Windows 32/64 位平台
- **POSIX**：Linux、Unix 等 POSIX 兼容系统

## 依赖项

- **TinyXML/TinyXML2**：XML 解析库
- **jsoncpp**：JSON 处理库
- **UTF-8 库**：UTF-8 编码支持
- **iconv**：字符编码转换库（POSIX 平台）

## 注意事项

- 该库提供了丰富的跨平台功能，但在使用时需要注意平台特定的行为差异
- 线程相关的类需要正确管理资源，避免资源泄漏
- 文件操作需要注意路径分隔符在不同平台上的差异
- 网络编程需要注意错误处理和连接状态管理

## 作者

lazyleize <398369685@qq.com>

## 版权

Copyright (c) lazyleize

