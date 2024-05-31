/**
 *   \file dlt645_public.h
 *   \brief 645规约一些公用的函数
 */

#ifndef DLT645_07_API_H
#define DLT645_07_API_H
#include "global.h"

/*  */
typedef enum _DLT645_BCD_ORDER {
    DLT645_BCD_POSITIVE = 0, /* 正序 */
    DLT645_BCD_REVERSE,
} DLT645_BCD_ORDER;

/*  */
typedef enum _HEX_ORDER {
    HEX_POSITIVE = 0, /* 正序 */
    HEX_REVERSE,
} HEX_ORDER;


/*  */
typedef enum _PROTOCOL_DL645_INDEX {
    DLT645_HEAD0 = 0,
    DLT645_ADDR0,
    DLT645_ADDR1,
    DLT645_ADDR2,
    DLT645_ADDR3,
    DLT645_ADDR4,
    DLT645_ADDR5,
    DLT645_HEAD1,
    DLT645_CFN,
    DLT645_LEN,
    DLT645_DI0,
    DLT645_DI1,
    DLT645_DI2,
    DLT645_DI3,
} PROTOCOL_DLT645_INDEX;

/**
 *  \brief bcd 转 字符
 *  \param bcd
 *  \param 字符串
 *  \param 长度
 *  \return void
 */
void bcd2str(const u08_t *inBCD, u08_t *outStr, int len);

/**
 *  \brief 字符串转bcd
 *  \param 字符串
 *  \param bcd
 *  \param 长度
 *  \return void
 */
void str2bcd(const u08_t *pstr, u08_t *pbcd, int len);

/**
 *  \brief 10进制转bcd
 *  \param 10进制字符
 *  \return bcd 码
 */
u08_t dec2bcd(u08_t d);

/**
 *  \brief 双字转bcd
 *  \param 双字
 *  \param bcd 缓冲区
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return void
 */
void dword2bcd(DWORD dw, u08_t *pbcd, int len, int flag);

/**
 *  \brief bcd转双字
 *  \param bcd 缓冲区
 *  \param 双字
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return void
 */
void bcd2dword(u08_t *pbcd, DWORD *dw, int len, int flag);

/**
 *  \brief 浮点数转bcd
 *  \param 浮点数
 *  \param bcd 缓冲区
 *  \param 变比
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return void
 */
void float2bcd(float fval, u08_t *pbcd, float scale, int len, int flag);

/**
 *  \brief bcd转浮点数
 *  \param bcd 缓冲区
 *  \param 浮点数
 *  \param 变比
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return void
 */
void bcd2float(u08_t *pbcd, float *fval, float scale, int len, int flag);

/**
 *  \brief bcd转浮点数
 *  \param bcd 缓冲区
 *  \param 浮点数
 *  \param 变比
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return void
 */
void bcd2ufloat(u08_t *pbcd, float *fval, float scale, int len, int flag);

/**
 *  \brief 浮点数转hex
 *  \param 浮点数
 *  \param hex 缓冲区
 *  \param 变比
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return BOOL
 */
BOOL float2hex(float fval, u08_t *phex, float scale, int len, int flag);

/**
 *  \brief hex转浮点数
 *  \param hex 缓冲区
 *  \param 浮点数
 *  \param 变比
 *  \param 转为bcd 的长度
 *  \param 1为正向0为反向
 *  \return BOOL
 */
BOOL hex2float(u08_t *phex, float *fval, float scale, int len, int flag);

/**
 *  \brief 加0x33
 *  \param psrc
 *  \param 长度
 *  \return pdes
 *  \return NULL 错误
 */
u08_t *add33h(u08_t *psrc, u08_t *pdes, int len, int flag);
/**
 *  \brief 减0x33
 *  \param psrc
 *  \param 长度
 *  \return pdes
 *  \return NULL 错误
 */
u08_t *sub33h(u08_t *psrc, u08_t *pdes, int len, int flag);

/**
 *  \brief bcd转16进制
 *  \param bcd
 *  \return 16进制
 */
u08_t bcd2dec(u08_t c);

/**
 *  \brief 字符串转16进制
 *  \param 字符串
 *  \return
 */
unsigned long str2hex(char *buf);

/**
 *  \brief asc转16进制
 *  \param asc
 *  \return 转化的数值
 */
unsigned long asc2hex(char *buf);

/**
 *  \brief asc转16进制
 *  \param asc
 *  \param 只转化前len个字节 len < 8
 *  \param byFlag=1:正序 byFlag=0 倒序
 *  \return 转化的数值
 */
unsigned long asctohex_order(char *buf, u08_t len, u08_t byFlag);

/**
 *  \brief 计算校验和
 *  \param buf
 *  \param len
 *  \return 校验和
 */
unsigned char cal_cs(const unsigned char *buf, int len);

/**
 *  \brief 生成32位的DI
 *  \param DI[4]
 *  \return 32DI
 */
u32_t pack_dword_di(u08_t *DI);

/*  */
typedef enum _DIRECTION
{
	DIR_POSITIVE = 0,
	DIR_REVERSE,
}DIRECTION;

/**
 *  \brief 字符串拷贝
 *
 *  正常的则用memcpy,需要反向或是正向的时候用这个
 *
 *  \param src
 *  \param dec
 *  \param len
 *  \param dir
 *  \return
 */
void *memcpydir(void *des, void *src, int len, int dir);

/**
 *  \brief 比较
 *
 *  正常的则用memcmp,需要反向或是正向的时候用这个
 *
 *  \param src
 *  \param dec
 *  \param len
 *  \param dir
 *  \return 1 大于
 *  \return 0 等于
 *  \return -1 小于
 *  \return -2 出错
 */
int memcmpdir(void *des, void *src, int len, int dir);

/**
 *  \brief 无符号16位转浮点数
 *  \param u16_t
 *  \param scale
 *  \return float
 */
float u16tof(u16_t val, float scale);

/**
 *  \brief 有符号16位转浮点数
 *  \param s16_t
 *  \param scale
 *  \return float
 */
float s16tof(s16_t val, float scale);

/**
 *  \brief 无符号32位转浮点数
 *  \param u32_t
 *  \param scale
 *  \return float
 */
float u32tof(u32_t val, float scale);

/**
 *  \brief 有符号32位转浮点数
 *  \param s32_t
 *  \param scale
 *  \return float
 */
float s32tof(s32_t val, float scale);

/**
 *  \brief 浮点数转无符号16位
 *  \param u16_t
 *  \param scale
 *  \return float
 */
u16_t ftou16(float fval, float scale);

/**
 *  \brief 浮点数转有符号16位
 *  \param s16_t
 *  \param scale
 *  \return float
 */
s16_t ftos16(float fval, float scale);

/**
 *  \brief 浮点数转无符号32位
 *  \param u32_t
 *  \param scale
 *  \return float
 */
u32_t ftou32(float fval, float scale);

/**
 *  \brief 浮点数转有符号32位
 *  \param s32_t
 *  \param scale
 *  \return float
 */
s32_t ftos32(float fval, float scale);

//根据年月日计算星期几
//返回：0星期日、1星期一、2星期二、3星期三、4星期四、5星期五、6星期六
u08_t public_cal_weekday(int y, u08_t m, u08_t d);

#endif /* DLT645_07_API_H */
