/*
 * @(#) UriTest.c Created on 2017-04-23
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */


#include "UriTest.h"
#include <stdio.h>
#include <string.h>
#include "comm/UriLib.h"

void test_parse_url(const char* szUri, const char* szProtocolName, const char* szHost, U16 nPort, const char* szPath)
{
    URIINFO stUriInfo;

    int iRet = uri_parse(szUri, &stUriInfo);

    if (iRet != 0)
    {
        printf("parse url '%s' error:%d\n", szUri, iRet);
        return;
    }

    if (strcmp(stUriInfo.szProtocolName, szProtocolName) != 0)
    {
        printf("parse url '%s' fail, protocol geted is:%s\n", szUri, stUriInfo.szProtocolName);
        return;
    }

    if (strcmp(stUriInfo.szHost, szHost) != 0)
    {
        printf("parse url '%s' fail, host geted is:%s\n", szUri, stUriInfo.szHost);
        return;
    }

    if (stUriInfo.nPort != nPort)
    {
        printf("parse url '%s' fail, port geted is:%d\n", szUri, stUriInfo.nPort);
        return;
    }

    if (strcmp(stUriInfo.szPath, szPath) != 0)
    {
        printf("parse url '%s' fail, path geted is:%s\n", szUri, stUriInfo.szPath);
        return;
    }
}

void test_uri()
{
    test_parse_url("/", "", "", 80, "/");
    test_parse_url("/index.html", "", "", 80, "/index.html");
    test_parse_url("/index.html?a=test&b=78", "", "", 80, "/index.html");
    test_parse_url("/index.html#test", "", "", 80, "/index.html");

    test_parse_url("index.html", "", "", 80, "index.html");
    test_parse_url("index.html?a=test&b=78", "", "", 80, "index.html");
    test_parse_url("index.html#test", "", "", 80, "index.html");

    test_parse_url("https://www.qq.com", "https", "www.qq.com", 80, "");
    test_parse_url("https://www.qq.com/", "https", "www.qq.com", 80, "/");
    test_parse_url("https://www.qq.com/index.html", "https", "www.qq.com", 80, "/index.html");
    test_parse_url("https://www.qq.com/index.html?a=test&b=78", "https", "www.qq.com", 80, "/index.html");
    test_parse_url("https://www.qq.com/index.html#test", "https", "www.qq.com", 80, "/index.html");

    test_parse_url("https://www.qq.com:3033", "https", "www.qq.com", 3033, "");
    test_parse_url("https://www.qq.com:4567/", "https", "www.qq.com", 4567, "/");
    test_parse_url("https://www.qq.com:2222/index.html", "https", "www.qq.com", 2222, "/index.html");
    test_parse_url("https://www.qq.com:4578/index.html?a=test&b=78", "https", "www.qq.com", 4578, "/index.html");
    test_parse_url("https://www.qq.com:678/index.html#test", "https", "www.qq.com", 678, "/index.html");
}

