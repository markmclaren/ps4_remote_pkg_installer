#pragma once

#include <stddef.h>
#include <stdint.h>

#define SCE_TRUE  1
#define SCE_FALSE 0

#define SCE_HTTP_METHOD_GET  0
#define SCE_HTTP_METHOD_POST 1
#define SCE_HTTP_METHOD_HEAD 2

#define SCE_HTTP_VERSION_1_0 1
#define SCE_HTTP_VERSION_1_1 2

#define SCE_HTTP_CONTENTLEN_EXIST        0
#define SCE_HTTP_CONTENTLEN_NOT_FOUND    1
#define SCE_HTTP_CONTENTLEN_CHUNK_ENC    2

#define SCE_HTTP_HEADER_OVERWRITE        0
#define SCE_HTTP_HEADER_ADD              1

#define SCE_HTTPS_FLAG_SERVER_VERIFY     (0x01U)
#define SCE_HTTPS_FLAG_CLIENT_VERIFY     (0x02U)
#define SCE_HTTPS_FLAG_CN_CHECK          (0x04U)
#define SCE_HTTPS_FLAG_NOT_AFTER_CHECK   (0x08U)
#define SCE_HTTPS_FLAG_NOT_BEFORE_CHECK  (0x10U)
#define SCE_HTTPS_FLAG_KNOWN_CA_CHECK    (0x20U)

#define SCE_HTTP_ERROR_OUT_OF_MEMORY     0x80431022
#define SCE_HTTP_ERROR_NO_CONTENT_LENGTH 0x80431071
#define SCE_HTTP_ERROR_NOT_FOUND         0x80431072
#define SCE_HTTP_ERROR_INVALID_ID        0x80431100
#define SCE_HTTP_ERROR_INVALID_VALUE     0x804311FE

int sceSslInit(size_t poolSize);
int sceSslTerm(int libsslCtxId);

int sceHttpInit(int libnetMemId, int libsslCtxId, size_t poolSize);
int sceHttpTerm(int libhttpCtxId);

int sceHttpCreateTemplate(int libhttpCtxId, const char* userAgent, int httpVer, int isAutoProxyConf);
int sceHttpDeleteTemplate(int tmplId);
int sceHttpCreateConnectionWithURL(int tmplId, const char* url, int isKeepalive);
int sceHttpDeleteConnection(int connId);
int sceHttpCreateRequestWithURL(int connId, int method, const char* url, uint64_t contentLength);
int sceHttpDeleteRequest(int reqId);

int sceHttpsDisableOption(int id, unsigned int sslFlags);
int sceHttpAddRequestHeader(int id, const char* name, const char* value, uint32_t mode);
int sceHttpSendRequest(int reqId, const void* postData, size_t size);
int sceHttpGetStatusCode(int reqId, int* statusCode);
int sceHttpGetResponseContentLength(int reqId, int* result, uint64_t* contentLength);
int sceHttpReadData(int reqId, void* data, size_t size);

int sceHttpUriEscape(char* out, size_t* require, size_t outSize, const char* in);
int sceHttpUriUnescape(char* out, size_t* require, size_t outSize, const char* in);

