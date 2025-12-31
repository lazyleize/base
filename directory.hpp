/*
* @file        directory.hpp
* @brief       目录操作简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#ifndef __LIBAPS_DIRECTORY_HPP__
#define __LIBAPS_DIRECTORY_HPP__

#include <base/platform.hpp>
#include <base/exception.hpp>

#include <string>
#include <vector>
#ifndef _WIN32
#include <dirent.h>
#endif

namespace aps 
{
    using namespace std;

    class FileInfo;
    class File;

    enum eDirSepStyle
    {
        eDirSepStyleAuto,
        eDirSepStyleWIN,
        eDirSepStyleUNIX
    };

    /// @brief 目录操作类(文件系统)
    class LIBAPS_API Directory
    {
    public:
        Directory(string dirpath);
        virtual ~Directory();

        void rewind();
        bool getCurFile(FileInfo &fileInfo) noexcept(false);
        bool getNextFile(FileInfo &fileInfo) noexcept(false);

        void setSeparator(char separator);
        string separator(eDirSepStyle style=eDirSepStyleWIN);
        FileInfo getDirInfo();

        /// 静态方法

        /*
        * @brief 获取目录下第一层文件
        * @param files 存储文件列表
        * @param bIncludeDot 结果中是否含 . ..
        */
        static bool getSubFiles(string dirPath, vector<FileInfo> &files, bool bIncludeDot=false) noexcept(false);

        /*
        * @brief 获取目录下所有文件(含子目录中)
        * @param files 存储文件列表
        * @param bIncludeDot 结果中是否含 . ..
        */
        static bool getAllSubFiles(string dirPath, vector<FileInfo> &files, bool bIncludeDot=false);

        /*
        * @brief 创建目录/路径
        * @param fullpath 目录路径
        * @param bCreateFullPath 是否创建完成路径
        */
        static int mkdir(string fullpath, bool bCreateFullPath=true);

        /*
        * @brief 删除目录/路径
        * @param fullpath 目录路径
        * @param bFailWhenNoEmpty 是否创建完成路径
        */
        static int rmdir(string fullpath, bool bFailWhenNoEmpty=true);

        //! 判断是不是目录
        static bool isDirectory(string fullpath);

    private:
        FileInfo *mDirInfo;
        struct dir_handle_t;
        dir_handle_t *mHandle;
        string mSeparator;
    };

}

#endif //!__LIBAPS_DIRECTORY_HPP__

