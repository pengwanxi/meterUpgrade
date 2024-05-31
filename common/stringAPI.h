#ifndef _STRING_PROCESS_H
#define _STRING_PROCESS_H

#include "zlog.h"

void hexBufferToAscii(const unsigned char *hexBuffer, int bufferLength, char *asciiBuffer);

void hexStringToAscii(const char *hexString, int hexStringLen, char *asciiString);

int hexArrayToInt(const unsigned char *hexArray, int length);

void CopyArray(char *src, char *dest, int size);

void ShowPacket(zlog_category_t *zlogA, unsigned char *head, unsigned char *buf, int len);

void zlog_print(zlog_category_t *zlogU,char *msg,const char *buf,int bufSize);

void get_system_time(unsigned char *time_array);//获取系统时间
#endif