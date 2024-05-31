/**
 *   \file protocol_gw1376_2_public.h
 *   \brief 协议用到的一些公用函数
 */
#ifndef _PROTOCOL_GW1376_2_PUBLIC_H_
#define _PROTOCOL_GW1376_2_PUBLIC_H_

#include "gw1376_2_data.h"

void set_addr_region_dest(char *src, PROTOCOL_GW1376_2_DATA *pdata);

int protocol_gw1376_2_process_data(char *buf, int len,
                                   PROTOCOL_GW1376_2_DATA *pdata);

int protocol_gw1376_2_process_cco_buf(char *buf, int len);
/**
 *  \brief 找到收到buf中第一个字节正确的位置
 *  \param BYTE *buf
 *  \param int *len
 *  \param pdata 协议指针
 *  \return pos
 */

void SetZlogP(zlog_category_t *zlog);
int protocol_gw1376_2_pos_valid_buf(char *buf, int len,
                                    PROTOCOL_GW1376_2_DATA *pdata, int pos);

/**
 *  \brief 获取数据类型的报文
 *  \param BYTE *buf
 *  \param int *len
 *  \param pdata 协议指针
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_2_check_buf(char *buf, int len,
                                PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief 找到正确的报文
 *  \param BYTE *buf
 *  \param int *len
 *  \param pdata 协议指针
 *  \return pos >= 0 正确报文的位置
 *  \return pos < 0 非正确报文
 */
int protocol_gw1376_2_find_valid_buf(char *buf, int len,
                                     PROTOCOL_GW1376_2_DATA *pdata, int pos);
/**
 *  \brief 打包数据
 *  \param buf 缓存
 *  \param &len
 *  \return >0 数据长度
 *  \return <=0 失败
 */

int protocol_gw1376_pack_frame_data(char *buf, int *len,
                                    PROTOCOL_GW1376_2_DATA *pdata);
/**
 *  \brief 主动问 打包数据
 *  \param buf 缓存
 *  \param &len
 *  \return >0 数据长度
 *  \return <=0 失败
 */
int protocol_gw1376_pack_frame_data_report(char *buf, int *len,
                                           PROTOCOL_GW1376_2_DATA *pdata, unsigned char count);

/**
 *  \brief 解包数据
 *  \param buf 缓存
 *  \param &len
 *  \return >0 数据长度
 *  \return <=0 失败
 */
int protocol_gw1376_unpack_frame_data(char *buf, int len,
                                      PROTOCOL_GW1376_2_DATA *pdata);

#endif /* _PROTOCOL_GW1376_2_PUBLIC_H_ */
