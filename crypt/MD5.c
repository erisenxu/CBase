/**
 * @(#) MD5.c MD5算法实现
 *
 * Copyright (c) 2014-2016 Erisen Xu (@itfriday)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author Erisen Xu
 * @version 1.0
 * @history 2017-04-23 ErisenXu 创建文件
 */

#include "MD5.h"
#include <string.h>
#include <stdio.h>

// Magic initialization constants
#define MD5_INIT_STATE_0 0x67452301
#define MD5_INIT_STATE_1 0xefcdab89
#define MD5_INIT_STATE_2 0x98badcfe
#define MD5_INIT_STATE_3 0x10325476

// Constants for Transform routine.
#define MD5_S11  7
#define MD5_S12 12
#define MD5_S13 17
#define MD5_S14 22
#define MD5_S21  5
#define MD5_S22  9
#define MD5_S23 14
#define MD5_S24 20
#define MD5_S31  4
#define MD5_S32 11
#define MD5_S33 16
#define MD5_S34 23
#define MD5_S41  6
#define MD5_S42 10
#define MD5_S43 15
#define MD5_S44 21

// Transformation Constants - Round 1
#define MD5_T01  0xd76aa478 //Transformation Constant 1
#define MD5_T02  0xe8c7b756 //Transformation Constant 2
#define MD5_T03  0x242070db //Transformation Constant 3
#define MD5_T04  0xc1bdceee //Transformation Constant 4
#define MD5_T05  0xf57c0faf //Transformation Constant 5
#define MD5_T06  0x4787c62a //Transformation Constant 6
#define MD5_T07  0xa8304613 //Transformation Constant 7
#define MD5_T08  0xfd469501 //Transformation Constant 8
#define MD5_T09  0x698098d8 //Transformation Constant 9
#define MD5_T10  0x8b44f7af //Transformation Constant 10
#define MD5_T11  0xffff5bb1 //Transformation Constant 11
#define MD5_T12  0x895cd7be //Transformation Constant 12
#define MD5_T13  0x6b901122 //Transformation Constant 13
#define MD5_T14  0xfd987193 //Transformation Constant 14
#define MD5_T15  0xa679438e //Transformation Constant 15
#define MD5_T16  0x49b40821 //Transformation Constant 16

// Transformation Constants - Round 2
#define MD5_T17  0xf61e2562 //Transformation Constant 17
#define MD5_T18  0xc040b340 //Transformation Constant 18
#define MD5_T19  0x265e5a51 //Transformation Constant 19
#define MD5_T20  0xe9b6c7aa //Transformation Constant 20
#define MD5_T21  0xd62f105d //Transformation Constant 21
#define MD5_T22  0x02441453 //Transformation Constant 22
#define MD5_T23  0xd8a1e681 //Transformation Constant 23
#define MD5_T24  0xe7d3fbc8 //Transformation Constant 24
#define MD5_T25  0x21e1cde6 //Transformation Constant 25
#define MD5_T26  0xc33707d6 //Transformation Constant 26
#define MD5_T27  0xf4d50d87 //Transformation Constant 27
#define MD5_T28  0x455a14ed //Transformation Constant 28
#define MD5_T29  0xa9e3e905 //Transformation Constant 29
#define MD5_T30  0xfcefa3f8 //Transformation Constant 30
#define MD5_T31  0x676f02d9 //Transformation Constant 31
#define MD5_T32  0x8d2a4c8a //Transformation Constant 32

// Transformation Constants - Round 3
#define MD5_T33  0xfffa3942 //Transformation Constant 33
#define MD5_T34  0x8771f681 //Transformation Constant 34
#define MD5_T35  0x6d9d6122 //Transformation Constant 35
#define MD5_T36  0xfde5380c //Transformation Constant 36
#define MD5_T37  0xa4beea44 //Transformation Constant 37
#define MD5_T38  0x4bdecfa9 //Transformation Constant 38
#define MD5_T39  0xf6bb4b60 //Transformation Constant 39
#define MD5_T40  0xbebfbc70 //Transformation Constant 40
#define MD5_T41  0x289b7ec6 //Transformation Constant 41
#define MD5_T42  0xeaa127fa //Transformation Constant 42
#define MD5_T43  0xd4ef3085 //Transformation Constant 43
#define MD5_T44  0x04881d05 //Transformation Constant 44
#define MD5_T45  0xd9d4d039 //Transformation Constant 45
#define MD5_T46  0xe6db99e5 //Transformation Constant 46
#define MD5_T47  0x1fa27cf8 //Transformation Constant 47
#define MD5_T48  0xc4ac5665 //Transformation Constant 48

// Transformation Constants - Round 4
#define MD5_T49  0xf4292244 //Transformation Constant 49
#define MD5_T50  0x432aff97 //Transformation Constant 50
#define MD5_T51  0xab9423a7 //Transformation Constant 51
#define MD5_T52  0xfc93a039 //Transformation Constant 52
#define MD5_T53  0x655b59c3 //Transformation Constant 53
#define MD5_T54  0x8f0ccc92 //Transformation Constant 54
#define MD5_T55  0xffeff47d //Transformation Constant 55
#define MD5_T56  0x85845dd1 //Transformation Constant 56
#define MD5_T57  0x6fa87e4f //Transformation Constant 57
#define MD5_T58  0xfe2ce6e0 //Transformation Constant 58
#define MD5_T59  0xa3014314 //Transformation Constant 59
#define MD5_T60  0x4e0811a1 //Transformation Constant 60
#define MD5_T61  0xf7537e82 //Transformation Constant 61
#define MD5_T62  0xbd3af235 //Transformation Constant 62
#define MD5_T63  0x2ad7d2bb //Transformation Constant 63
#define MD5_T64  0xeb86d391 //Transformation Constant 64

// Null data (except for first BYTE) used to finalise the checksum calculation
static U8 MD5_PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Rotates the bits in a 32 bit U32 left by a specified amount
 */
static U32 rotateLeft(U32 x, int n)
{
    return (x << n) | (x >> (32 - n));
}

/**
 * Implementation of basic MD5 transformation algorithm
 */
static void FF(U32* a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 t)
{
    U32 f = (b & c) | (~b & d);
    *a += f + x + t;
    *a = rotateLeft(*a, s);
    *a += b;
}

/**
 * Implementation of basic MD5 transformation algorithm
 */
static void GG(U32* a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 t)
{
    U32 g = (b & d) | (c & ~d);
    *a += g + x + t;
    *a = rotateLeft(*a, s);
    *a += b;
}

/**
 * Implementation of basic MD5 transformation algorithm
 */
static void HH(U32* a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 t)
{
    U32 h = (b ^ c ^ d);
    *a += h + x + t;
    *a = rotateLeft(*a, s);
    *a += b;
}

/**
 * Implementation of basic MD5 transformation algorithm
 */
static void II(U32* a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 t)
{
    U32 i = (c ^ (b | ~d));
    *a += i + x + t;
    *a = rotateLeft(*a, s);
    *a += b;
}

/**
 * Transfers the data in an 8 bit array to a 32 bit array
 * NOTES: Four BYTES from the input array are transferred to each U32 entry
 * of the output array. The first U8 is transferred to the bits (0-7)
 * of the output U32, the second U8 to bits 8-15 etc.
 * The algorithm assumes that the input array is a multiple of 4 bytes long
 * so that there is a perfect fit into the array of 32 bit words.
 */
static void byte_to_dword(U32* output, const U8* input, U32 nLength)
{
    // initialisations
    U32 i = 0;	// index to Output array
    U32 j = 0;	// index to Input array

    // transfer the data by shifting and copying
    for (; j < nLength; i++, j += 4)
    {
        output[i] = (U32)input[j] |
            (U32)input[j + 1] << 8 |
            (U32)input[j + 2] << 16 |
            (U32)input[j + 3] << 24;
    }
}

/*
 * Transfers the data in an 32 bit array to a 8 bit array
 * One U32 from the input array is transferred into four BYTES
 * in the output array. The first (0-7) bits of the first U32 are
 * transferred to the first output U8, bits bits 8-15 are transferred from
 * the second U8 etc.
 *
 * The algorithm assumes that the output array is a multiple of 4 bytes long
 * so that there is a perfect fit of 8 bit BYTES into the 32 bit DWORDs.
 */
static void dword_to_byte(U8* output, U32* input, U32 nLength )
{
    // transfer the data by shifting and copying
    U32 i = 0;
    U32 j = 0;
    for (; j < nLength; i++, j += 4)
    {
        output[j] =   (U8)(input[i] & 0xff);
        output[j+1] = (U8)((input[i] >> 8) & 0xff);
        output[j+2] = (U8)((input[i] >> 16) & 0xff);
        output[j+3] = (U8)((input[i] >> 24) & 0xff);
    }
}

/**
 * MD5 basic transformation algorithm; transforms 'lMD5'
 * NOTES: An MD5 checksum is calculated by four rounds of 'Transformation'.
 * The MD5 checksum currently held in m_lMD5 is merged by the
 * transformation process with data passed in 'Block'.
 */
static void transform(const U8 block[64], U32 lMD5[4])
{
    // initialise local data with current checksum
    U32 a = lMD5[0];
    U32 b = lMD5[1];
    U32 c = lMD5[2];
    U32 d = lMD5[3];

    // copy BYTES from input 'Block' to an array of ULONGS 'X'
    U32 X[16];

    byte_to_dword(X, block, 64);

    // Perform Round 1 of the transformation
    FF(&a, b, c, d, X[ 0], MD5_S11, MD5_T01);
    FF(&d, a, b, c, X[ 1], MD5_S12, MD5_T02);
    FF(&c, d, a, b, X[ 2], MD5_S13, MD5_T03);
    FF(&b, c, d, a, X[ 3], MD5_S14, MD5_T04);
    FF(&a, b, c, d, X[ 4], MD5_S11, MD5_T05);
    FF(&d, a, b, c, X[ 5], MD5_S12, MD5_T06);
    FF(&c, d, a, b, X[ 6], MD5_S13, MD5_T07);
    FF(&b, c, d, a, X[ 7], MD5_S14, MD5_T08);
    FF(&a, b, c, d, X[ 8], MD5_S11, MD5_T09);
    FF(&d, a, b, c, X[ 9], MD5_S12, MD5_T10);
    FF(&c, d, a, b, X[10], MD5_S13, MD5_T11);
    FF(&b, c, d, a, X[11], MD5_S14, MD5_T12);
    FF(&a, b, c, d, X[12], MD5_S11, MD5_T13);
    FF(&d, a, b, c, X[13], MD5_S12, MD5_T14);
    FF(&c, d, a, b, X[14], MD5_S13, MD5_T15);
    FF(&b, c, d, a, X[15], MD5_S14, MD5_T16);

    // Perform Round 2 of the transformation
    GG(&a, b, c, d, X[ 1], MD5_S21, MD5_T17);
    GG(&d, a, b, c, X[ 6], MD5_S22, MD5_T18);
    GG(&c, d, a, b, X[11], MD5_S23, MD5_T19);
    GG(&b, c, d, a, X[ 0], MD5_S24, MD5_T20);
    GG(&a, b, c, d, X[ 5], MD5_S21, MD5_T21);
    GG(&d, a, b, c, X[10], MD5_S22, MD5_T22);
    GG(&c, d, a, b, X[15], MD5_S23, MD5_T23);
    GG(&b, c, d, a, X[ 4], MD5_S24, MD5_T24);
    GG(&a, b, c, d, X[ 9], MD5_S21, MD5_T25);
    GG(&d, a, b, c, X[14], MD5_S22, MD5_T26);
    GG(&c, d, a, b, X[ 3], MD5_S23, MD5_T27);
    GG(&b, c, d, a, X[ 8], MD5_S24, MD5_T28);
    GG(&a, b, c, d, X[13], MD5_S21, MD5_T29);
    GG(&d, a, b, c, X[ 2], MD5_S22, MD5_T30);
    GG(&c, d, a, b, X[ 7], MD5_S23, MD5_T31);
    GG(&b, c, d, a, X[12], MD5_S24, MD5_T32);

    // Perform Round 3 of the transformation
    HH(&a, b, c, d, X[ 5], MD5_S31, MD5_T33);
    HH(&d, a, b, c, X[ 8], MD5_S32, MD5_T34);
    HH(&c, d, a, b, X[11], MD5_S33, MD5_T35);
    HH(&b, c, d, a, X[14], MD5_S34, MD5_T36);
    HH(&a, b, c, d, X[ 1], MD5_S31, MD5_T37);
    HH(&d, a, b, c, X[ 4], MD5_S32, MD5_T38);
    HH(&c, d, a, b, X[ 7], MD5_S33, MD5_T39);
    HH(&b, c, d, a, X[10], MD5_S34, MD5_T40);
    HH(&a, b, c, d, X[13], MD5_S31, MD5_T41);
    HH(&d, a, b, c, X[ 0], MD5_S32, MD5_T42);
    HH(&c, d, a, b, X[ 3], MD5_S33, MD5_T43);
    HH(&b, c, d, a, X[ 6], MD5_S34, MD5_T44);
    HH(&a, b, c, d, X[ 9], MD5_S31, MD5_T45);
    HH(&d, a, b, c, X[12], MD5_S32, MD5_T46);
    HH(&c, d, a, b, X[15], MD5_S33, MD5_T47);
    HH(&b, c, d, a, X[ 2], MD5_S34, MD5_T48);

    // Perform Round 4 of the transformation
    II(&a, b, c, d, X[ 0], MD5_S41, MD5_T49);
    II(&d, a, b, c, X[ 7], MD5_S42, MD5_T50);
    II(&c, d, a, b, X[14], MD5_S43, MD5_T51);
    II(&b, c, d, a, X[ 5], MD5_S44, MD5_T52);
    II(&a, b, c, d, X[12], MD5_S41, MD5_T53);
    II(&d, a, b, c, X[ 3], MD5_S42, MD5_T54);
    II(&c, d, a, b, X[10], MD5_S43, MD5_T55);
    II(&b, c, d, a, X[ 1], MD5_S44, MD5_T56);
    II(&a, b, c, d, X[ 8], MD5_S41, MD5_T57);
    II(&d, a, b, c, X[15], MD5_S42, MD5_T58);
    II(&c, d, a, b, X[ 6], MD5_S43, MD5_T59);
    II(&b, c, d, a, X[13], MD5_S44, MD5_T60);
    II(&a, b, c, d, X[ 4], MD5_S41, MD5_T61);
    II(&d, a, b, c, X[11], MD5_S42, MD5_T62);
    II(&c, d, a, b, X[ 2], MD5_S43, MD5_T63);
    II(&b, c, d, a, X[ 9], MD5_S44, MD5_T64);

    // add the transformed values to the current checksum
    lMD5[0] += a;
    lMD5[1] += b;
    lMD5[2] += c;
    lMD5[3] += d;
}

/**
 * Implementation of main MD5 checksum algorithm
 */
static void do_encrypt(const U8* input, U32 nInputLen,
                       U8* lpszBuffer, U32 nCount[2],
                       U32 lMD5[4])
{
    // Compute number of bytes mod 64
    U32 nIndex = (U32)((nCount[0] >> 3) & 0x3F);

    // Update number of bits
    if ((nCount[0] += nInputLen << 3) < (nInputLen << 3))
    {
        nCount[1]++;
    }

    nCount[1] += (nInputLen >> 29);

    // Transform as many times as possible.
    U32 i = 0;
    U32 nPartLen = 64 - nIndex;

    if (nInputLen >= nPartLen)
    {
        memcpy(&lpszBuffer[nIndex], input, nPartLen);
        transform(lpszBuffer, lMD5);
        for (i = nPartLen; i + 63 < nInputLen; i += 64)
        {
            transform(&input[i], lMD5);
        }
        nIndex = 0;
    }
    else
    {
        i = 0;
    }

    // Buffer remaining input
    memcpy(&lpszBuffer[nIndex], &input[i], nInputLen - i);
}

/**
 * Implementation of main MD5 checksum algorithm; ends the checksum calculation.
 * the final hexadecimal MD5 checksum result
 * Performs the final MD5 checksum calculation ('Update' does most of the work,
 * this function just finishes the calculation.)
 */
static void cal_md5(char* strMD52, U8* lpszBuffer, U32 nCount[2],
                    U32 lMD5[4])
{
    // Save number of bits
    U8 bits[8];
    int i;
    dword_to_byte(bits, nCount, 8);

    // Pad out to 56 mod 64.
    U32 nIndex = (U32)((nCount[0] >> 3) & 0x3f);
    U32 nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
    do_encrypt(MD5_PADDING, nPadLen, lpszBuffer, nCount, lMD5);

    // Append length (before padding)
    do_encrypt(bits, 8, lpszBuffer, nCount, lMD5);

    // Store final state in 'lpszMD5'
    const int nMD5Size = 16;
    unsigned char lpszMD5[nMD5Size];
    dword_to_byte(lpszMD5, lMD5, nMD5Size);

    // Convert the hexadecimal checksum to a CString
    for (i = 0; i < nMD5Size; i++)
    {
        char str[3];
#if 0
        if (lpszMD5[i] == 0) {
            strcpy(str, "00");
        }
        else if (lpszMD5[i] <= 15) 	{
            sprintf(str, "0%x", lpszMD5[i]);
        }
        else {
            sprintf(str, "%x", lpszMD5[i]);
        }
#else
        sprintf(str, "%02x", lpszMD5[i]);
#endif
        //ASSERT(str.GetLength() == 2);
        strcat(strMD52, str);
    }
}

/**
 * MD5 Encryption
 *
 * @param output The output buffer used to store the encryption MD5 string
 * @param pBuf The input string
 * @param nLength The length of the input string
 * @return Returns the output buffer
 */
char* md5_encrypt(char* output, const U8* pBuf, U32 nLength)
{
    U8 lpszBuffer[64]; // input buffer
    U32 nCount[2];     // number of bits, modulo 2^64 (lsb first)
    U32 lMD5[4];       // MD5 checksum

    // zero members
    memset(lpszBuffer, 0, sizeof(lpszBuffer));
    nCount[0] = nCount[1] = 0;

    // Load magic state initialization constants
    lMD5[0] = MD5_INIT_STATE_0;
    lMD5[1] = MD5_INIT_STATE_1;
    lMD5[2] = MD5_INIT_STATE_2;
    lMD5[3] = MD5_INIT_STATE_3;

    do_encrypt(pBuf, nLength, lpszBuffer, nCount, lMD5);
    cal_md5(output, lpszBuffer, nCount, lMD5);

    return output;
}

