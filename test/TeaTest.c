/*
 * @(#) TeaTest.c Created on 2017-04-23
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */


#include "TeaTest.h"
#include "crypt/Tea.h"
#include <stdio.h>
#include <string.h>

void test_tea_encrypt()
{
    U64 ullSmsToken = 4756660489819997782l;
    S32 dwSmsCode = 55651;
    S32 dwKey1 = (S32)(ullSmsToken >> 32);
    S32 dwKey2 = (S32)ullSmsToken;
    S32 dwKey3 = dwKey1 ^ dwSmsCode;
    S32 dwKey4 = dwKey2 ^ dwSmsCode;
    S32 key[] = {dwKey1, dwKey2, dwKey3, dwKey4};
    S32 mixkey[] = {dwKey3, dwKey4};
    char szDecryptValue[1024];
    size_t dwCryptLen = 0;

    char szCrypt[1024];
    size_t dwDecryptValueLen = 0;

    S32 i;

    char szValue[] = {
        (char)0x00, (char)0x01, (char)0x0b, (char)0x00, (char)0x00, (char)0x00, (char)0x33, (char)0x00,
        (char)0x01, (char)0x09, (char)0x00, (char)0x00, (char)0x00, (char)0x05, (char)0x35, (char)0x35,
        (char)0x36, (char)0x35, (char)0x31, (char)0x00, (char)0x02, (char)0x09, (char)0x00, (char)0x00,
        (char)0x00, (char)0x20, (char)0x63, (char)0x35, (char)0x35, (char)0x31, (char)0x32, (char)0x38,
        (char)0x39, (char)0x38, (char)0x35, (char)0x64, (char)0x31, (char)0x30, (char)0x64, (char)0x35,
        (char)0x37, (char)0x63, (char)0x64, (char)0x39, (char)0x30, (char)0x39, (char)0x62, (char)0x64,
        (char)0x64, (char)0x33, (char)0x30, (char)0x38, (char)0x30, (char)0x64, (char)0x33, (char)0x34,
        (char)0x37, (char)0x34
    };

    tea_encrypt(szValue, sizeof(szValue)/sizeof(char), szCrypt, &dwCryptLen, key, mixkey);

    printf("++++++++++++++++++++%ld\n", sizeof(szValue)/sizeof(char));

    for (i = 0; i < dwCryptLen; i++)
    {
        if (i > 0 && i%16 == 0) printf("\n");
        printf("%02x ", (U8)szCrypt[i]);
    }

    printf("\n");

    tea_decrypt(szCrypt, dwCryptLen, szDecryptValue, &dwDecryptValueLen, key, mixkey);

    printf("++++++++++++++++++++\n");

    for (i = 0; i < dwDecryptValueLen; i++)
    {
        if (i > 0 && i%16 == 0) printf("\n");
        printf("%02x ", (U8)szDecryptValue[i]);
    }
    printf("\n");
}

void test_tea_decrypt()
{
    U64 ullSmsToken = 4756660489819997782l;
    S32 dwSmsCode = 55651;
    S32 dwKey1 = (S32)(ullSmsToken >> 32);
    S32 dwKey2 = (S32)ullSmsToken;
    S32 dwKey3 = dwKey1 ^ dwSmsCode;
    S32 dwKey4 = dwKey2 ^ dwSmsCode;
    S32 key[] = {dwKey1, dwKey2, dwKey3, dwKey4};
    S32 mixkey[] = {dwKey3, dwKey4};
    char szValue[1024];
    size_t dwValueLen = 0;
    S32 i;

    char szCrypt[] = {
        (char)0xc5, (char)0x02, (char)0x68, (char)0x56, (char)0x0c, (char)0xce,
        (char)0x11, (char)0x66, (char)0xa8, (char)0xa7, (char)0x5e, (char)0x01,
        (char)0x8a, (char)0xa9, (char)0x0b, (char)0x9c, (char)0x15, (char)0x49,
        (char)0x3d, (char)0x6b, (char)0x5f, (char)0x6e, (char)0x28, (char)0x2c,
        (char)0x01, (char)0x85, (char)0x21, (char)0x5c, (char)0xd2, (char)0xa5,
        (char)0xdf, (char)0x4e, (char)0xd5, (char)0x9c, (char)0xac, (char)0xe3,
        (char)0xff, (char)0x9a, (char)0x82, (char)0xa9, (char)0xe3, (char)0xad,
        (char)0x6b, (char)0x13, (char)0xc9, (char)0x68, (char)0xfe, (char)0x2c,
        (char)0x2c, (char)0x2b, (char)0x3d, (char)0xe1, (char)0xc3, (char)0xc6,
        (char)0x3c, (char)0x2f, (char)0xab, (char)0x7f, (char)0x96, (char)0x13,
        (char)0xf3, (char)0x76, (char)0xb6, (char)0x9f
    };

    tea_decrypt(szCrypt, sizeof(szCrypt)/sizeof(char),
                szValue, &dwValueLen, key, mixkey);

    printf("++++++++++++++++++++\n");

    for (i = 0; i < dwValueLen; i++)
    {
        if (i > 0 && i%16 == 0) printf("\n");
        printf("%02x ", (U8)szValue[i]);
    }
    printf("\n");
}

void test_tea()
{
    test_tea_encrypt();
    test_tea_decrypt();
}

