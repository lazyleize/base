/*
 * @file    md5.hpp
 * @brief   MD5摘要算法
 * 摘自QT源码
 * @author  Ian Jackson <ian@chiark.greenend.org.uk>
*/
#include <base/md5.hpp>

#include <cstring>
#include <cstdio>

using namespace aps;

struct MD5Context
{
	unsigned int buf[4];
	unsigned int bytes[2];
	unsigned int in[16];
};

static void MD5Init(struct MD5Context *context);
static void MD5Update(struct MD5Context *context, unsigned char const *buf, unsigned len);
static void MD5Final(struct MD5Context *context, unsigned char digest[16]);
static void MD5Transform(unsigned int buf[4], unsigned int const in[16]);

MD5Digest::MD5Digest():mConext(new MD5Context)
{
	MD5Init(mConext);
}

MD5Digest::~MD5Digest()
{
	delete mConext;
}

void MD5Digest::update(const char* buff,size_t len)
{
	MD5Update(mConext,(const unsigned char*)buff,len);
}

std::string MD5Digest::digest()
{
	char szBuffer[33];

	unsigned char digest[16];
	memset(digest,0,sizeof(digest));
	MD5Final(mConext,digest);

	for(int i = 0;i < 16;i++)
		sprintf(szBuffer + i*2,"%02x",digest[i]);

	szBuffer[32] = '\0';
	return szBuffer;
}

void MD5Digest::clear()
{
	MD5Init(mConext);
}

//////////////////////////////////////////////////////////////////////////

static void byteSwap(unsigned int* buf,unsigned words)
{
	const unsigned int byteOrderTest = 0x01;
	if(((char*)&byteOrderTest)[0] == 0)
	{
		unsigned char* p = (unsigned char*)buf;

		do
		{
			*buf++ = (unsigned int)((unsigned)p[3] << 8 | p[2]) << 16 | ((unsigned)p[1] << 8 | p[0]);
			p += 4;
		}while(--words);
	}
}

/*
* Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
* initialization constants.
*/

static void MD5Init(struct MD5Context* ctx)
{
	ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bytes[0] = 0;
    ctx->bytes[1] = 0;
}

/*
* Update context to reflect the concatenation of another buffer full
* of bytes.
*/
static void MD5Update(struct MD5Context *ctx,unsigned char const* buf,unsigned len)
{
	unsigned int t;

	/* Update byte count */
	t = ctx->bytes[0];
	if((ctx->bytes[0] = t + len) < t)
		ctx->bytes[1]++;/* Carry from low to high */

	t = 64 - (t & 0x3f);/* Space available in ctx->in (at least 1) */
	if(t > len)
	{
		memcpy((unsigned char*)ctx->in + 64 - t,buf,len);
		return ;
	}
	/* First chunk is an odd size */
	memcpy((unsigned char*)ctx->in + 64 -t,buf,t);
	byteSwap(ctx->in,16);
	MD5Transform(ctx->buf,ctx->in);
	buf += t;
	len -= t;

	/* Process data in 64-byte chunks */
	while(len >= 64)
	{
		memcpy(ctx->in,buf,64);
		byteSwap(ctx->in,16);
		MD5Transform(ctx->buf,ctx->in);
		buf += 64;
		len -= 64;
	}

	/* Handle any remaining bytes of data. */
	memcpy(ctx->in,buf,len);
}


/*
* Final wrapup - pad to 64-byte boundary with the bit pattern 
* 1 0* (64-bit count of bits processed, MSB-first)
*/
static void MD5Final(struct MD5Context* ctx,unsigned char digest[16])
{
	int count = ctx->bytes[0] & 0x3f;/* Number of bytes in ctx->in */
	unsigned char *p = (unsigned char*)ctx->in + count;

	/* Set the first char of padding to 0x80.  There is always room. */
    *p++ = 0x80;

	/* Bytes of padding needed to make 56 bytes (-8..55) */
    count = 56 - 1 - count;

	if(count < 0)/* Padding forces an extra block */
	{
		memset(p,0,count + 8);
		byteSwap(ctx->in,16);
		MD5Transform(ctx->buf,ctx->in);
		p = (unsigned char*)ctx->in;
		count = 56;
	}
	memset(p,0,count);
	byteSwap(ctx->in,14);

	/* Append length in bits and transform */
	ctx->in[14] = ctx->bytes[0] << 3;
	ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
	MD5Transform(ctx->buf,ctx->in);

	byteSwap(ctx->buf,4);
	memcpy(digest,ctx->buf,16);
	memset(ctx,0,sizeof(MD5Context));/* In case it's sensitive */
}

#ifndef ASM_MD5

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
    (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
* The core of the MD5 algorithm, this alters an existing MD5 hash to
* reflect the addition of 16 longwords of new data.  MD5Update blocks
* the data and converts bytes into longwords for this routine.
*/
static void MD5Transform(unsigned int buf[4], unsigned int const in[16])
{
    register unsigned int a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

#endif

/* MD5 算法介绍 

    MD5的全称是Message-Digest Algorithm 5，在90年代初由MIT的计算机科学实验室
 和RSA Data Security Inc发明，经MD2、MD3和MD4发展而来。Message-Digest泛指字节串
(Message)的Hash变换，就是把一个任意长度的字节串变换成一定长的大整数。请注意
我使用了“字节串”而不是“字符串”这个词，是因为这种变换只与字节的值有关，与
字符集或编码方式无关。MD5将任意长度的“字节串”变换成一个128bit的大整数，并且
它是一个不可逆的字符串变换算法。换句话说就是，即使你看到源程序和算法描述，也
无法将一个MD5的值变换回原始的字符串，从数学原理上说，是因为原始的字符串有无穷
多个，这有点象不存在反函数的数学函数。 

    MD5的典型应用是对一段Message(字节串)产生fingerprint(指纹)，以防止被“篡改”。
举个例子，你将一段话写在一个叫readme.txt文件中，并对这个readme.txt产生一个MD5的
值并记录在案，然后你可以传播这个文件给别人，别人如果修改了文件中的任何内容，你对
这个文件重新计算MD5时就会发现。如果再有一个第三方的认证机构，用MD5还可以防止文件
作者的“抵赖”，这就是所谓的数字签名应用。 

    MD5还广泛用于加密和解密技术上，在很多操作系统中，用户的密码是以MD5值（或类似
的其它算法）的方式保存的，用户Login的时候，系统是把用户输入的密码计算成MD5值，然
后再去和系统中保存的MD5值进行比较，而系统并不“知道”用户的密码是什么。 

MD5算法： 

第一步：增加填充 

    增加padding使得数据长度（bit为单位）模512为448。
    如果数据长度正好是模512为448，增加512个填充bit，也就是说填充的个数为1-512。
    第一个bit为1，其余全部为0。(即第一个字节为0x80)

第二步：补足长度 

    将数据长度转换为64bit的数值，如果长度超过64bit所能表示的数据长度的范围，值保留最后64bit，
    增加到前面填充的数据后面，使得最后的数据为512bit的整数倍。也就是32bit的16倍的整数倍。
    在RFC1321中，32bit称为一个word。 

第三步：初始化变量： 

    用到4个变量，分别为A、B、C、D，均为32bit长。初始化为： 
    A: 01 23 45 67 
    B: 89 ab cd ef 
    C: fe dc ba 98 
    D: 76 54 32 10 

第四步：数据处理： 

    首先定义4个辅助函数： 
    F(X,Y,Z) = XY v not(X) Z 
    G(X,Y,Z) = XZ v Y not(Z) 
    H(X,Y,Z) = X xor Y xor Z 
    I(X,Y,Z) = Y xor (X v not(Z)) 
    其中：XY表示按位与，X v Y表示按位或，not(X)表示按位取反。xor表示按位异或。 
    函数中的X、Y、Z均为32bit。 
    定义一个需要用到的数组：T(i),i取值1-64,T(i)等于abs(sin(i))的4294967296倍的整数部分,i为弧度。 

第五步：输出： 

    最后得到的ABCD为输出结果，共128bit。A为低位，D为高位。 

补充说明：

  MD5使用四个32位整数作为计算基础和计算结果，因此消息摘要长度=4x32=128位
  MD5每次处理16个32位整数,分四个圈替换完成,块大小为16x32/8=64字节,即64x8=512位
  如果输入的长度不足64字节,则需要填充0x80,0x00,0x00,0x00..,其中最后一个块的14(56 57 58 59)
    15(60 61 62 63)字节分别为输入消息的总位数=字节数x8,因此最后一个块的填充长度=64-8=56-前次
      剩余的长度.如果前次剩余长度>56,则多添一个块。
  最后由于主机cpu可能是低位在前,也可能是高位在前,因此需要转换成低位在前,计算结果再转换回来.

    A B C D       D C B A
    1 2 3 4  ---> 4 3 2 1
   ---------     ---------
    A1B2C3D4      D4C3B2A1 --> A1B2C3D4
*/