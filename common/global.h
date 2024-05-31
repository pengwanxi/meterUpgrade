/**
 *   \file global.h
 *   \brief 数据结构
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// using namespace std;
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
// #define FROMLOCAL(a) QString::fromUtf8(a)
#define FROMLOCAL(a) QString::fromUtf8(a)

// 定义数据类型
typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned long DWORD;
typedef bool BOOL;
typedef int HANDLE;
typedef void *LPVOID;

#define MAKEWORD(a, b)                      \
    ((WORD)(((BYTE)(((DWORD)(a)) & 0xff)) | \
            ((WORD)((BYTE)(((DWORD)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)                         \
    ((DWORD)(((WORD)(((DWORD)(a)) & 0xffff)) | \
             ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define MAKEDWORD(a, b)                        \
    ((DWORD)(((WORD)(((DWORD)(a)) & 0xffff)) | \
             ((DWORD)((WORD)(((DWORD)(b)) & 0xffff))) << 16))
#define LOWORD(l) ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l) ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w) ((BYTE)(((WORD)(w)) & 0xff))
#define HIBYTE(w) ((BYTE)((((WORD)(w)) >> 8) & 0xff))
#define UINT8 BYTE
#define UINT16 WORD
#define FALSE false
#define TRUE true

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

////////////////////////////////////////////////////////////////////////////////
// 全局数据类型及数据结构定义
typedef signed char s08_t;
typedef signed short s16_t;
typedef signed int s32_t;
typedef unsigned char u08_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef long long int s64_t;
typedef unsigned long long int u64_t;
typedef float f32_t;
typedef double f64_t;

#define NR_BITS_BYTE (8)
#define BITMASK_BYTE (0xFF)

#define BIT_SET(num, i) ((num) |= (1 << (i)))
#define BIT_GET(num, i) (((num) & ((unsigned int)1 << (i))) > 0 ? true : false)
#define BIT_CLR(num, i) ((num) &= ~(1 << (i)))

#define MOD_INC(x, mod) x = (((x) + 1) < (mod) ? ((x) + 1) : 0) //
#define MOD_DEC(x, mod) x = ((x) > 0 ? ((x) - 1) : (mod))       //
#define MOD_ADD(x, y, mod) \
    x = (((x) + (y)) < (mod) ? ((x) + (y)) : (((x) + (y)) - (mod))) //
#define MOD_SUB(x, y, mod) \
    x = (((x) < (y)) ? ((mod) - ((y) - (x))) : ((x) - (y))) //

// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define BETWEEN(a, b, c) ((((a) >= (b)) && ((a) <= (c))) ? true : false)
/*  */
typedef enum _TEXT_FRAME_FLAG
{
    TF_SEND = 0,
    TF_RECV,
    TF_SEND_ASC,
    TF_RECV_ASC,
} TEXT_FRAME_FLAG;

/**
 *  \brief 字符串转bcd
 *  \param 字符串
 *  \param bcd
 *  \param 长度
 *  \return void
 */
void public_stob(const char *pstr, unsigned char *pbcd, int len);

void add2print_queue(const char *fmt, ...);
char *get_print_queue(char *out);

u32_t Get_CRC32(u08_t *buf, u32_t size, u32_t CRC);
void hexStringToAscii(const char *hexString, int hexStringLen, unsigned char *asciiString);

#endif /* _GLOBAL_H_ */
