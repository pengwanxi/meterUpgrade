#include "dlt645_07_api.h"
#include <float.h>
#include <math.h>

u32_t DLT645_PUBLIC_BAUD[] = {
    0, 600, 1200, 2400, 4800, 9600, 19200,
};

static unsigned char ascii_table[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void bcd2str(const u08_t *inBCD, u08_t *outStr, int len)
{
    int i = 0, j = 0;

    if (NULL == inBCD || NULL == outStr || len < 1)
        return;

    for (i = len - 1, j = 0; i >= 0; i--) {
        char c0 = inBCD[i] & 0xF;
        char c1 = (inBCD[i] >> 4) & 0xF;
        if (c1 >= 0 && c1 <= 9)
            outStr[j++] = c1 + '0';
        else
            outStr[j++] = c1 - 0x0A + 'A';

        if (c0 >= 0 && c0 <= 9)
            outStr[j++] = c0 + '0';
        else
            outStr[j++] = c0 - 0x0A + 'A';
    }
}

void str2bcd(const u08_t *pstr, u08_t *pbcd, int len)
{
    u08_t tmpValue;
    int i;
    int j;
    int m;
    int sLen;

    sLen = strlen((char *)pstr);
    for (i = 0; i < sLen; i++) {
        if ((pstr[i] < '0') || ((pstr[i] > '9') && (pstr[i] < 'A')) ||
            ((pstr[i] > 'F') && (pstr[i] < 'a')) || (pstr[i] > 'f')) {
            sLen = i;
            break;
        }
    }

    sLen = (sLen <= (len * 2)) ? sLen : sLen * 2;
    memset((void *)pbcd, 0x00, len);

    for (i = sLen - 1, j = 0, m = 0; (i >= 0) && (m < len); i--, j++) {
        if ((pstr[i] >= '0') && (pstr[i] <= '9')) {
            tmpValue = pstr[i] - '0';
        }
        else if ((pstr[i] >= 'A') && (pstr[i] <= 'F')) {
            tmpValue = pstr[i] - 'A' + 0x0A;
        }
        else if ((pstr[i] >= 'a') && (pstr[i] <= 'f')) {
            tmpValue = pstr[i] - 'a' + 0x0A;
        }
        else {
            tmpValue = 0;
        }

        if ((j % 2) == 0) {
            pbcd[m] = tmpValue;
        }
        else {
            pbcd[m++] |= (tmpValue << 4);
        }

        if ((tmpValue == 0) && (pstr[i] != '0')) {
            break;
        }
    }
}

u08_t bcd2dec(u08_t c)
{
    return (u08_t)((c >> 4) * 10 + (c & 0x0f));
}

u08_t dec2bcd(u08_t d)
{
    return ((d / 100) << 8) | (((d % 100) / 10) << 4) | ((d % 100) % 10);
}

void dword2bcd(DWORD dw, u08_t *pbcd, int len, int flag)
{
    int i;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;
        if (flag == DLT645_BCD_REVERSE)
            pos = i;

        pbcd[pos] = dec2bcd(dw % 100);

        dw = dw / 100;
    }
}

void bcd2dword(u08_t *pbcd, DWORD *dw, int len, int flag)
{
    int i;
    DWORD tmp;
    *dw = 0;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;
        if (flag == DLT645_BCD_POSITIVE)
            pos = i;

        tmp = bcd2dec(pbcd[pos]);
        *dw = (*dw * 100) + tmp;
    }
}

extern void add2print_queue(const char *fmt, ...);
void float2bcd(float fval, u08_t *pbcd, float scale, int len, int flag)
{
    long l = (long)(fval * scale);
    bool negative = FALSE;
    int i;
	// add2print_queue("fval=%f scale=%f l=%d\n", fval, scale, l);

    if (l < 0) {
        /* ms_printf("\n\n\nnegative l=%d\n\n\n", (int)l); */
        negative = TRUE;
        l = 0 - l;
    }

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;
        if (flag == DLT645_BCD_REVERSE)
            pos = i;

        pbcd[pos] = dec2bcd(l % 100);
        if (negative && i == (len - 1))
            pbcd[pos] |= 0x80;

        /* ms_printf("pos=%d ch=%x\n", pos, pbcd[pos]); */

        l = l / 100;
    }
}

void bcd2ufloat(u08_t *pbcd, float *fval, float scale, int len, int flag)
{
    int i;
    u08_t tmp;
    long l = 0;
    // bool negative = FALSE;

    for (i = 0; i < len; i++) {
        u08_t ch;
        int pos = len - i - 1;
        if (flag == DLT645_BCD_POSITIVE)
            pos = i;

        ch = pbcd[pos];
        /* ms_printf("pos=%d ch=%x\n", pos, ch); */
        // if ((i == 0) && (ch & 0x80) > 0) {
        //     /* ms_printf("negative ch=%x\n", ch); */
        //     negative = TRUE;
        //     ch = ch & 0x7F;
        // }

        tmp = bcd2dec(ch);
        l = (l * 100) + tmp;
    }

    // if (negative) {
    //     l = 0 - l;
    //     /* ms_printf("negative l=%d\n", (int)l); */
    // }

    *fval = (float)l * scale;
}

void bcd2float(u08_t *pbcd, float *fval, float scale, int len, int flag)
{
    int i;
    u08_t tmp;
    long l = 0;
    bool negative = FALSE;

    for (i = 0; i < len; i++) {
        u08_t ch;
        int pos = len - i - 1;
        if (flag == DLT645_BCD_POSITIVE)
            pos = i;

        ch = pbcd[pos];
        /* ms_printf("pos=%d ch=%x\n", pos, ch); */
        if ((i == 0) && (ch & 0x80) > 0) {
            /* ms_printf("negative ch=%x\n", ch); */
            negative = TRUE;
            ch = ch & 0x7F;
        }

        tmp = bcd2dec(ch);
        l = (l * 100) + tmp;
    }

    if (negative) {
        l = 0 - l;
        /* ms_printf("negative l=%d\n", (int)l); */
    }

    *fval = (float)l * scale;
}

BOOL float2hex(float fval, u08_t *phex, float scale, int len, int flag)
{
    switch (len) {
    case 1: {
        u16_t val = ftou16(fval, scale);
        *phex = LOBYTE(val);
    } break;
    case 2: {
        u16_t val = ftou16(fval, scale);
        if (HEX_POSITIVE == flag) {
            phex[0] = HIBYTE(val);
            phex[1] = LOBYTE(val);
        }
        else {
            phex[1] = HIBYTE(val);
            phex[0] = LOBYTE(val);
        }
    } break;
    case 3: {
        u32_t val = ftou32(fval, scale);
        if (HEX_POSITIVE == flag) {
            phex[0] = LOBYTE(HIWORD(val));
            phex[1] = HIBYTE(LOWORD(val));
            phex[2] = LOBYTE(LOWORD(val));
        }
        else {
            phex[2] = LOBYTE(HIWORD(val));
            phex[1] = HIBYTE(LOWORD(val));
            phex[0] = LOBYTE(LOWORD(val));
        }
    } break;
    case 4: {
        u32_t val = ftou32(fval, scale);
        if (HEX_POSITIVE == flag) {
            phex[0] = HIBYTE(HIWORD(val));
            phex[1] = LOBYTE(HIWORD(val));
            phex[2] = HIBYTE(LOWORD(val));
            phex[3] = LOBYTE(LOWORD(val));
        }
        else {
            phex[3] = HIBYTE(HIWORD(val));
            phex[2] = LOBYTE(HIWORD(val));
            phex[1] = HIBYTE(LOWORD(val));
            phex[0] = LOBYTE(LOWORD(val));
        }
    } break;
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL hex2float(u08_t *phex, float *fval, float scale, int len, int flag)
{
    switch (len) {
    case 1: {
        *fval = (float)(*phex);
    } break;
    case 2: {
        if (HEX_POSITIVE == flag)
            *fval = (float)MAKEWORD(phex[1], phex[0]);
        else
            *fval = (float)MAKEWORD(phex[0], phex[1]);
    } break;
    case 3: {
        if (HEX_POSITIVE == flag)
            *fval = (float)MAKEWORD(phex[2], phex[1]) + phex[0] * 0x10000;
        else
            *fval = (float)MAKEWORD(phex[0], phex[1]) + phex[3] * 0x10000;
    } break;
    case 4: {
        if (HEX_POSITIVE == flag)
            *fval = (float)MAKEDWORD(MAKEWORD(phex[3], phex[2]),
                                     MAKEWORD(phex[1], phex[0]));
        else
            *fval = (float)MAKEDWORD(MAKEWORD(phex[0], phex[1]),
                                     MAKEWORD(phex[2], phex[3]));
    } break;
    default:
        return FALSE;
        break;
    }

    *fval = *fval * scale;
    return TRUE;
}

unsigned long asc2hex(char *buf)
{
    u08_t i = 0;
    unsigned long tempvalue = 0;
    unsigned long value = 0;
    u08_t len = strlen(buf);

    for (i = 0; i < len; i++) {
        if ((buf[i] >= 'A') && (buf[i] <= 'F')) {
            tempvalue = buf[i] - 'A' + 10;
        }
        if ((buf[i] >= 'a') && (buf[i] <= 'f')) {
            tempvalue = buf[i] - 'a' + 10;
        }
        if ((buf[i] >= '0') && (buf[i] <= '9')) {
            tempvalue = buf[i] - '0';
        }
        value = value * 16 + tempvalue;
    }

    return value;
}

void hex2asc(unsigned char dat, unsigned char **asc)
{
    **asc = ascii_table[dat >> 4];
    *asc += 1;
    **asc = ascii_table[dat & 0x0f];
    *asc += 1;
}

unsigned long asctohex_order(char *buf, u08_t len, u08_t byFlag)
{
    u08_t i = 0, j = 0;
    unsigned long tempvalue = 0;
    unsigned long value = 0;

    //判断长度
    if (8 <= len) {
        len = 8;
    }

    for (i = 0; i < len; i++) {
        if (1 == byFlag) //正序
        {
            j = i;
        }
        else if (0 == byFlag) //倒序
        {
            j = len - i;
        }

        //转化过程
        if ((buf[j] >= 'A') && (buf[j] <= 'F')) {
            tempvalue = buf[j] - 'A' + 10;
        }
        if ((buf[j] >= 'a') && (buf[j] <= 'f')) {
            tempvalue = buf[j] - 'a' + 10;
        }
        if ((buf[j] >= '0') && (buf[j] <= '9')) {
            tempvalue = buf[j] - '0';
        }

        value = value * 16 + tempvalue;
    }

    return value;
}

unsigned char cal_cs(const unsigned char *buf, int len)
{
    unsigned char byRtn = 0x00;
    int i;

    if (buf == NULL || len <= 0)
        return byRtn;

    for (i = 0; i < len; i++) {
        byRtn += buf[i];
    }

    return byRtn;
}

u08_t *add33h(u08_t *psrc, u08_t *pdes, int len, int flag)
{
    int i;

    if (NULL == psrc || NULL == pdes)
        return NULL;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;

        if (flag == DLT645_BCD_POSITIVE)
            pos = i;

        pdes[pos] = psrc[i] + 0x33;
    }

    return pdes;
}

u08_t *sub33h(u08_t *psrc, u08_t *pdes, int len, int flag)
{
    int i;

    if (NULL == psrc || NULL == pdes)
        return NULL;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;

        if (flag == DLT645_BCD_POSITIVE)
            pos = i;

        pdes[pos] = psrc[i] - 0x33;
    }

    return pdes;
}

u32_t pack_dword_di(u08_t *DI)
{
    return MAKEDWORD(MAKEWORD(DI[0], DI[1]), MAKEWORD(DI[2], DI[3]));
}

void *memcpydir(void *des, void *src, int len, int dir)
{
    int i;

    if (NULL == src || NULL == des)
        return NULL;

    if (len == 0)
        return NULL;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;
        if (dir == DIR_POSITIVE)
            pos = i;

        *((char *)des + pos) = *((char *)src + i);
    }

    return des;
}

int memcmpdir(void *des, void *src, int len, int dir)
{
    int i;

    if (NULL == src || NULL == des)
        return -2;

    if (len == 0)
        return -2;

    for (i = 0; i < len; i++) {
        int pos = len - i - 1;
        char ch_des, ch_src;

        if (dir == DIR_POSITIVE)
            pos = i;

        ch_des = *((char *)des + pos);
        ch_src = *((char *)src + i);

        if (ch_des > ch_src)
            return 1;
        else if (ch_des < ch_src)
            return -1;
    }

    return 0;
}

float u16tof(u16_t val, float scale)
{
    return ((float)val * scale);
}

float s16tof(s16_t val, float scale)
{
    /* if ( val < 0 ) { */
    /* 	float fval; */
    /* 	fval = (float)(0-val); */
    /* 	fval *= scale; */
    /* 	fval = 0 - fval; */

    /* 	return fval; */
    /* } */

    return ((float)val * scale);
}

float u32tof(u32_t val, float scale)
{
    return ((float)val * scale);
}

float s32tof(s32_t val, float scale)
{
    /* if ( val < 0 ) { */
    /* 	float fval; */
    /* 	fval = (float)(0-val); */
    /* 	fval *= scale; */
    /* 	fval = 0 - fval; */

    /* 	return fval; */
    /* } */

    return ((float)val * scale);
}

u16_t ftou16(float fval, float scale)
{
    return (u16_t)(fval * scale);
}

s16_t ftos16(float fval, float scale)
{
    return (s16_t)(fval * scale);
}

u32_t ftou32(float fval, float scale)
{
    return (u32_t)(fval * scale);
}

s32_t ftos32(float fval, float scale)
{
    return (s32_t)(fval * scale);
}

u08_t public_cal_weekday(int y, u08_t m, u08_t d)
{
    u08_t w;
    y += 2000;
    if ((m == 1) || (m == 2)) {
        m += 12;
        y--;
    }
    w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
    return (w + 1) % 7;
}
