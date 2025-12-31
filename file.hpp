/*
* @file        file.hpp
* @brief       文件操作简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_FILE_HPP__
#define __LIBAPS_FILE_HPP__

#include <base/platform.hpp>
#include <base/datetime.hpp>
#include <base/directory.hpp>
#include <base/stream.hpp>
#include <base/toolkit.hpp>

#include <string>
#include <vector>

#ifndef _WIN32 
#include <dirent.h>
#endif

namespace aps
{
	using namespace std;

	//! 文件类型
    enum eFileType 
    {
        eFileTypeUnknown     =  0, /*!< 未知/虚拟 */
        eFileTypeFile        =  1, /*!< 普通文件 */
        eFileTypeDirectory   =  2, /*!< 目录 */
        eFileTypeCharDevice  =  4, /*!< 字符设备 */
        eFileTypeBlockDevice =  8, /*!< 块设备 */
        eFileTypeLink        = 16, /*!< 软连接 */
        eFileTypePipe        = 32  /*!< 命名/匿名管道 */
    };

	//! 文件信息类(文件系统)
	class LIBAPS_API FileInfo
	{
	public:
		FileInfo();
		FileInfo(const string& fullpath);
		~FileInfo();

		//! 获取文件信息
		void getFileInfo(string fullpath)noexcept(false);

		//! 纯名字
		const string& name()const;

		//! 纯名字[无扩展名]
		const string pureName();

		void setName(string _name);

		//! 返回扩展名, 不含.号
        string suffix();

		//! 返回扩展名, 不含.号
        static string getSuffix(string path);

		//! 修改扩展名(LINUX)
        static string changeSuffix(string path, string newSuffix);

        //! 修改扩展名(LINUX)
        FileInfo& changeSuffix(string newSuffix);

		//! 不含名字的路径 
        const string path(eDirSepStyle toStyle=eDirSepStyleAuto) const;

        void setPath(string _path);

        //! 返回含文件名的完整路径
        string fullpath(eDirSepStyle style=eDirSepStyleAuto) const ;

        //! 文件大小
        TInt64 size64() const;

        //! 文件大小
        off_t size() const;

        void setSize(TInt64 s);

        //! 文件类型
        eFileType type() const;

        void setType(eFileType t);

        //! 创建时间
        const Datetime& ctime() const;

        void setCtime(Datetime&time_);

        //! 修改时间
        const Datetime& mtime() const;

        void setMtime(Datetime&time_);

        //! 访问时间
        const Datetime& atime() const;

        void setAtime(Datetime&time_);

        // 类型判断 

        bool isVirtual() const;

        bool isFile() const;

        bool isDirectory() const;

        bool isCharDevice() const;

        bool isBlockDevice() const;

        bool isLink() const;

        bool isPipe() const;

        void reset(); //清空信息
		
		//! 修改路径中的分隔符
        static string setDirSepStyle(string origPath, eDirSepStyle toStyle);

	private:

		string      path_;
        string      dirSepChar_;
        string      name_;
        TInt64      size_;
        eFileType   type_;
        Datetime    ctime_;
        Datetime    mtime_;
        Datetime    atime_;

		void setValues(const FileInfo& other);
	};

	class LIBAPS_API File:public Stream
	{
	public:
		File();
		File(const string& path,const string& mode)noexcept(false);
		void open(const string& path,const string&mode)noexcept(false);
		~File();

		//! 关闭
        virtual void close();
 
        //! 写数据  
        virtual size_t  write(const char* buffer, size_t count);

        //! 读数据  
        virtual size_t  read(char* buffer, size_t count);

        //! 移动读写指针
        virtual bool    seek(off_t offset, int mode);

        //! 是否已打开
        bool isOpened();

        //! 截断/扩展文件
        void truncate(TInt64 size);
        
        //! 文件大小
        TInt64 size();

        //! 重新打开
        void reopen() noexcept(false);

        //! 路径
        const string& path() const;

        //! 删除
        void unlink() noexcept(false);

        // 静态函数
        //////////////////////////////////////////////////////////////////////////
        
        //! 删除
        static void unlink(const string& path)  noexcept(false);

        //! 是否存在
        static bool exists(const string& path) ;

        //! 移动
        static bool move(const string& pathA, const string& pathB) ;
        
        //! 是否是文件
        static bool isFile(const string& path);
        
        //! 获取文件信息
        static FileInfo getFileInfo(const string& path) noexcept(false);
        
        //! 创建临时文件供读写
        static void mktemp(const string& prefix, File& file) noexcept(false);
        
	private:
		FILE*    fp_;
        string   path_;
        string   openMode_;
	};

	#ifndef S_ISLNK
    #define S_ISLNK(m) (0)
    #endif
    #ifndef S_ISREG
    #define S_ISREG(m) ((m & _S_IFREG) == _S_IFREG)
    #endif
    #ifndef S_ISDIR
    #define S_ISDIR(m) (((m) & _S_IFDIR) == _S_IFDIR)
    #endif
}

#endif //!__LIBAPS_FILE_HPP__
