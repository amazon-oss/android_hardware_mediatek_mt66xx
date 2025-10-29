#ifndef _OS_DEP_H
#define _OS_DEP_H

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>


#ifndef FALSE
#define FALSE    0
#endif
#ifndef TRUE
#define TRUE     1
#endif
#ifndef BOOL
#define BOOL     bool
#endif

typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef unsigned char* PBYTE;
typedef unsigned char* PUCHAR;
typedef char INT8;
typedef short INT16;
typedef int INT32;
typedef long long INT64;
typedef void VOID;
typedef void* PVOID;


/* LOG_TAG must be defined before log.h */
#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG               "[BT]"
#include <cutils/log.h>

#define BT_DRIVER_DEBUG       1
#define LOG_ERR(f, ...)       ALOGE("%s: " f, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WAN(f, ...)       ALOGW("%s: " f, __FUNCTION__, ##__VA_ARGS__)
#if BT_DRIVER_DEBUG
#define LOG_DBG(f, ...)       ALOGD("%s: " f,  __FUNCTION__, ##__VA_ARGS__)
#define LOG_TRC(f)            ALOGW("%s #%d", __FUNCTION__, __LINE__)
#else
#define LOG_DBG(...)          ((void)0)
#define LOG_TRC(f)            ((void)0)
#endif
#endif
