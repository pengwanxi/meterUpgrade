#include "stringAPI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 获取系统时间并返回一个6字节数组
void get_system_time(unsigned char *time_array)
{
    // 获取当前时间
    time_t rawtime;
    struct tm *timeinfo;

    // 获取当前系统时间
    time(&rawtime);
    // 转换成本地时间
    timeinfo = localtime(&rawtime);

    // 按秒、分、时、日、月、年的顺序存储时间信息
    time_array[0] = (unsigned char)timeinfo->tm_sec;          // 秒
    time_array[1] = (unsigned char)timeinfo->tm_min;          // 分
    time_array[2] = (unsigned char)timeinfo->tm_hour;         // 时
    time_array[3] = (unsigned char)timeinfo->tm_mday;         // 日
    time_array[4] = (unsigned char)(timeinfo->tm_mon + 1);    // 月 (tm_mon 范围是 0-11，故 +1)
    time_array[5] = (unsigned char)(timeinfo->tm_year % 100); // 年 (取 tm_year 的低两位)
}

void hexStringToAscii(const char *hexString, int hexStringLen, char *asciiString)
{
    int i, j = 0;
    // 确保输入字符串长度是偶数
    if (hexStringLen % 2 != 0)
    {
        fprintf(stderr, "Error: Odd-length hex string.\n");
        return;
    }

    // 逐两个字符处理
    for (i = 0; i < hexStringLen; i += 2)
    {
        char byte[3];
        byte[0] = hexString[i];
        byte[1] = hexString[i + 1];
        byte[2] = '\0';

        // 将两个字符转换成一个字节
        asciiString[j++] = (char)strtol(byte, NULL, 16);
    }

    // 添加字符串结束符
    asciiString[j] = '\0';
    // printf("asciiString : [%s]\n", asciiString);
}

void hexBufferToAscii(const unsigned char *hexBuffer, int bufferLength, char *asciiBuffer)
{
    for (int i = 0; i < bufferLength; ++i)
    {
        asciiBuffer[i] = hexBuffer[i];
    }
    asciiBuffer[bufferLength] = '\0'; // 添加字符串结束符
}

int hexArrayToInt(const unsigned char *hexArray, int length)
{
    int result = 0;

    for (size_t i = 0; i < length; i++)
    {
        // 左移四位，给下一个字节腾出空间
        result = (result << 4) | hexArray[i];
    }

    return result;
}

// 赋值，将src的第一个元素赋值给dest的最后一个元素
void CopyArray(char *src, char *dest, int size)
{
    for (int i = 0; i < size; ++i)
    {
        dest[size - 1 - i] = src[i];
    }
}

void zlog_print(zlog_category_t *zlogU, char *msg, const char *buf, int bufSize)
{
    /*
    char *printBuf = (char *)malloc(bufSize * sizeof(char));
    if (printBuf == NULL)
    {
        zlog_error(zlogU, "malloc failed!");
        return;
    }*/
    char printBuf[65535] = {0};
    for (int i = 0; i < bufSize; i++)
    {
        sprintf(printBuf + (i * 3), "%02hhx ", buf[i]);
    }
    zlog_info(zlogU, msg, printBuf);
}