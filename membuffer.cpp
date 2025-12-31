/*
* @file        memBuffer.cpp
* @brief       原始内存块简单封装
* @author      leize<leize@xgd.com>
*
* @copyright   Copyright (c) 2021 xgd
*/
#include <base/membuffer.hpp>

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <stdarg.h>

using namespace aps;

MemBuffer::MemBuffer():pBuffer_(NULL),pOrigBuffer_(NULL),nMisalign_(0),nTotalLength_(0),nOff_(0)
{
	_init(256);
}

void MemBuffer::_init(size_t preAlloc)
{
	pBuffer_ = NULL;
	pOrigBuffer_ = NULL;
	nMisalign_ = 0;
	nTotalLength_ = 0;
	nOff_ = 0;

	if(preAlloc <= 0)
		preAlloc = 256;

	pBuffer_ = pOrigBuffer_ = (char*)calloc(1,preAlloc);
	assert(NULL!=pOrigBuffer_);
	nTotalLength_ = preAlloc;
}

MemBuffer::MemBuffer(const MemBuffer& buf)
{
	size_t length = 256;
	if(buf.size()>length)
		length = buf.size();
	_init(length);
	append(buf.getRawBuffer(),buf.size());
}

MemBuffer& MemBuffer::operator=(const MemBuffer& buf)
{
	if(buf.size()>0)
	{
		if(&buf == this)
			return *this;
		else
		{
			pBuffer_ = pOrigBuffer_;
			nOff_ = 0;
			append(buf.getRawBuffer(),buf.size());
		}
	}
	else
	{
		pBuffer_ = pOrigBuffer_;
		nOff_ = 0;
	}
	return *this;
}

MemBuffer::~MemBuffer()
{
	assert(NULL!=pOrigBuffer_);
	free(pOrigBuffer_);
}

void MemBuffer::align()
{
	memmove(pOrigBuffer_,pBuffer_,nOff_);
	pBuffer_ = pOrigBuffer_;
	nMisalign_ = 0;
}

int MemBuffer::more(size_t datlen)
{
	size_t need = nMisalign_ + nOff_ + datlen;
	if(nTotalLength_ >= need)
		return 0;

	if(nMisalign_ >= datlen)
		align();
	else
	{
		void *newbuf;
		size_t length = nTotalLength_;

		if(length < 512)
			length = 512;
		while(length < need)
			length <<= 1;

		if(pOrigBuffer_ != pBuffer_)
			align();

		newbuf = realloc(pBuffer_,length);
		assert(newbuf!=NULL);
		if(newbuf==NULL)
			return -1;
		pOrigBuffer_ = pBuffer_ = (char*)newbuf;
		nTotalLength_ = length;
		nMisalign_ = 0;
	}

	return  0;
}

char* MemBuffer::getBuffer()
{
	more(1);
	pBuffer_[nOff_] = '\0';
	return pBuffer_;
}

void * MemBuffer::getRawBuffer()const
{
	return pBuffer_;
}

const size_t& MemBuffer::size()const
{
	return nOff_;
}

size_t MemBuffer::totalMemory()
{
	return nTotalLength_;
}

static void dumpBinaryData(unsigned char* in,int len);

void MemBuffer::dumpInfo(size_t dumpCount)
{
	printf("CMemBuffer::dumpInfo \n");
    printf("=======================\n");
    printf("    Size =% -5d", (int)nOff_);
    printf("    Free =% -5d", (int)(nTotalLength_-nOff_));
    printf("   Total =% -5d", (int)nTotalLength_);
    printf("   align =% -5d\n", (int)nMisalign_);

	if(dumpCount>nOff_)
		dumpCount = nOff_;

	if(nOff_>0)
	{
		printf("--------------------------------------------------------------------------\n");
        dumpBinaryData((unsigned char*)pBuffer_, dumpCount);
        printf("--------------------------------------------------------------------------\n\n");
    }
	else
		printf("\n");
}

int MemBuffer::append(const void * data,size_t datlen)
{
	int r = more(datlen);
	memcpy(pBuffer_ + nOff_,data,datlen);
	nOff_ += datlen;
	return r;
}

int MemBuffer::append(const string& str)
{
	int r = 0;
	if(str.length()>0)
		r = append(str.c_str(),str.length());
	return r;
}

int MemBuffer::append(MemBuffer& buf)
{
	if(buf.size()>0)
	{
		if(&buf == this)
		{
			more(buf.size());
			append(buf.getRawBuffer(),buf.size());
		}
		else
			append(buf.getRawBuffer(),buf.size());
	}
	return 0;
}

//[[0...misalign][0...off] ...totalLen]
void MemBuffer::reserve(size_t size)
{
    if (nTotalLength_<size)
    {
        more(size - nOff_);
    }
    //if (m_pBuffer!=m_pOrigBuffer)
    //    align();
}

void MemBuffer::reset()
{
	pBuffer_ = pOrigBuffer_;
	nMisalign_ = 0;
	nOff_ = 0;
	memset(pBuffer_,0x00,nTotalLength_);
}

void MemBuffer::truncate(size_t len)
{
	int space = nTotalLength_ - nMisalign_;
	if((int)len<=space)
		nOff_ = len;
	else
	{
		more(len);
		nOff_ = len;
	}
}

int MemBuffer::format(const char* fmt,...)
{
	int r = 0;
    if( NULL != strchr( fmt, '%' ) )
    {
        char *ret1 = NULL;
        char **ret = &ret1;

        va_list args;
        char *buf = NULL;
        size_t bufsize = 0;
        char *newbuf = NULL;
        size_t nextsize = 0;
        int outsize = 0;

        bufsize = 0;
        for (;;)
        {
            if (bufsize == 0) 
            {
                if ((buf = (char *)malloc(512)) == NULL) 
                {
                    *ret = NULL;
                    return -1;
                }
                bufsize = 512;
            } 
            else if ((newbuf = (char *)realloc(buf, nextsize)) != NULL) 
            {
                buf = newbuf;
                bufsize = nextsize;
            } 
            else
            {
                free(buf);
                *ret = NULL;
                return -1;
            }

            va_start(args, fmt);
            outsize = vsnprintf(buf, bufsize, fmt, args);
            va_end(args);

            if (outsize == -1)
            {
                // 只能瞎猜大小 
                nextsize = bufsize * 2;
            } 
            else if (outsize == (int)bufsize)
            {
                // 被截断, 大小亦不知道
                nextsize = bufsize * 2;
            } 
            else if (outsize > (int)bufsize) 
            {
                // 这个还好, 已经知道需要多大内存
                nextsize = outsize + 2;
            } 
            else if (outsize == (int)bufsize - 1)
            {
                // 在用些系统上不明确(不明白...)
                nextsize = bufsize * 2;
            } 
            else
            {
                // 这样最好
                break;
            }
        }
        *ret = buf;

        if (ret1!=NULL)
        {
            r = append(ret1, outsize);
            free(ret1);
            ret1 = NULL;
        }
    }
    else 
    {
        r = append( fmt, strlen(fmt));
    }
    return r;
}

int MemBuffer::saveToFile(const string filePath, bool bappend) const
{
	FILE *fp = NULL;
	if(bappend)
	{
		fp = fopen(filePath.c_str(),"ab");
		if(NULL == fp)
		{
			fp = fopen(filePath.c_str(),"wb");
			if(fp==NULL)
				return -1;
		}
	}
	else
	{
		fp = fopen(filePath.c_str(),"wb");
		if(fp == NULL)
			return -1;
	}

	int rc = 0;
	if(nOff_ > 0)
		rc = fwrite(pBuffer_,sizeof(char),nOff_,fp);
	fclose(fp);

	if(rc!=(int)nOff_)
		return -1;
	return 0;
}

int MemBuffer::loadFromFile(const string filePath,size_t nOffset/*=0*/, size_t nCount/*=0*/)
{
	reset();

    FILE *fp = NULL;
    fp = fopen(filePath.c_str(), "rb");
    if (NULL==fp)
        return -1;

    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);

    if (nOffset>=fileSize)
    {
        fclose(fp);
        return -2;
    }

    size_t toReadSize = fileSize;
    if(nCount>0)
        toReadSize = nCount;
    if (fileSize-nOffset<toReadSize)
        toReadSize = fileSize - nOffset;

    reserve(toReadSize+1);

    size_t leftBytes = toReadSize;
    int perReadBytes = 0;
    int perReadedBytes = 0;

    fseek(fp, nOffset, SEEK_SET);
    for(; leftBytes>0;)
    {
        if (leftBytes<1024)
            perReadBytes = leftBytes;
        else
            perReadBytes = 1024;
        perReadedBytes = fread(pBuffer_+(toReadSize-leftBytes), 1, perReadBytes, fp );
        leftBytes -= perReadedBytes;
        nOff_ += perReadedBytes;
    }
    fclose(fp);
    pBuffer_[nOff_] = '\0';
    return toReadSize;
}

void dumpBinaryData(unsigned char* in,int len)
{
	int cur = 0;
    int row_cnt = 0;
    const int countPerRow = 20;
    int rightPrintedCount = 0;

    printf("|% 3d:",row_cnt++);
    do {
        if (cur % countPerRow == 0 && cur > 0)
        {
            printf("  |  ");
            unsigned char*p = in + cur -countPerRow;
            rightPrintedCount += countPerRow;
            for (int i=0; i<countPerRow; i++)
            {
                if (isgraph(p[i]))
                    printf("%c", p[i]);
                else
                    printf(".");
            }
            printf("\n|% 3d:",countPerRow*row_cnt++);
        } 
        printf(" %02x",in[cur]);
        cur++;
    } while(cur < len);

    int leftCount = countPerRow - len%countPerRow;
    if (leftCount != countPerRow)
    {
        for (int i=0; i<leftCount; i++)
        {
            printf(" **");
        }
    }

    leftCount = len - rightPrintedCount;
    if (leftCount != 0)
    {
        printf("  |  ");
        unsigned char*pLeft = in + len -leftCount;
        for (int i=0; i<leftCount; i++)
        {
            if (isgraph(pLeft[i]))
                printf("%c", pLeft[i]);
            else
                printf(".");
        }
    }

    printf("\n");
}