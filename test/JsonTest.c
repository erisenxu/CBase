/*
 * @(#) JsonTest.c Created on 2017-04-23
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */


#include "JsonTest.h"
#include <stdio.h>
#include <string.h>
#include <json/JsonParser.h>

#define JSON_STR "\
    {\"a\":\"100\",\"b\":[{\"b1\":\"b_value1\",\"b2\":\"b_value2\"}, {\"b1\":\"b_value1\",\"b2\":\"b_value2\"}], \"c\": {\"c1\":\"c_value1\",\"c2\":\"c_value2\"}}  \
"

void onJsonParseError(int iErrCode, const char* szErrInfo, const char* szJson, void* pstCallData)
{
    printf("%s,%d,'%c' is unexpected,%s\n", szErrInfo, iErrCode, *szJson, szJson);
}

int onParseStartElement(const char* szName, void* pstCallData)
{
    printf("onStartElement:%s.\n", szName);
    return 0;
}

int onParseEndElement(const char* szName, const char* szValue, char chJsonType, char chItemType, void* pstCallData)
{
    printf("onEndElement:%s=%s,json type:%d, item type:%d.\n", szName, szValue, chJsonType, chItemType);
    return 0;
}

void test_parse_json_file(const char* szJsonFile)
{
    JSONPARSEHANDLER stJsonHdr;

    stJsonHdr.onStartElement = onParseStartElement;
    stJsonHdr.onEndElement = onParseEndElement;
    stJsonHdr.onParseError = onJsonParseError;

    parse_json_file(szJsonFile, &stJsonHdr, NULL);
}

void test_parse_json_string()
{
    JSONPARSEHANDLER stJsonHdr;

    stJsonHdr.onStartElement = onParseStartElement;
    stJsonHdr.onEndElement = onParseEndElement;
    stJsonHdr.onParseError = onJsonParseError;

    parse_json_string(JSON_STR, &stJsonHdr, NULL);
}

void test_json(const char* szJsonFile)
{
    test_parse_json_file(szJsonFile);

    printf("---------------------------------------\n");

    test_parse_json_string();
}

