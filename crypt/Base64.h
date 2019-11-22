/**
 * @(#) Base64.h Base64加密解密算法实现
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

#ifndef BASE_64_H
#define BASE_64_H

#include "comm/BaseDef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Base64 Encode
 *
 * @param base64code The output buffer used to store the encoded base64 string
 * @param src The input string
 * @param srcLen The length of the input string
 * @return Returns the length of the encoded base64 string
 */
int base64_encode(char* base64code, const char* src, int srcLen);

/**
 * Base64 decode
 *
 * @param buf The output buffer used to store the decoded string
 * @param base64code The base64 string
 * @param srcLen The length of the base64 string
 * @return Returns the length of the decoded string
 */
int base64_decode(char* buf, const char* base64code, int srcLen);

#ifdef __cplusplus
}
#endif

#endif

