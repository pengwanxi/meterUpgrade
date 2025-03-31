#include "gw1376_2_AFN.h"
#include "gw1376_2_public.h"
#include "global.h"
#include "stringAPI.h"
#include "define.h"

static zlog_category_t *m_zlogA = NULL;

// std::map<std::string, METER_INFO> m_meterInfoMap;
std::map<std::string, METER_INFO> *m_meterInfoMap = NULL;

static GW103762DataTypeFunc_t gw103762DataTypeFunc[0xff] = {
    {GW1376_2_DATA_TYPE_ROUTE_ADD_SUBNODE, protocol_gw1376_AFN11_Fn01_up, NULL},
    {GW1376_2_DATA_TYPE_ROUTE_READ_SUBNODE_NUM, protocol_gw1376_AFN10_Fn01_up, NULL},
    {GW1376_2_DATA_TYPE_ROUTE_READ_SUBNODE_INFO, protocol_gw1376_AFN10_Fn02_up, NULL},
    {GW1376_2_DATA_TYPE_ROUTE_DEL_SUBNODE, protocol_gw1376_AFN11_Fn02_up, NULL},
    {GW1376_2_DATA_TYPE_PARAM_INIT, protocol_gw1376_AFN01_up, NULL},
    {GW1376_2_DATA_TYPE_TRANS_DATA, protocol_gw1376_AFN02_Fn01_up, NULL},
    {GW1376_2_DATA_TYPE_READ_MAIN_NODE, protocol_gw1376_AFN03_Fn04_up, NULL},
    {GW1376_2_DATA_TYPE_WRITE_MAIN_NODE, protocol_gw1376_AFN05_Fn01_up, NULL},
    {GW1376_2_DATA_TYPE_ROUTE_READ_NETWORK_TOPO_INFO, protocol_gw1376_AFN10_Fn21_up, NULL},

};

/*
 * AFN处理函数，不同返回值做不同处理  仅限ccoRouter主动下发返回的
 */
int protocol_gw1376_TotalAFN_up(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    int i;
    for (i = 0; i < 0xff; i++)
    {
        // 主动下发
        if (gw103762DataTypeFunc[i].dataType == (PROTOCOL_GW1376_2_DATA_TYPE)pdata->state && gw103762DataTypeFunc[i].FuncHandler != NULL)
        {
            int ret = gw103762DataTypeFunc[i].FuncHandler(pdata, gw103762DataTypeFunc[i].MqttPublish);
            pdata->state = -1;
            return ret;
        }
    }
    return 0;
}

void SetZlogPointer(zlog_category_t *zlog)
{
    if (zlog != NULL)
    {
        m_zlogA = zlog;

        SetZlogP(zlog);
    }
}

/**
 *  \brief 下行CCO 无单元数据
 *  \param pdata
 *  \param AFN
 *  \param FN
 *  \return 0
 */
static int
protoco_gw1376_2_cco_down_no_unit(PROTOCOL_GW1376_2_DATA *pdata,
                                  unsigned char AFN, unsigned Fn)
{
    /* PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata); */

    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_send_apply_region_get(pdata);

    protocol_gw1376_2_data_set_send_comm_module_flag(pdata, GW1376_2_CCO);

    papply_region->AFN = AFN;
    papply_region->Fn = Fn;
    papply_region->unit_len = 0;

    return 0;
}

/**
 *  \brief 下行CCO 无单元数据
 *  \param pdata
 *  \param AFN
 *  \param FN
 *  \return 0
 */
static int
protoco_gw1376_2_cco_down_unit(PROTOCOL_GW1376_2_DATA *pdata,
                               PROTOCOL_GW1376_2_APPLY_REGION *pregin)
{
    /* PROTOCOL_GW1376_2_SEND_DATA *psend = protocol_gw1376_2_send_data_get(pdata); */

    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_send_apply_region_get(pdata);

    protocol_gw1376_2_data_set_send_comm_module_flag(pdata, GW1376_2_CCO);

    papply_region->AFN = pregin->AFN;
    papply_region->Fn = pregin->Fn;
    papply_region->unit_len = 0;
    if (BETWEEN(pregin->unit_len, 1, PROTOCOL_GW1376_2_APPLY_DATA_LEN))
    {
        papply_region->unit_len = pregin->unit_len;
        memcpy(papply_region->unit_buf, pregin->unit_buf, pregin->unit_len);
    }

    return 0;
}

int protocol_gw1376_AFN01_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata)
{
    return protoco_gw1376_2_cco_down_no_unit(pdata, 0x01, 0x01);
}

/*
 * 设置主节点地址
 */
int GW1376AFN05Fn01down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN05_Fn01_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN05_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{

    pdata->state = GW1376_2_DATA_TYPE_WRITE_MAIN_NODE;
    PROTOCOL_GW1376_2_APPLY_REGION regin;
    regin.AFN = 0x05;
    regin.Fn = 0x01;
    memset(regin.unit_buf, 0, sizeof(regin.unit_buf));
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);

    memset(buffer, 0, sizeof(buffer));
    int bufSize;

    // CCOUartSend(buf, bufSize);

    // zlog_print(m_zlogA, "Send gw10376.2 data frame AFN=[05] Fn = [01] : [%s]", buf, bufSize);

    return protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);
}
/*
 * 设置主节点地址 返回数据处理
 */
int protocol_gw1376_AFN05_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);

    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;

    if (pdata->recv.apply_region.AFN != 0x00)
    {
        zlog_error(m_zlogA, "%s AFN=%d error", __FUNCTION__, pdata->recv.apply_region.AFN);
        return -1;
    }
    unsigned char ack;
    ack = (unsigned char)papply_region->Fn;
    moduleInfo->isSuccessSetAddr = (int)papply_region->Fn;

    return (int)papply_region->Fn;
}

/*
 * 读取主节点地址
 */
int GW1376AFN03Fn04down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN03_Fn04_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN03_Fn04_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_READ_MAIN_NODE;
    protoco_gw1376_2_cco_down_no_unit(pdata, 0x03, 0x04);

    memset(buffer, 0, sizeof(buffer));
    int bufSize;
    bufSize = protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);

    return bufSize;
}
/*
 * 读取主节点地址回复
 */
int protocol_gw1376_AFN03_Fn04_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;

    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    if (papply_region->AFN != 3 || papply_region->Fn != 4)
    {
        zlog_error(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        return -1;
    }
    char pubBuffer[6] = {0};
    CopyArray(papply_region->unit_buf, pubBuffer, 6);
    CopyArray(papply_region->unit_buf, (char *)moduleInfo->moduleAddr, 6);
    // zlog_print(m_zlogA, "moduleInfo->moduleAddr : [%s] ", (char *)moduleInfo->moduleAddr, 6);

    return 0;
}

/*
 * 从节点档案信息初始化（参数初始化）
 */
int GW1376AFN01Fn02down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN01_Fn02_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN01_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_PARAM_INIT;
    protoco_gw1376_2_cco_down_no_unit(pdata, 0x01, 0x02);
    int bufSize;

    memset(buffer, 0, sizeof(buffer));
    return protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);
}

int protocol_gw1376_AFN01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);

    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;

    if (pdata->recv.apply_region.AFN != 0x00)
    {
        zlog_error(m_zlogA, "%s AFN=%d error", __FUNCTION__, pdata->recv.apply_region.AFN);
        return -1;
    }
    unsigned char ack;
    ack = (unsigned char)papply_region->Fn;
    moduleInfo->isInit = (int)papply_region->Fn;

    /*
    if (MqttPublish != NULL)
    {
        if (MqttPublish(&ack, sizeof(ack)) < 0)
        {
            return -1;
        }
    }*/
    return moduleInfo->isInit;
}

int protocol_gw1376_AFN01_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata)
{
    return protoco_gw1376_2_cco_down_no_unit(pdata, 0x01, 0x03);
}

/*
 *   转发数据
 */
int GW1376AFN02Fn01down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN02_Fn01_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN02_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_TRANS_DATA;
    PROTOCOL_GW1376_2_APPLY_REGION regin;

    regin.AFN = 0x02;
    regin.Fn = 0x01;
    memset(regin.unit_buf, 0, sizeof(regin.unit_buf));
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);

    protocol_gw1376_2_data_set_send_comm_module_flag(pdata, GW1376_2_STA);
    memset(buffer, 0, sizeof(buffer));
    bufferLen = protocol_gw1376_pack_frame_data((char *)buffer, &bufferLen, pdata);

    // CCOUartSend(buf, bufSize);
    // zlog_print(m_zlogA, "Send gw10376.2 data frame AFN=[02] Fn = [01] : [%s]", (char *)buffer, bufferLen);

    return bufferLen;
}
/*
 * 转发数据回复
 */
int protocol_gw1376_AFN02_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;

    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    if (papply_region->AFN != 2 || papply_region->Fn != 1)
    {
        zlog_error(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        return -1;
    }

    return 0;

    char *pubBuffer = (char *)malloc(sizeof(char) * papply_region->unit_len - 2);

    if (pubBuffer == NULL)
    {
        zlog_error(m_zlogA, "malloc failed!\n", __FUNCTION__);
        return -1;
    }
    memset(pubBuffer, 0, sizeof(char) * papply_region->unit_len - 2);
    memcpy(pubBuffer, papply_region->unit_buf + 2, sizeof(char) * papply_region->unit_len - 2);

    /*
    if (MqttPublish != NULL)
    {
        if (MqttPublish(pubBuffer, sizeof(pubBuffer)) < 0)
        {
            return -1;
        }
    }*/
    return 0;
}

int protocol_gw1376_AFN03_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata)
{
    PROTOCOL_GW1376_2_APPLY_REGION regin;

    regin.AFN = 0x3;
    regin.Fn = 3;
    regin.unit_len = 2;

    /*
    int val;
    val = GetValStrInt(0);
    regin.unit_buf[0] = val;
    val = GetValStrInt(1);
    regin.unit_buf[1] = val;
    */

    return protoco_gw1376_2_cco_down_unit(pdata, &regin);
}

int protocol_gw1376_AFN03_Fn03_up(PROTOCOL_GW1376_2_DATA *pdata)
{
    /* APP_DATA *papp_data = app_data_get(); */
    // COMM_DATA *pcomm_data = &pdata->pcomm->data;
    /* APP_CFG *pcfg = &papp_data->cfg; */
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    int i;
    unsigned char addr[6];

    if (papply_region->AFN != 0x10 || papply_region->Fn != 2)
    {
        zlog_error(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        // comm_data_set_step(pcomm_data, GW1376_2_DATA_TYPE_PARAM_INIT);
        return -1;
    }
    /*
    XmlCfg *pcfg = (XmlCfg *)GetXmlCfg();
    CFG_SUBTREE *psubtree = pcfg->GetCurrentSubTree();
    CFG_SUBTREE_DATA *psubdata = &psubtree->pdata[0];
    int start = 2;
    int pos = 0;
    for (i = 0; i < psubtree->data_num; i++) {
        process_buf_by_subdata((u08_t *)papply_region->unit_buf + pos, psubdata,
                               start++);

        pos += psubdata->length;
    }*/
    return 0;
}

int protocol_gw1376_AFN03_Fn10_down(PROTOCOL_GW1376_2_DATA *pdata)
{
    return protoco_gw1376_2_cco_down_no_unit(pdata, 0x03, 10);
}

int protocol_gw1376_AFN03_Fn10_up(PROTOCOL_GW1376_2_DATA *pdata)
{
    /* PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata); */
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    unsigned char addr[6];

    if (papply_region->AFN != 3 || papply_region->Fn != 10)
    {
        zlog_error(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        return -1;
    }

    if (papply_region->unit_len < 41)
    {
        zlog_error(m_zlogA, "%s unit_len error", __FUNCTION__);
        return -1;
    }

    /* 判断通信方式 */
    if ((papply_region->unit_buf[0] & 0x0f) != 2)
    {
        zlog_error(m_zlogA, "%s comm_flag != 2", __FUNCTION__);
        return -1;
    }

    if (BIT_GET(papply_region->unit_buf[0], 4))
    {
        protocol_gw1376_2_data_set_send_route_mode(pdata, GW1376_2_ROUTE_MODE);
    }
    else
    {
        protocol_gw1376_2_data_set_send_route_mode(pdata,
                                                   GW1376_2_ROUTE_MODE_NO);
    }

    return 0;
}

/*
 * 查询从节点数量
 */
int GW1376AFN10Fn01down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN10_Fn01_down(protoData);
}
int protocol_gw1376_AFN10_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata)
{
    pdata->state = GW1376_2_DATA_TYPE_ROUTE_READ_SUBNODE_NUM;
    protoco_gw1376_2_cco_down_no_unit(pdata, 0x10, 1);

    char buf[65535] = {0};
    int bufSize;
    protocol_gw1376_pack_frame_data(buf, &bufSize, pdata);
    // CCOUartSend(buf, bufSize);
    // zlog_print(m_zlogA, "Send gw10376.2 data frame AFN=[10] Fn = [01] : [%s]", buf, bufSize);

    return 0;
}
/*
 * 查询从节点数量返回数据处理
 */
int protocol_gw1376_AFN10_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;
    if (papply_region->AFN != 0x10 || papply_region->Fn != 1)
    {
        return -1;
    }

    if (papply_region->unit_len != 4)
    {
        zlog_error(m_zlogA, "%s unit_len error", __FUNCTION__);
        return -1;
    }

    char tmp[2] = {0};
    memcpy(tmp, papply_region->unit_buf, sizeof(tmp) / sizeof(char));

    int count = tmp[1] << 8 | tmp[0];
    moduleInfo->archCount = count;

    /*
    if (MqttPublish != NULL)
    {
        if (MqttPublish(tmp, sizeof(tmp)) < 0)
        {
            return -1;
        }
    }
    */
    return 0;
}

/*
 * 查询档案
 */
int GW1376AFN10Fn02down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN10_Fn02_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN10_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_ROUTE_READ_SUBNODE_INFO;
    PROTOCOL_GW1376_2_APPLY_REGION regin;
    regin.AFN = 0x10;
    regin.Fn = 2;
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);
    int bufSize;
    memset(buffer, 0, sizeof(buffer));
    return protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);
}

/*
 * 查询档案  串口返回数据处理
 */
int protocol_gw1376_AFN10_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    int i, j;

    if (papply_region->AFN != 0x10 || papply_region->Fn != 2)
    {
        zlog_info(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        // comm_data_set_step(pcomm_data, GW1376_2_DATA_TYPE_PARAM_INIT);
        return -1;
    }

    u16_t total = MAKEWORD(papply_region->unit_buf[0], papply_region->unit_buf[1]);
    u08_t num = papply_region->unit_buf[2];
    zlog_info(m_zlogA, "total : [%d]  num : [%d]", total, num);

    if (num == 0)
    {
        zlog_info(m_zlogA, "无节点应答");
        return 0;
    }

    char *unitBufTemp = NULL;
    unitBufTemp = papply_region->unit_buf + 3;

    char pubBuffer[6] = {0};

    for (i = 0; i < num; i++)
    {

        METER_INFO meterInfo;
        CopyArray(unitBufTemp + i * 8, (char *)meterInfo.addr, 6);
        meterInfo.phase = (unitBufTemp[i * 8 + 7]) & 0b00000111;          // 获取相位
        meterInfo.protoType = (unitBufTemp[i * 8 + 7] >> 3) & 0b00000111; // 获取协议类型

        char msg[13] = {0};
        hexArrayToString((char *)meterInfo.addr, 6, msg);
        std::string addrkey = msg;
        auto mInfo = m_meterInfoMap->find(addrkey);
        if (mInfo != m_meterInfoMap->end())
        {
            // zlog_info(m_zlogA, "addr : [%s]", msg);
            mInfo->second.phase = meterInfo.phase;
            mInfo->second.protoType = meterInfo.protoType;
        }
        // hdzlog_info(meterInfo.addr, 6);
        //  zlog_info(m_logc, "meterInfo[i].phase : %d", meterInfo.phase);
        //   meterInfoVector.push_back(meterInfo);
        //    zlog_info(m_logc, "meterInfo[i].phase : %d", meterInfo.phase);
        //    printf("extractedBits : [%d] , protoType : [%d] \n", extractedBits, proType);
    }
    return 0;
}

std::map<std::string, METER_INFO> *GetMeterInfo()
{
    return m_meterInfoMap;
}

void SetMeterInfo(std::map<std::string, METER_INFO> *info)
{

    m_meterInfoMap = info;
    // return &m_meterInfoMap;
}

/*
 * 添加从节点
 */
int GW1376AFN11Fn01down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN11_Fn01_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN11_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;
    pdata->state = GW1376_2_DATA_TYPE_ROUTE_ADD_SUBNODE;
    PROTOCOL_GW1376_2_APPLY_REGION regin;
    regin.AFN = 0x11;
    regin.Fn = 1;
    memset(regin.unit_buf, 0, sizeof(regin.unit_buf));
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);

    memset(buffer, 0, sizeof(buffer));
    int bufSize;
    protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);

    return bufSize;
}

/*
 * 添加从节点返回数据处理
 */
int protocol_gw1376_AFN11_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    // COMM_DATA *pcomm_data = &pdata->pcomm->data;
    ModuleInfo_t *moduleInfo = &pdata->moduleInfo;
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);

    if (pdata->recv.apply_region.AFN != 0x00)
    {
        zlog_info(m_zlogA, "%s AFN=%d error", __FUNCTION__, pdata->recv.apply_region.AFN);
        return -1;
    }
    unsigned char ack;
    ack = (unsigned char)papply_region->Fn;
    moduleInfo->isAddArch = (int)papply_region->Fn;

    /*
    if (MqttPublish != NULL)
    {
        if (MqttPublish(&ack, sizeof(ack)) < 0)
        {
            return -1;
        }
    }
*/
    return (int)papply_region->Fn;
}

/*
 * 删除从节点下发
 */
int GW1376AFN11Fn02down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN11_Fn02_down(protoData, buffer, bufferLen);
}

int protocol_gw1376_AFN11_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_ROUTE_DEL_SUBNODE;
    PROTOCOL_GW1376_2_APPLY_REGION regin;
    regin.AFN = 0x11;
    regin.Fn = 2;
    memset(regin.unit_buf, 0, sizeof(regin.unit_buf));
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);

    char buf[65535] = {0};
    int bufSize;
    protocol_gw1376_pack_frame_data(buf, &bufSize, pdata);

    // CCOUartSend(buf, bufSize);

    // zlog_print(m_zlogA, "Send gw10376.2 data frame AFN=[11] Fn = [02] : [%s]", buf, bufSize);
    free(buffer);
    return 0;
}
/*
 * 删除从节点返回数据处理
 */
int protocol_gw1376_AFN11_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);

    if (pdata->recv.apply_region.AFN != 0x00)
    {
        zlog_info(m_zlogA, "%s AFN=%d error", __FUNCTION__, pdata->recv.apply_region.AFN);
        return -1;
    }
    unsigned char ack;
    ack = (unsigned char)papply_region->Fn;

    /*
    if (MqttPublish != NULL)
    {
        if (MqttPublish(&ack, sizeof(ack)) < 0)
        {
            return -1;
        }
    }*/
    return 0;
}

/*
 * 查询网络拓扑信息
 */
int GW1376AFN10Fn21down(unsigned char *buffer, int bufferLen)
{
    PROTOCOL_GW1376_2_DATA *protoData = get_pdata();
    return protocol_gw1376_AFN10_Fn21_down(protoData, buffer, bufferLen);
}
int protocol_gw1376_AFN10_Fn21_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen)
{
    pdata->state = GW1376_2_DATA_TYPE_ROUTE_READ_NETWORK_TOPO_INFO;
    PROTOCOL_GW1376_2_APPLY_REGION regin;
    regin.AFN = 0x10;
    regin.Fn = 21;
    memcpy(regin.unit_buf, buffer, bufferLen);
    regin.unit_len = bufferLen;
    protoco_gw1376_2_cco_down_unit(pdata, &regin);
    int bufSize;
    memset(buffer, 0, sizeof(buffer));
    return protocol_gw1376_pack_frame_data((char *)buffer, &bufSize, pdata);
}

/*
 * 查询网络拓扑信息  串口返回数据处理
 */
int protocol_gw1376_AFN10_Fn21_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen))
{
    PROTOCOL_GW1376_2_RECV_DATA *precv = protocol_gw1376_2_recv_data_get(pdata);
    PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
        protocol_gw1376_2_recv_apply_region_get(pdata);
    int i, j;

    if (papply_region->AFN != 0x10 || papply_region->Fn != 21)
    {
        zlog_info(m_zlogA, "%s AFN or Fn error", __FUNCTION__);
        // comm_data_set_step(pcomm_data, GW1376_2_DATA_TYPE_PARAM_INIT);
        return -1;
    }

    u16_t total = MAKEWORD(papply_region->unit_buf[0], papply_region->unit_buf[1]);
    u16_t startNum = MAKEWORD(papply_region->unit_buf[2], papply_region->unit_buf[3]);

    u08_t num = papply_region->unit_buf[4];
    zlog_info(m_zlogA, "total : [%d] startNum : [%d]  num : [%d]", total, startNum, num);

    if (num == 0)
    {
        zlog_info(m_zlogA, "无节点应答");
        return 0;
    }

    char *unitBufTemp = NULL;
    unitBufTemp = papply_region->unit_buf + 5;

    char pubBuffer[6] = {0};

    for (auto it = m_meterInfoMap->begin(); it != m_meterInfoMap->end(); ++it)
    {
        it->second.topoStatus = false;
    }

    for (i = 0; i < num; i++)
    {

        METER_INFO meterInfo;
        CopyArray(unitBufTemp + i * 11, (char *)meterInfo.addr, 6);

        // meterInfo.phase = (unitBufTemp[i * 8 + 7]) & 0b00000111;      // 获取相位
        // meterInfo.protoType = (unitBufTemp[i * 8 + 7] >> 3) & 0b00000111; // 获取协议类型

        char msg[13] = {0};
        hexArrayToString((char *)meterInfo.addr, 6, msg);
        std::string addrkey = msg;
        auto mInfo = m_meterInfoMap->find(addrkey);
        // zlog_info(m_zlogA, "查找的LTU地址 : [ %s ] ", addrkey.c_str());
        if (mInfo != m_meterInfoMap->end())
        {
            zlog_info(m_zlogA, "LTU地址 : [ %s ]  可以升级", addrkey.c_str());
            // mInfo->second.phase = meterInfo.phase;
            // mInfo->second.protoType = meterInfo.protoType;
            // meterInfo.topoStatus = true;
            mInfo->second.topoStatus = true;
        }
        // hdzlog_info(meterInfo.addr, 6);
        //  zlog_info(m_logc, "meterInfo[i].phase : %d", meterInfo.phase);
        //   meterInfoVector.push_back(meterInfo);
        //    zlog_info(m_logc, "meterInfo[i].phase : %d", meterInfo.phase);
        //    printf("extractedBits : [%d] , protoType : [%d] \n", extractedBits, proType);
    }
    return 0;
}
