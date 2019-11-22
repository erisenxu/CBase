/*
 * @(#) MD5Test.c Created on 2017-04-23
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */


#include "MD5Test.h"
#include "crypt/MD5.h"
#include <stdio.h>
#include <string.h>

void test_md5(const char* szBuf, int len)
{
    char out[64];
    memset(out, 0, sizeof(out));
    md5_encrypt(out, szBuf, len);
    printf("%s\n", out);
}
