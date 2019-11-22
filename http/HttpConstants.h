/*
 * @(#) HttpConstants.h Created on 2016-09-28
 *
 * Copyright (c) 2008-2054 Erisen Xu(徐勇) All Rights Reserved
 */
 
#ifndef HTTP_CONSTANTS_H
#define HTTP_CONSTANTS_H

#define MAX_HTTP_PROTOCOL_LEN       8
#define MAX_HTTP_VERSION_LEN        8
#define MAX_HTTP_STATUS_LEN         16
#define MAX_HTTP_REASON_LEN         256
#define MAX_HTTP_STATUS_LINE_LEN    312

#define MAX_HTTP_METHOD_LEN         8
#define MAX_HTTP_URL_LEN            512
#define MAX_HTTP_REQUEST_LINE_LEN   576

#define MAX_HTTP_HDR_NAME_LEN       64
#define MAX_HTTP_HDR_VALUE_LEN      2048
#define MAX_HTTP_HDR_LEN            2304

#define MAX_HTTP_HDR_NUM            128

#define MAX_HTTP_BODY_LEN           8192
#define MAX_HTTP_MSG_LEN            8192

#define HTTP_HDR_LENGTH_NAME        "Content-Length"

#define HTTP_BODY_INDICATOR         "\r\n\r\n"

/* 状态码定义 */
#define HTTP_STATUS_CODE_100        "100"
#define HTTP_STATUS_INFO_100        "Continue"
#define HTTP_STATUS_CODE_101        "101"
#define HTTP_STATUS_INFO_101        "Switching Protocols"
#define HTTP_STATUS_CODE_102        "102"
#define HTTP_STATUS_INFO_102        "Processing"

/* 成功 */
#define HTTP_STATUS_CODE_200        "200"
#define HTTP_STATUS_INFO_200        "OK"
#define HTTP_STATUS_CODE_201        "201"
#define HTTP_STATUS_INFO_201        "Created"
#define HTTP_STATUS_CODE_202        "202"
#define HTTP_STATUS_INFO_202        "Accepted"
#define HTTP_STATUS_CODE_203        "203"
#define HTTP_STATUS_INFO_203        "Non-Authoritative Information"
#define HTTP_STATUS_CODE_204        "204"
#define HTTP_STATUS_INFO_204        "No Content"
#define HTTP_STATUS_CODE_205        "205"
#define HTTP_STATUS_INFO_205        "Reset Content"
#define HTTP_STATUS_CODE_206        "206"
#define HTTP_STATUS_INFO_206        "Partial Content"
#define HTTP_STATUS_CODE_207        "207"
#define HTTP_STATUS_INFO_207        "Multi-Status"

/* 重定向 */
#define HTTP_STATUS_CODE_300        "300"
#define HTTP_STATUS_INFO_300        "Multiple Choices"
#define HTTP_STATUS_CODE_301        "301"
#define HTTP_STATUS_INFO_301        "Moved Permanently"
#define HTTP_STATUS_CODE_302        "302"
#define HTTP_STATUS_INFO_302        "Move temporarily"
#define HTTP_STATUS_CODE_303        "303"
#define HTTP_STATUS_INFO_303        "See Other"
#define HTTP_STATUS_CODE_304        "304"
#define HTTP_STATUS_INFO_304        "Not Modified"
#define HTTP_STATUS_CODE_305        "305"
#define HTTP_STATUS_INFO_305        "Use Proxy"
#define HTTP_STATUS_CODE_306        "306"
#define HTTP_STATUS_INFO_306        "Switch Proxy"
#define HTTP_STATUS_CODE_307        "307"
#define HTTP_STATUS_INFO_307        "Temporary Redirect"

/* 请求错误 */
#define HTTP_STATUS_CODE_400        "400"
#define HTTP_STATUS_INFO_400        "Bad Request"
#define HTTP_STATUS_CODE_401        "401"
#define HTTP_STATUS_INFO_401        "Unauthorized"
#define HTTP_STATUS_CODE_402        "402"
#define HTTP_STATUS_INFO_402        "Payment Required"
#define HTTP_STATUS_CODE_403        "403"
#define HTTP_STATUS_INFO_403        "Forbidden"
#define HTTP_STATUS_CODE_404        "404"
#define HTTP_STATUS_INFO_404        "Not Found"
#define HTTP_STATUS_CODE_405        "405"
#define HTTP_STATUS_INFO_405        "Method Not Allowed"
#define HTTP_STATUS_CODE_406        "406"
#define HTTP_STATUS_INFO_406        "Not Acceptable"
#define HTTP_STATUS_CODE_407        "407"
#define HTTP_STATUS_INFO_407        "Proxy Authentication Required"
#define HTTP_STATUS_CODE_408        "408"
#define HTTP_STATUS_INFO_408        "Request Timeout"
#define HTTP_STATUS_CODE_409        "409"
#define HTTP_STATUS_INFO_409        "Conflict"
#define HTTP_STATUS_CODE_410        "410"
#define HTTP_STATUS_INFO_410        "Gone"
#define HTTP_STATUS_CODE_411        "411"
#define HTTP_STATUS_INFO_411        "Length Required"
#define HTTP_STATUS_CODE_412        "412"
#define HTTP_STATUS_INFO_412        "Precondition Failed"
#define HTTP_STATUS_CODE_413        "413"
#define HTTP_STATUS_INFO_413        "Request Entity Too Large"
#define HTTP_STATUS_CODE_414        "414"
#define HTTP_STATUS_INFO_414        "Request-URI Too Long"
#define HTTP_STATUS_CODE_415        "415"
#define HTTP_STATUS_INFO_415        "Unsupported Media Type"
#define HTTP_STATUS_CODE_416        "416"
#define HTTP_STATUS_INFO_416        "Requested Range Not Satisfiable"
#define HTTP_STATUS_CODE_417        "417"
#define HTTP_STATUS_INFO_417        "Expectation Failed"
#define HTTP_STATUS_CODE_421        "421"
#define HTTP_STATUS_INFO_421        "Too many connections"
#define HTTP_STATUS_CODE_422        "422"
#define HTTP_STATUS_INFO_422        "Unprocessable Entity"
#define HTTP_STATUS_CODE_423        "423"
#define HTTP_STATUS_INFO_423        "Locked"
#define HTTP_STATUS_CODE_424        "424"
#define HTTP_STATUS_INFO_424        "Failed Dependency"
#define HTTP_STATUS_CODE_425        "425"
#define HTTP_STATUS_INFO_425        "Unordered Collection"
#define HTTP_STATUS_CODE_426        "426"
#define HTTP_STATUS_INFO_426        "Upgrade Required"
#define HTTP_STATUS_CODE_449        "449"
#define HTTP_STATUS_INFO_449        "Retry With"
#define HTTP_STATUS_CODE_451        "451"
#define HTTP_STATUS_INFO_451        "Unavailable For Legal Reasons"

/* 服务器错误 */
#define HTTP_STATUS_CODE_500        "500"
#define HTTP_STATUS_INFO_500        "Internal Server Error"
#define HTTP_STATUS_CODE_501        "501"
#define HTTP_STATUS_INFO_501        "Not Implemented"
#define HTTP_STATUS_CODE_502        "502"
#define HTTP_STATUS_INFO_502        "Bad Gateway"
#define HTTP_STATUS_CODE_503        "503"
#define HTTP_STATUS_INFO_503        "Service Unavailable"
#define HTTP_STATUS_CODE_504        "504"
#define HTTP_STATUS_INFO_504        "Gateway Timeout"
#define HTTP_STATUS_CODE_505        "505"
#define HTTP_STATUS_INFO_505        "HTTP Version Not Supported"
#define HTTP_STATUS_CODE_506        "506"
#define HTTP_STATUS_INFO_506        "Variant Also Negotiates"
#define HTTP_STATUS_CODE_507        "507"
#define HTTP_STATUS_INFO_507        "Insufficient Storage"
#define HTTP_STATUS_CODE_509        "509"
#define HTTP_STATUS_INFO_509        "Bandwidth Limit Exceeded"
#define HTTP_STATUS_CODE_510        "510"
#define HTTP_STATUS_INFO_510        "Not Extended"
#define HTTP_STATUS_CODE_600        "600"
#define HTTP_STATUS_INFO_600        "Unparseable Response Headers"

#endif

