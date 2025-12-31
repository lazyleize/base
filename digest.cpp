/*
 * @file        digest.hpp
 * @brief       常用摘要算法
 * @author      leize<leize@xgd.com>
 *
 * @copyright   xgd
*/

#include <base/digest.hpp>
#include <base/exception.hpp>
#include <base/file.hpp>
#include <base/logger.hpp>
#include <cstring>
#include <string>
#include <memory>

using namespace aps;

using namespace std;

Digest::Digest()
{
}

Digest::~Digest()
{
}

string Digest::fromString(string buffer)
{
	clear();
	update(buffer.c_str(),buffer.size());
	return digest();
}

string Digest::fromFile(string filePath)noexcept(false)
{
	clear();

    try
    {
        File afile(filePath.c_str(), "rb");
        FileInfo fileInfo = File::getFileInfo(filePath.c_str());

        const size_t kBufferSize = 1024*10;
        unique_ptr<char> pBuffer(new char[kBufferSize+1]);

        off_t fileSize = fileInfo.size();
        off_t readedAllBytes = 0;

        size_t perBytesToRead = kBufferSize;
        size_t perBytesReaded = 0;
        for (;readedAllBytes<fileSize;)
        {
            if ( (fileSize-readedAllBytes) < (int)kBufferSize )
                perBytesToRead = (size_t) (fileSize-readedAllBytes);
            else
                perBytesToRead = kBufferSize;

            memset(pBuffer.get(), 0x00, kBufferSize);
            perBytesReaded = afile.read(pBuffer.get(), perBytesToRead);

            update(pBuffer.get(), perBytesReaded);
            readedAllBytes += perBytesReaded;
        }
    }
    catch (Exception& e)
    {
        Z_LOG_X(eERROR) << e.what();
    }
    
    return digest();
}
