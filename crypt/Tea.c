/**
 * @(#) Tea.c Tea加密解密算法实现
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
 * @history 2017-04-13 ErisenXu 创建文件
 */

#include "Tea.h"

#define TEA_DELTA       0x9e3779b9
#define TEA_MAX_ROUND   32

/**
 * TEA加密
 * @param pdwValue 待加密明文
 * @param pdwKey 加密密钥
 * @param pdwCrypt 加密密文
 */
static void tea_encrypt_round(S32* pdwValue, const S32* pdwKey, S32* pdwCrypt)
{
    S32 y = pdwValue[0];
    S32 z = pdwValue[1];
    S32 sum = 0;
    S32 i = 0;

    for (i = 0; i < TEA_MAX_ROUND; i++)
    {
        sum += TEA_DELTA;
        y += ((z << 4) + pdwKey[0]) ^ (z + sum) ^ ((z >> 5) + pdwKey[1]);
        z += ((y << 4) + pdwKey[2]) ^ (y + sum) ^ ((y >> 5) + pdwKey[3]);
    }

    pdwCrypt[0] = y;
    pdwCrypt[1] = z;
}

/**
 * TEA解密
 * @param pdwCrypt 待解密密文
 * @param pdwKey 解密密钥
 * @param pdwValue 解密明文
 */
static void tea_decrypt_round(S32* pdwCrypt, const S32* pdwKey, S32* pdwValue)
{
    S32 y = pdwCrypt[0];
    S32 z = pdwCrypt[1];
    S32 sum = 0xC6EF3720;
    S32 i = 0;

    for (i = 0; i < TEA_MAX_ROUND; i++)
    {
        z -= ((y << 4) + pdwKey[2]) ^ (y + sum) ^ ((y >> 5) + pdwKey[3]);
        y -= ((z << 4) + pdwKey[0]) ^ (z + sum) ^ ((z >> 5) + pdwKey[1]);
        sum -= TEA_DELTA;
    }
    pdwValue[0] = y;
    pdwValue[1] = z;
}

/**
 * TEA加密
 * @param szValue 待加密明文
 * @param len 明文长度
 * @param szCrypt 加密后密文
 * @param plenCrypt 加密后密文长度
 * @param key 加密key
 * @param mixkey 加密随机值
 */
void tea_encrypt(const char* szValue, size_t len, char* szCrypt, size_t* plenCrypt, const S32* key, const S32* mixkey)
{
    S32 value[2];
    S32 midkey[] = {mixkey[0], mixkey[1]};
    S32 i = 0;
    const char* s_ptr = szValue;
    char* e_ptr = szCrypt;
    S32 ll = 8*(len/8);

    for (i = 0; i < ll; i += 8)
    {
        M_CHAR_TO_U32(value[0], s_ptr);
        M_CHAR_TO_U32(value[1], s_ptr);

        value[0] = midkey[0] ^ value[0];
        value[1] = midkey[1] ^ value[1];

        tea_encrypt_round(value, key, midkey);

        M_U32_TO_CHAR(e_ptr, midkey[0]);
        M_U32_TO_CHAR(e_ptr, midkey[1]);
    }

    if (i < len)
    {
        char tmp[8];
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, s_ptr, len - i);

        s_ptr = tmp;

        M_CHAR_TO_U32(value[0], s_ptr);
        M_CHAR_TO_U32(value[1], s_ptr);

        value[0] = midkey[0] ^ value[0];
        value[1] = midkey[1] ^ value[1];

        tea_encrypt_round(value, key, midkey);

        M_U32_TO_CHAR(e_ptr, midkey[0]);
        M_U32_TO_CHAR(e_ptr, midkey[1]);
    }

    *plenCrypt = e_ptr - szCrypt;
}

/**
 * TEA解密
 * @param szCrypt 待解密密文
 * @param len 密文长度
 * @param szValue 解密后明文
 * @param plenValue 解密后明文长度
 * @param key 解密key
 * @param mixkey 解密随机值
 */
void tea_decrypt(const char* szCrypt, size_t len, char* szValue, size_t* plenValue, const S32* key, const S32* mixkey)
{
    S32 value[2];
    S32 tmpValue[2];
    S32 midkey[] = {mixkey[0], mixkey[1]};
    S32 i = 0;
    const char* s_ptr = szCrypt;
    char* e_ptr = szValue;
    S32 ll = 8*(len/8);

    for (i = 0; i < ll; i += 8)
    {
        M_CHAR_TO_U32(value[0], s_ptr);
        M_CHAR_TO_U32(value[1], s_ptr);

        tea_decrypt_round(value, key, tmpValue);

        midkey[0] = tmpValue[0] ^ midkey[0];
        midkey[1] = tmpValue[1] ^ midkey[1];

        M_U32_TO_CHAR(e_ptr, midkey[0]);
        M_U32_TO_CHAR(e_ptr, midkey[1]);

        midkey[0] = value[0];
        midkey[1] = value[1];
    }

    if (i < len)
    {
        char tmp[8];
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, s_ptr, len - i);

        s_ptr = tmp;

        M_CHAR_TO_U32(value[0], s_ptr);
        M_CHAR_TO_U32(value[1], s_ptr);

        tea_decrypt_round(value, key, tmpValue);

        midkey[0] = tmpValue[0] ^ midkey[0];
        midkey[1] = tmpValue[1] ^ midkey[1];

        M_U32_TO_CHAR(e_ptr, midkey[0]);
        M_U32_TO_CHAR(e_ptr, midkey[1]);

        midkey[0] = value[0];
        midkey[1] = value[1];
    }

    *plenValue = e_ptr - szValue;
}

