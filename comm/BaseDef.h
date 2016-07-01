/*
 * @(#) BaseDef.h Created on 2014-03-27
 *
 * Copyright (c) 2014-2054 Erisen Xu(徐勇) All Rights Reserved
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

#endif
