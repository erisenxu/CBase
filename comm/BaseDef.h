/*
 * @(#) BaseDef.h Created on 2014-03-27
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
 */

#ifndef BASE_DEF_H
#define BASE_DEF_H

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH    260
#endif

typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef long long S64;
typedef int S32;
typedef short S16;
typedef char S8;

/* 共享内存默认权限 */
#define SHM_DFT_ACCESS  0664

#define SOCKET_ERROR    -1

#endif
