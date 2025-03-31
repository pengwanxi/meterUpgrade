#include "gw1376_2_public.h"
#include "global.h"
#include "gw1376_2_AFN.h"
#include "dlt645_07_api.h"
#include "stringAPI.h"
#include <stdio.h>
#include <stdlib.h>

zlog_category_t *zlogU;

void SetZlogP(zlog_category_t *zlog)
{
    zlogU = zlog;
}

int protocol_gw1376_2_process_cco_buf(char *buf, int len)
{
    PROTOCOL_GW1376_2_DATA *pprotocol_data = get_pdata();
    PROTOCOL_GW1376_2_RECV_DATA *precv =
        protocol_gw1376_2_recv_data_get(pprotocol_data);

    int pos = 0;
    int error = 0;

    while (pos <= len && pos >= 0)
    {
        int rtn;
        /* 找到有效报文 */
        pos = protocol_gw1376_2_find_valid_buf(buf, len, pprotocol_data, pos);
        if (pos < 0)
        {
            return -1;
        }

        // printf("value pos = [%d] \n", pos);
        rtn = protocol_gw1376_2_process_data(buf + pos, precv->value_len,
                                             pprotocol_data);
        if (rtn < 0)
        {
            pos += precv->value_len;
            error = -1;
            continue;
        }

        return rtn;
    }

    return error;
}

int protocol_gw1376_2_process_data(char *buf, int len,
                                   PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    PROTOCOL_GW1376_2_APPLY_REGION *papplydata = &precv->apply_region;
    int rtn;
    bool prm;
    if (protocol_gw1376_unpack_frame_data(buf, len, pdata) < 0)
    {
        zlog_info(zlogU, "%s unpack error", __FUNCTION__);
        return -1;
    }

    prm = protocol_gw1376_2_data_set_recv_prm(pdata);

    return protocol_gw1376_TotalAFN_up(pdata);
}

unsigned char protocol_gw1376_2_cal_fn(unsigned char DT1, unsigned char DT2)
{
    int i;

    for (i = 7; i >= 0; i--)
    {
        if (BIT_GET(DT1, i))
        {
            return (DT2 * 8) + i + 1;
        }
    }

    return 0;
}

int protocol_gw1376_2_dec_fn(unsigned char fn, unsigned char *DT1,
                             unsigned char *DT2)
{
    *DT1 = 1 << ((fn - 1) % 8);
    *DT2 = (fn - 1) / 8;
    return 0;
}

int protocol_gw1376_2_pos_valid_buf(char *buf, int len,
                                    PROTOCOL_GW1376_2_DATA *pdata, int pos)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);

    while (len - pos > 0)
    {
        if (buf[pos] == 0x68)
        {
            int tmp_len = MAKEWORD(buf[pos + 1], buf[pos + 2]);
            if (tmp_len > (len - pos))
            {
                pos++;
                continue;
            }
            if (buf[pos + tmp_len - 1] == 0x16)
            {
                precv->value_len = tmp_len;
                return pos;
            }
        }

        pos++;
    }

    return -1;
}

int protocol_gw1376_2_check_buf(char *buf, int len,
                                PROTOCOL_GW1376_2_DATA *pdata)
{
    /* 输入 */
    if (NULL == buf || len < 11)
    {
        zlog_error(zlogU, "%s input error ", __FUNCTION__);
        return -1;
    }

    /* 头 */
    if (0x68 != buf[0])
    {
        zlog_error(zlogU, "%s head error ", __FUNCTION__);
        return -1;
    }

    /* 判断长度 */
    if (len != MAKEWORD(buf[1], buf[2]))
    {
        zlog_error(zlogU, "%s len error ", __FUNCTION__);
        return -1;
    }

    /* 判断地址 */

    /* 判断cs */
    unsigned char cs = cal_cs((unsigned char *)buf, len - 2);
    if (cs == buf[len - 2])
    {
        zlog_error(zlogU, "%s cs %.2x!=%.2x error ", __FUNCTION__, cs, buf[len - 2]);
        return -1;
    }

    /* 判断结束 */
    if (buf[len - 1] != 0x16)
    {
        zlog_error(zlogU, "%s end error ", __FUNCTION__);
        return -1;
    }

    return 0;
}

int protocol_gw1376_2_find_valid_buf(char *buf, int len,
                                     PROTOCOL_GW1376_2_DATA *pdata, int pos)
{
    pos = protocol_gw1376_2_pos_valid_buf(buf, len, pdata, pos);
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    if (pos < 0 || pos > len)
    {
        zlog_error(zlogU, "%s pos error", __FUNCTION__);
        return -1;
    }

    zlog_info(zlogU, "%s len=%.3d ", __FUNCTION__, precv->value_len);

    if (protocol_gw1376_2_check_buf(buf + pos, precv->value_len, pdata) < 0)
    {
        zlog_error(zlogU, "%s check error ", __FUNCTION__);
        return -1;
    }

    return pos;
}

/**
 *  \brief 打包控制域
 *  \param pdata
 *  \return 0xff 失败
 *  \return 其它 成功
 */
char protocol_gw1376_pack_ctrl_data(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);

    char ctrl = psend->comm_type & 0x3f;

    if (psend->dir)
        BIT_SET(ctrl, 7);

    if (psend->prm)
        BIT_SET(ctrl, 6);

    return ctrl;
}

/**
 *  \brief 解包控制域
 *  \param pdata
 *  \return 0xff 失败
 *  \return 其它 成功
 */
char protocol_gw1376_unpack_ctrl_data(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);

    precv->comm_type = precv->ctrl_region & 0x3f;

    precv->dir = BIT_GET(precv->ctrl_region, 7);
    precv->prm = BIT_GET(precv->ctrl_region, 6);

    zlog_info(zlogU, "%s : [comm_type=%d , dir=%d , prm=%d] ", __FUNCTION__, precv->comm_type, precv->dir,
              precv->prm);
    return 0;
}

/**
 *  \brief 打包信息域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_pack_user_info_region(PROTOCOL_GW1376_2_INFO_REGION *pinfo,
                                          PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    unsigned short baud;

    if (NULL == pinfo)
        return -1;

    memset(pinfo, 0, sizeof(PROTOCOL_GW1376_2_INFO_REGION));

    /* data0 */
    if (psend->route_mode)
        BIT_SET(pinfo->data[0], 0);

    if (psend->subnode)
        BIT_SET(pinfo->data[0], 1);

    if (psend->comm_module_flag)
        BIT_SET(pinfo->data[0], 2);

    if (psend->conflict)
        BIT_SET(pinfo->data[0], 3);

    pinfo->data[0] |= (psend->relay_level << 4);

    /* data1 */
    pinfo->data[1] = (psend->coding << 4) | psend->channel;

    /* data2 */
    pinfo->data[2] = psend->bytes_count;

    /* data3-4 */
    baud = psend->comm_baud;
    if (psend->comm_baud_flag)
        BIT_SET(baud, 15);
    pinfo->data[3] = LOBYTE(baud);
    pinfo->data[4] = HIBYTE(baud);

    /* data5 */
    MOD_INC(pdata->buf_count, 0xff);
    pinfo->data[5] = pdata->buf_count;

    return 0;
}

/**
 *  \brief 打包信息域 hplc主动上报后的序号
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_pack_user_info_region_report(PROTOCOL_GW1376_2_INFO_REGION *pinfo,
                                                 PROTOCOL_GW1376_2_DATA *pdata, unsigned char count)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    unsigned short baud;

    if (NULL == pinfo)
        return -1;

    memset(pinfo, 0, sizeof(PROTOCOL_GW1376_2_INFO_REGION));

    /* data0 */
    if (psend->route_mode)
        BIT_SET(pinfo->data[0], 0);

    if (psend->subnode)
        BIT_SET(pinfo->data[0], 1);

    if (psend->comm_module_flag)
        BIT_SET(pinfo->data[0], 2);

    if (psend->conflict)
        BIT_SET(pinfo->data[0], 3);

    pinfo->data[0] |= (psend->relay_level << 4);

    /* data1 */
    pinfo->data[1] = (psend->coding << 4) | psend->channel;

    /* data2 */
    pinfo->data[2] = psend->bytes_count;

    /* data3-4 */
    baud = psend->comm_baud;
    if (psend->comm_baud_flag)
        BIT_SET(baud, 15);
    pinfo->data[3] = LOBYTE(baud);
    pinfo->data[4] = HIBYTE(baud);

    /* data5 */
    // MOD_INC(pdata->buf_count, 0xff);
    pinfo->data[5] = count;

    return 0;
}

/**
 *  \brief 解包信息域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_unpack_user_info_region(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    PROTOCOL_GW1376_2_INFO_REGION *pinfo = &precv->info_region;
    unsigned short baud;

    /* data0 */
    precv->route_mode = BIT_GET(pinfo->data[0], 0);

    precv->subnode = BIT_GET(pinfo->data[0], 1);
    // zlog_info(zlogU, "subnode = %d , ", precv->subnode);

    precv->comm_module_flag = BIT_GET(pinfo->data[0], 2);
    // zlog_info(zlogU, "comm_module_flag = %d , ", precv->comm_module_flag);

    precv->conflict = BIT_GET(pinfo->data[0], 3);
    // zlog_info(zlogU, "conflict = %d , ", precv->conflict);

    precv->relay_level = (pinfo->data[0] >> 4);
    // zlog_info(zlogU, "relay_level = %d , ", precv->relay_level);
    /* data1 */
    precv->channel = pinfo->data[1] & 0x0f;
    // zlog_info(zlogU, "channel = %d , ", precv->channel);

    precv->coding = pinfo->data[1] >> 4;
    // zlog_info(zlogU, "coding = %d , ", precv->coding);

    /* data2 */
    precv->bytes_count = pinfo->data[2];
    // zlog_info(zlogU, "bytes_count = %d , ", precv->bytes_count);

    /* data3-4 */
    baud = MAKEWORD(pinfo->data[3], pinfo->data[4]);
    // zlog_info(zlogU, "baud = %d , ", precv->comm_baud);

    precv->comm_baud_flag = BIT_GET(baud, 15);
    // zlog_info(zlogU, "baud_flag = %d , ", precv->comm_baud_flag);

    /* data5 */
    // zlog_info(zlogU, "buf_count = %d ]", pinfo->data[5]);
    zlog_info(zlogU, "rounte_mode = %d , subnode = %d ,comm_module_flag = %d ,conflict = %d ,relay_level = %d , channel = %d ,coding = %d , bytes_count = %d , baud = %d , baud_flag = %d ,buf_count = %d",
              precv->route_mode, precv->subnode, precv->comm_module_flag, precv->conflict,
              precv->relay_level, precv->channel, precv->coding, precv->bytes_count, precv->comm_baud, precv->comm_baud_flag, pinfo->data[5]);

    /*
    if ((u08_t)pinfo->data[5] != pdata->buf_count)
    {
         pdata->send.subnode_auto_up = true; */
    /* protocol_gw1376_2_data_set_data_type( */
    /* 	pdata, GW1376_2_DATA_TYPE_WRITE_SUBNODE_AUTO_UP);

    zlog_error(zlogU, "%s error data buf_count=%d", __FUNCTION__, pdata->buf_count);
    return -1;
}*/

    return 0;
}

/**
 *  \brief 打包地址域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_pack_user_addr_region(PROTOCOL_GW1376_2_ADDR_REGION *paddr,
                                          PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    // APP_DATA *papp_data = app_data_get();
    // APP_CFG *pcfg = &papp_data->cfg;
    // DEV_DATA *pdev_data = comm_data_get_current_dev(&pdata->pcomm->data);

    if (NULL == paddr)
        return -1;

    /*
    XmlCfg *pcfg = (XmlCfg *)GetXmlCfg();
    XML_DATA *pxmldata = pcfg->GetData();

    public_stob(pxmldata->gw13762_addr, (unsigned char *)paddr->src, 6);

    protocol_gw1376_2_data_set_send_relay_level(pdata, 0);
    printf("%s %d", __FUNCTION__, psend->info_region.data[0]);
    paddr->relay_num = 0;

    u08_t *p645addr = dlt645_07_get_addr();
    // public_stob((char *)p645addr, (unsigned char *)paddr->dest, 6);
    // public_stob((char *)p645addr, (unsigned char *)paddr->dest, 6);
    // memcpy(p645addr, paddr->dest, 6);
    memcpydir(paddr->dest, p645addr, 6, DIR_REVERSE);
    */
    return 0;
}

/**
 *  \brief 解包地址域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_unpack_user_addr_region(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    /* APP_DATA *papp_data = app_data_get(); */
    /* APP_CFG *pcfg = &papp_data->cfg; */
    int index = 6;
    /* DEV_DATA *pdev_data = comm_data_get_current_dev(&pdata->pcomm->data); */
    PROTOCOL_GW1376_2_ADDR_REGION *paddr = &precv->addr_region;

    if (NULL == paddr)
        return -1;

    memcpy(paddr->src, pdata->user_data_buf + index, 6);
    // printf("src addr=");
    //  log_debug_frame(paddr->src, 6);

    if (pdata->isbroadcast)
    {
        int j;
        for (j = 0; j < 6; j++)
        {
            if ((unsigned char)paddr->src[j] != 0x99)
            {
                break;
            }
        }

        zlog_info(zlogU, "broadcast j=%d", j);
        if (j == 6)
        {
            zlog_info(zlogU, "broadcast switch");
            pdata->isbroadcast = false;
        }
    }

    index += 6;

    paddr->relay_num = precv->relay_level;

    memcpy(paddr->dest, pdata->user_data_buf + index, 6);
    // printf("dest addr=");
    //  log_debug_frame(paddr->dest, 6);
    index += 6;

    return index;
}

/**
 *  \brief 打包应用域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
static int protocol_gw1376_pack_user_apply_region(
    PROTOCOL_GW1376_2_APPLY_REGION *papplydata, PROTOCOL_GW1376_2_DATA *pdata)
{
    unsigned char DT1, DT2;
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    int index = 0;

    protocol_gw1376_2_dec_fn(papplydata->Fn, &DT1, &DT2);

    psend->apply_data[index++] = papplydata->AFN;

    psend->apply_data[index++] = DT1;
    psend->apply_data[index++] = DT2;

    if (papplydata->unit_len > 0)
    {
        memcpy(&psend->apply_data[index], papplydata->unit_buf,
               papplydata->unit_len);

        index += papplydata->unit_len;
    }

    psend->apply_data_len = index;
    return 0;
}

/**
 *  \brief 解包应用域
 *  \param pinfo
 *  \param pdata
 *  \return 0 成功
 *  \return <0 失败
 */
static int
protocol_gw1376_unpack_user_apply_region(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    int index = 0;
    PROTOCOL_GW1376_2_APPLY_REGION *papplydata = &precv->apply_region;
    
    papplydata->AFN = precv->apply_data[index];
    index++;

    papplydata->Fn = protocol_gw1376_2_cal_fn(precv->apply_data[index],
                                              precv->apply_data[index + 1]);
    index += 2;
    papplydata->unit_len = precv->apply_data_len - index;
    if (papplydata->unit_len > 0)
    {

        memcpy(papplydata->unit_buf, &precv->apply_data[index],
               papplydata->unit_len);

        index += papplydata->unit_len;
    }
    zlog_info(zlogU, "%s : [ AFN = %x , Fn = F%x , unit_len=%d ]", __FUNCTION__, papplydata->AFN, papplydata->Fn, papplydata->unit_len);
    // printf("Fn = F%x , ", papplydata->Fn);
    // printf("unit_len=%d ] \n", papplydata->unit_len);
    //  log_debug_frame(papplydata->unit_buf, papplydata->unit_len);

    return index;
}

/**
 *  \brief 打包数据
 *  \param buf 用户数据
 *  \param &len 用户数据长度
 *  \return >0 数据长度
 *  \return <=0 失败
 */

void set_addr_region_dest(char *src, PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    memcpy(psend->addr_region.dest, src, 6);
}

int protocol_gw1376_pack_user_data(char *buf, int *len,
                                   PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    int index = 0;

    if (psend->comm_module_flag == GW1376_2_STA)
        protocol_gw1376_pack_user_addr_region(&psend->addr_region, pdata);

    protocol_gw1376_pack_user_info_region(&psend->info_region, pdata);
    memcpy(buf, psend->info_region.data, 6);
    index += 6;

    if (psend->comm_module_flag == GW1376_2_STA)
    {
        int i;

        char src[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x01};
        memcpy(buf + index, src, 6);
        index += 6;

        for (i = 0; i < psend->addr_region.relay_num; i++)
        {
            memcpy(buf + index, psend->addr_region.relay[i], 6);
            index += 6;
        }
        memcpy(buf + index, psend->addr_region.dest, 6);
        index += 6;
    }

    protocol_gw1376_pack_user_apply_region(&psend->apply_region, pdata);
    memcpy(buf + index, psend->apply_data, psend->apply_data_len);
    index += psend->apply_data_len;

    *len = index;

    zlog_info(zlogU, "%s %d", __FUNCTION__, psend->info_region.data[0]);
    return index;
}

/**
 *  \brief 主动上报 打包数据
 *  \param buf 用户数据
 *  \param &len 用户数据长度
 *  \return >0 数据长度
 *  \return <=0 失败
 */
int protocol_gw1376_pack_user_data_report(char *buf, int *len,
                                          PROTOCOL_GW1376_2_DATA *pdata, unsigned char count)
{
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);
    int index = 0;

    if (psend->comm_module_flag == GW1376_2_STA)
        protocol_gw1376_pack_user_addr_region(&psend->addr_region, pdata);

    protocol_gw1376_pack_user_info_region_report(&psend->info_region, pdata, count);
    memcpy(buf, psend->info_region.data, 6);
    index += 6;

    if (psend->comm_module_flag == GW1376_2_STA)
    {
        int i;

        memcpy(buf + index, psend->addr_region.src, 6);
        index += 6;

        for (i = 0; i < psend->addr_region.relay_num; i++)
        {
            memcpy(buf + index, psend->addr_region.relay[i], 6);
            index += 6;
        }

        memcpy(buf + index, psend->addr_region.dest, 6);
        index += 6;
    }

    protocol_gw1376_pack_user_apply_region(&psend->apply_region, pdata);
    memcpy(buf + index, psend->apply_data, psend->apply_data_len);
    index += psend->apply_data_len;

    *len = index;

    zlog_info(zlogU, "%s %d", __FUNCTION__, psend->info_region.data[0]);
    return index;
}

/**
 *  \brief 解包数据
 *  \param buf 用户数据
 *  \param &len 用户数据长度
 *  \return >0 数据长度
 *  \return <=0 失败
 */
int protocol_gw1376_unpack_user_data(char *buf, int len,
                                     PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    int index = 0;

    memcpy(precv->info_region.data, buf + index, 6);
    if (protocol_gw1376_unpack_user_info_region(pdata) < 0)
    {
        zlog_error(zlogU, "protocol_gw1376_unpack_user_info_region error");
        return -1;
    }
    index += 6;

    if (precv->comm_module_flag == GW1376_2_STA)
    {
        index = protocol_gw1376_unpack_user_addr_region(pdata);
    }

    precv->apply_data_len = len - index;
    memcpy(precv->apply_data, buf + index, precv->apply_data_len);
    protocol_gw1376_unpack_user_apply_region(pdata);

    return pdata->user_data_len;
}

int protocol_gw1376_pack_frame_data(char *buf, int *len,
                                    PROTOCOL_GW1376_2_DATA *pdata)
{
    int index = 0;
    unsigned short data_len;
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);

    protocol_gw1376_pack_user_data(pdata->user_data_buf, &pdata->user_data_len,
                                   pdata);

    buf[index++] = 0x68;

    data_len = 6 + pdata->user_data_len;
    buf[index++] = LOBYTE(data_len);
    buf[index++] = HIBYTE(data_len);

    buf[index++] = psend->ctrl_region;

    memcpy(&buf[index], psend->info_region.data, 6);

    memcpy(&buf[index], pdata->user_data_buf, pdata->user_data_len);
    index += pdata->user_data_len;

    buf[index] = cal_cs((unsigned char *)(buf + 3), index - 3);
    index++;

    buf[index++] = 0x16;

    *len = index;
    // zlog_print(zlogU, "protocol_gw1376_pack_frame_data buf [ %s ] ", buf, index);
    zlog_info(zlogU, "%s %d %d", __FUNCTION__, psend->info_region.data[0], index);

    pdata->issending = true;
    pdata->sending_time = 0;
    return index;
}

int protocol_gw1376_pack_frame_data_report(char *buf, int *len,
                                           PROTOCOL_GW1376_2_DATA *pdata, unsigned char count)
{
    int index = 0;
    unsigned short data_len;
    PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata);

    protocol_gw1376_pack_user_data_report(pdata->user_data_buf, &pdata->user_data_len,
                                          pdata, count);

    buf[index++] = 0x68;

    data_len = 6 + pdata->user_data_len;
    buf[index++] = LOBYTE(data_len);
    buf[index++] = HIBYTE(data_len);

    buf[index++] = psend->ctrl_region;

    memcpy(&buf[index], psend->info_region.data, 6);
    memcpy(&buf[index], pdata->user_data_buf, pdata->user_data_len);
    index += pdata->user_data_len;

    buf[index] = cal_cs((unsigned char *)(buf + 3), index - 3);
    index++;

    buf[index++] = 0x16;

    *len = index;

    zlog_info(zlogU, "%s %d %d", __FUNCTION__, psend->info_region.data[0], index);
    pdata->issending = true;
    pdata->sending_time = 0;
    return index;
}

int protocol_gw1376_unpack_frame_data(char *buf, int len,
                                      PROTOCOL_GW1376_2_DATA *pdata)
{
    int index;
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);

    precv->frame_len = MAKEWORD(buf[1], buf[2]);

    precv->ctrl_region = buf[3];
    protocol_gw1376_unpack_ctrl_data(pdata);
    if (!precv->prm)
    {
        pdata->issending = false;
    }
    else
    {
        /* 不处理prm 的报文 */
        pdata->issending = false;
        // return 0;
    }

    index = 4;
    pdata->user_data_len = precv->frame_len - index - 2;
    memcpy(pdata->user_data_buf, &buf[index], pdata->user_data_len);

    if (protocol_gw1376_unpack_user_data(pdata->user_data_buf,
                                         pdata->user_data_len, pdata) < -1)
    {
        zlog_error(zlogU, "%s unpack error", __FUNCTION__);
        return -1;
    }

    return precv->frame_len;
}
