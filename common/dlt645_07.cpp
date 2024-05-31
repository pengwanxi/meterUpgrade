#include "dlt645_07.h"
#include "global.h"
#include <stdlib.h>
#include "zlog.h"
#include "file_operation.h"
#include "stringAPI.h"
static u16_t cal_crc(u08_t *pbuf, int len)
{
    u16_t genpoly = 0xA001;
    u16_t crc = 0xFFFF;
    u16_t index;
    while (len--)
    {
        crc = crc ^ (u16_t)*pbuf++;
        for (index = 0; index < 8; index++)
        {
            if ((crc & 0x0001) == 1)
                crc = (crc >> 1) ^ genpoly;
            else
                crc = crc >> 1;
        }
    }

    return crc;
}

static char EVENT_CODE[][32] = {
    "NULL",
    "过载长延时保护",
    "短路短延时保护",
    "短路瞬时保护",
    "接通电流脱扣",
    "接地故障保护",
    "过压保护",
    "欠压保护",
    "缺相保护",
    "断零保护",
    "电流不平衡保护",
    "电压不平衡保护",
    "过频保护",
    "欠频保护",
    "相序保护",
    "逆功率保护",
    "剩余电流突变保护",
    "剩余电流缓变保护",
    "控制器温度保护",
    "控制器湿度保护",
    "接线端子温度保护",
    "防孤岛保护",
    "重合闸",
    "远程跳闸",
    "远程合闸",
    "手动分闸",
    "手动合闸",
    "重合闸闭锁",
    "长延时预告警",
    "断路器触头寿命超限",
    "断路器机械寿命超限",
    "断路器整体寿命超限",
    "断路器运行时间超限",
};

#define FE_NUM 4

// const static u08_t DEFAULT_PASSWD[3] = { 0x00, 0x00, 0x00 };
static u08_t DEFAULT_USER[4] = {0x00, 0x00, 0x00, 0x00};

/* 控制码 */
#define DLT645_07_CFN_READ_CAL_DATA 0x01       /* 读校准数据 */
#define DLT645_07_CFN_WRITE_CAL_DATA 0x04      /* 写校准数据 */
#define DLT645_07_CFN_TIME_SYNC 0x08           /* 对时 */
#define DLT645_07_CFN_READ_DATA 0x11           /* 读数据 */
#define DLT645_07_CFN_READ_FOLLOW_DATA 0x12    /* 读后续数据 */
#define DLT645_07_CFN_READ_ADDR 0x13           /* 读通信地址 */
#define DLT645_07_CFN_WRITE_DATA 0x14          /* 写数据 */
#define DLT645_07_CFN_WRITE_ADDR 0x15          /* 写通信地址 */
#define DLT645_07_CFN_FROZEN 0x16              /* 冻结命令 */
#define DLT645_07_CFN_BAUD 0x17                /* 更改通信速率 */
#define DLT645_07_CFN_PASSWD 0x18              /* 修改密码 */
#define DLT645_07_CFN_CLEAR_DEMAND 0x19        /* 最大需量清零 */
#define DLT645_07_CFN_CLEAR_METER 0x1A         /* 电表清零 */
#define DLT645_07_CFN_CELAR_EVENT 0x1B         /* 事件清零 */
#define DLT645_07_CFN_CTRL_REQ 0x1C            /* 控制请求 主要用于升级 */
#define DLT645_07_CFN_READ_TOPOLOGIC_DATA 0x1E /* 读拓扑数据 */

#define DL645_07_SLAVE_SENDER_BIT 0x80         /* 从站发送 */
#define DL645_07_SLAVE_ERROR_RESPONSE_BIT 0x40 /* 异常应答 */
#define DL645_07_SLAVE_FOLLOW_FRAME_BIT 0x20   /* 有后序帧 */

/* 错误信息字ERRO */
#define DL645_07_ERROR_INFO_OTHER 0x00     /* 其它错误 */
#define DL645_07_ERROR_INFO_NONE_DATA 0x01 /* 无请求数据 */
#define DL645_07_ERROR_INFO_PASSWD 0x02    /* 密码错 / 未授权 */
#define DL645_07_ERROR_INFO_BAUD 0x04      /* 通信速率不能更改 */
#define DL645_07_ERROR_INFO_YEAR_ZONE 0x08 /* 年时区数超 */
#define DL645_07_ERROR_INFO_DAY_ZONE 0x10  /* 日时段数超 */
#define DL645_07_ERROR_INFO_DAY 0x20       /* 费率数超 */

/* 地址类型 */
typedef enum _ADDR_TYPE_DATA
{
    ERROR_ADDR = 0, /* 地址错误 */
    NORMAL_ADDR,    /* 正常地址 */
    BROADCAST_ADDR, /* 广播地址 */
    WILDCARD_ADDR,  /* 通配地址 */
} ADDR_TYPE_DATA;

/* 通配地址 */
const static u08_t wildcard_addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
/* 广播地址 */
const static u08_t boardcast_addr[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99};

static PROTOCOL_DLT645_07_DATA gs_pdata;

static zlog_category_t *m_logc = NULL;

FileOperateInfo fileOperateInfo;

void setCurrent(int current)
{
    fileOperateInfo.current = current;
}

void setFileInfo(FileOperateInfo *info)
{
    fileOperateInfo.fileName = info->fileName;
    fileOperateInfo.fileVersion = info->fileVersion;
    fileOperateInfo.fileBlock = info->fileBlock;
    fileOperateInfo.fileSize = info->fileSize;
    fileOperateInfo.current = info->current;
    fileOperateInfo.total = info->total;
}

void setZlogP(zlog_category_t *zlogp)
{
    m_logc = zlogp;
}

static PROTOCOL_DLT645_07_DATA *get_pdata(void)
{
    return &gs_pdata;
}

/**
 *  \brief 组织报文
 *  \param PROTOCOL_DLT645_07_DATA
 *  \param DataBuf
 *  \param DataLen
 *  \return bool
 */
static bool dlt645_07_pack_send_frame(PROTOCOL_DLT645_07_DATA *pdata,
                                      u08_t *data, int len)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;
    PROTOCOL_DLT645_07_RECV_FRAME_DATA *precv = &pdata->recv_frame;
    int index = 0;
    if ((len + 10 + FE_NUM) > PROTOCOL_DLT645_MAX_BUF_LEN)
    {
        return false;
    }
    memset(psend->buf, 0xFE, FE_NUM);
    index = FE_NUM;

    /* 头 */
    psend->buf[index++] = 0x68;
    if (psend->cfn == 0x13)
    {
        memset(&psend->buf[index], 0xAA, 6);
    }
    else if (psend->cfn == 0x08)
    {
        memset(&psend->buf[index], 0x99, 6);
    }
    else
    {
        memcpydir(&psend->buf[index], pdata->addr, 6, DIR_REVERSE);
    }
    index += 6;
    psend->buf[index++] = 0x68;

    /* 控制码 */
    psend->buf[index++] = psend->cfn;

    if (psend->have_DI)
    {
        /* 长度 */
        psend->buf[index++] = len + 4;

        add33h(precv->DI, &psend->buf[index], 4, DLT645_BCD_POSITIVE);
        index += 4;
    }
    else
    {
        /* 长度 */
        psend->buf[index++] = len;
    }
    /* 数据 */
    add33h(data, &psend->buf[index], len, DLT645_BCD_POSITIVE);
    index += len;

    /* cs */
    psend->buf[index] = cal_cs(&psend->buf[FE_NUM], index - FE_NUM);
    index++;

    /* end */
    psend->buf[index++] = 0x16;

    psend->len = index;
    return true;
}

/**
 *  \brief 解析写通用数据
 */
static bool dlt645_07_process_write_common(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_RECV_FRAME_DATA *precv = &pdata->recv_frame;

    if (((precv->cfn & 0x40) == 0))
    {
        add2print_queue("写成功");
    }
    else
    {
        add2print_queue("写失败");
    }

    return true;
}

/**
 *  \brief 问一般数据
 */
static bool dlt645_07_read_common(PROTOCOL_DLT645_07_DATA *pdata, DWORD dwDI)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;
    u08_t DI[4];
    DI[0] = LOBYTE(LOWORD(dwDI));
    DI[1] = HIBYTE(LOWORD(dwDI));
    DI[2] = LOBYTE(HIWORD(dwDI));
    DI[3] = HIBYTE(HIWORD(dwDI));

    psend->cfn = 0x11;
    psend->datalen = 4;

    psend->datalen = 4;
    psend->data[0] = DI[0];
    psend->data[1] = DI[1];
    psend->data[2] = DI[2];
    psend->data[3] = DI[3];

    return true;
}

void setUpdateStep(UPDATE_STEP update_step)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();
    pdata->updateStep = update_step;
}

UPDATE_STEP getUpdateStep()
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();
    return pdata->updateStep;
}

int currentBuf(u08_t *buf, int &len)
{
    unsigned int pos = fileOperateInfo.current * fileOperateInfo.fileBlock;
    zlog_info(m_logc, "len=%d m_sendByte=%d pos=%d current=%d ", len,
              fileOperateInfo.fileBlock, pos, fileOperateInfo.current);
    int l = (int)file_operation_read((const char *)fileOperateInfo.fileName.c_str(), buf, len, &pos);

    /*
    zlog_print(m_logc, "read buf : [%s] ", (char *)buf, l);
    zlog_info(m_logc, "read num : [%d] ", l);
    */
    return l;
}

unsigned int getcrc(void)
{
    int len = 0;
    u32_t crc = 0xffffffff;
    char *pdata = file_operation_read_malloc((char *)fileOperateInfo.fileName.c_str(), &len);

    if (NULL == pdata || len <= 0)
    {
        return -1;
    }

    int pos = 0;
    while (pos < len)
    {
        int l = 500;
        if ((pos + 500) > len)
        {
            l = len - pos;
        }

        crc = Get_CRC32((u08_t *)pdata + pos, l, crc);
        pos += 500;
    }

    file_operation_read_free(&pdata);
    return crc;
}

/**
 *  \brief 升级
 */
static bool dlt645_07_update(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;

    char tmp[256];

    if (!BETWEEN(getUpdateStep(), 0, 5))
    {
        if (getUpdateStep() == UPDATE_STEP_VER)
        {
            zlog_info(m_logc, "read meter app version");
            DWORD dwDI = 0x04000102;
            return dlt645_07_read_common(pdata, dwDI);
        }
        return false;
    }
    DWORD dwDI = 0x06010501 + getUpdateStep();

    u08_t DI[4];
    int len = 0;
    DI[0] = LOBYTE(LOWORD(dwDI));
    DI[1] = HIBYTE(LOWORD(dwDI));
    DI[2] = LOBYTE(HIWORD(dwDI));
    DI[3] = HIBYTE(HIWORD(dwDI));

    psend->cfn = 0x1D;

    psend->data[len++] = DI[0];
    psend->data[len++] = DI[1];
    psend->data[len++] = DI[2];
    psend->data[len++] = DI[3];

    psend->data[len++] = pdata->passwd[0];
    psend->data[len++] = pdata->passwd[1];
    psend->data[len++] = pdata->passwd[2];
    psend->data[len++] = pdata->passwd[3];
    psend->data[len++] = DEFAULT_USER[0];
    psend->data[len++] = DEFAULT_USER[1];
    psend->data[len++] = DEFAULT_USER[2];
    psend->data[len++] = DEFAULT_USER[3];

    switch (getUpdateStep())
    {
    case UPDATE_STEP_AUTO:
    case UPDATE_STEP_CONTINUE:
    {
        if (getUpdateStep() == UPDATE_STEP_AUTO)
        {
            zlog_info(m_logc, "升级准备");
        }
        else
        {
            zlog_info(m_logc, "续传准备");
        }

        memcpydir(&psend->data[len], (void *)fileOperateInfo.fileName.c_str(), 20, DIR_REVERSE);
        len += 20;

        DWORD size = fileOperateInfo.fileSize;
        psend->data[len++] = LOBYTE(LOWORD(size));
        psend->data[len++] = HIBYTE(LOWORD(size));
        psend->data[len++] = LOBYTE(HIWORD(size));
        psend->data[len++] = HIBYTE(HIWORD(size));

        WORD block = fileOperateInfo.fileBlock;
        psend->data[len++] = LOBYTE(block);
        psend->data[len++] = HIBYTE(block);

        char ver[10];
        snprintf(ver, 9, "VER %s", fileOperateInfo.fileVersion.c_str());
        memcpydir(&psend->data[len], ver, 8, DIR_REVERSE);
        len += 8;

        zlog_info(m_logc, "name=%s size=%lu block=%d ver=%s\n", fileOperateInfo.fileName.c_str(), size, block, ver);
    }
    break;
    case UPDATE_STEP_FILE:
    {
        u16_t current = fileOperateInfo.current;
        // dword2bcd(current, &psend->data[len], 2, DLT645_BCD_REVERSE);
        psend->data[len++] = LOBYTE(current);
        psend->data[len++] = HIBYTE(current);

        int max = fileOperateInfo.fileBlock;
        int size = currentBuf(&psend->data[len], max);
        // psend->data[len++] = size;

        // zlog_print(m_logc, "read buf : [%s] ", (char *)buf, l);
        len += size;

        zlog_info(m_logc, "共%d帧 , 开始传输第%d帧", fileOperateInfo.total, fileOperateInfo.current + 1);
        // zlog_info(m_logc, "共%d帧 , 开始传输第%d帧", fileOperateInfo.total, fileOperateInfo.current);
    }
    break;

    case UPDATE_STEP_CRC:
    {
        u32_t crc = getcrc();
        psend->data[len++] = LOBYTE(LOWORD(crc));
        psend->data[len++] = HIBYTE(LOWORD(crc));
        psend->data[len++] = LOBYTE(HIWORD(crc));
        psend->data[len++] = HIBYTE(HIWORD(crc));
        zlog_info(m_logc, "开始校验");
    }
    break;

    case UPDATE_STEP_STATE:
    {
        zlog_info(m_logc, "读状态");
    }
    break;

    default:
        break;
    }
    psend->datalen = len;

    return true;
}

char *gerAppVersion()
{
    return fileOperateInfo.ver;
}

/**
 *  \brief 解析通用数据
 */
static bool dlt645_07_process_update(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_RECV_FRAME_DATA *precv = &pdata->recv_frame;

    zlog_info(m_logc, "precv->data[4]=%d", precv->data[4]);
    if (0 == precv->data[4])
    {
        zlog_info(m_logc, "正常响应");
    }
    else
    {
        zlog_info(m_logc, "异常响应");
        return false;
    }

    switch (getUpdateStep())
    {
    case UPDATE_STEP_AUTO:
    {
        setUpdateStep(UPDATE_STEP_FILE);
    }
    break;
    case UPDATE_STEP_FILE:
    {
        u16_t current = fileOperateInfo.current;
        u16_t block = MAKEWORD(precv->data[5], precv->data[6]);
        zlog_info(m_logc, "current : [%d] , block : [%d] ", current, block);
        if (current == block)
        {
            zlog_info(m_logc, "块序号成功\n\n");
        }
        else
        {
            return false;
        }

        fileOperateInfo.current++;

        if (fileOperateInfo.current >= fileOperateInfo.total)
        {
            setUpdateStep(UPDATE_STEP_STATE);
        }
    }
    break;

    case UPDATE_STEP_CRC:
    {
        zlog_info(m_logc, "CRC校验成功");
        setUpdateStep(UPDATE_STEP_VER);
    }
    break;

    case UPDATE_STEP_STATE:
    {
        if (precv->data_len != 185)
        {
            zlog_info(m_logc, "长度不对");
            return false;
        }
        int i;
        for (i = 0; i < 180; i++)
        {
            u08_t state = precv->data[i + 5];
            if (state == 0xFF)
            {
                continue;
            }
            else
            {
                int j;
                for (j = 0; j < 8; j++)
                {
                    if (!BIT_GET(state, j))
                    {
                        int current = i * 8 + j;
                        zlog_info(m_logc, "第%d帧状态", current + 1);
                        if (getUpdateStep() == UPDATE_STEP_CONTINUE)
                        {
                            fileOperateInfo.current = current;
                            setUpdateStep(UPDATE_STEP_FILE);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
            }
        }
        setUpdateStep(UPDATE_STEP_CRC);
    }
    break;
    case UPDATE_STEP_CONTINUE:
    {
        setUpdateStep(UPDATE_STEP_STATE);
    }
    break;
    case UPDATE_STEP_VER:
    {
        memcpydir(fileOperateInfo.ver, &precv->data[4], 10, DIR_REVERSE);
        fileOperateInfo.ver[10] = '0';
        setUpdateStep(UPDATE_STEP_END);
        zlog_info(m_logc, "update after version : [ %s ]", fileOperateInfo.ver);
        return true;
    }
    break;
    default:
        return false;
        break;
    }

    return true;
}

/**
 *  \brief 处理写保护参数数据
 *  \param pubf
 *  \return len
 */
static int
dlt645_07_process_write_protect_param_data(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_RECV_FRAME_DATA *precv = &pdata->recv_frame;

    if (precv->data_len == 0x00 && ((precv->cfn & 0x40) == 0))
    {
        add2print_queue("写成功 ");
    }
    else
    {
        add2print_queue("写失败 ");
    }
    return 0;
}

/**
 *  \brief 询问系统参数
 */
static bool
dlt645_07_read_protect_sys_param_data(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;

    psend->cfn = 0x11;
    psend->datalen = 4;

    psend->datalen = 4;
    psend->data[0] = 0x01;
    psend->data[1] = 0x00;
    psend->data[2] = 0x50;
    psend->data[3] = 0x04;

    return true;
}

/**
 *  \brief 询问系统地址
 */
static bool dlt645_07_read_param_addr_data(PROTOCOL_DLT645_07_DATA *pdata)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;

    psend->cfn = 0x11;
    psend->datalen = 4;

    psend->datalen = 4;
    psend->data[0] = 0x01;
    psend->data[1] = 0x04;
    psend->data[2] = 0x00;
    psend->data[3] = 0x04;

    return true;
}

int dlt645_07_get_frame(unsigned char *pbuf, int len)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;

    if (dlt645_07_update(pdata))
    {
        dlt645_07_pack_send_frame(pdata, psend->data, psend->datalen);
        if (psend->len > 0)
        {
            memcpy(pbuf, psend->buf, psend->len);
            psend->update_sent++;
            return psend->len;
        }
    }

    return 0;
}

/**
 *  \brief 检测地址合理性
 *  \param 地址
 *  \return >0 合理
 *  \return <=0 不合理
 */
static int dlt645_07_check_addr(PROTOCOL_DLT645_07_DATA *pdata, u08_t *addr)
{
    int i;

    /* 本机地址 */
    if (memcmpdir(pdata->addr, addr, 6, DIR_REVERSE) == 0)
        return NORMAL_ADDR;
    else if (memcmpdir((void *)boardcast_addr, addr, 6, DIR_REVERSE) ==
             0) /* 广播地址 */
        return BROADCAST_ADDR;
    else if (memcmpdir((void *)wildcard_addr, addr, 6, DIR_REVERSE) ==
             0) /* 通配地址 */
        return WILDCARD_ADDR;

    /* 通配与正常地址混合 */
    for (i = 0; i < 6; i++)
    {
        if (pdata->addr[5 - i] == addr[i] || 0xAA == addr[i])
            continue;
        else
            return ERROR_ADDR;
    }

    return WILDCARD_ADDR;
}

// /**
//  *  \brief 检查控制码
//  *  \param 地址类型
//  *  \param 控制码
//  *  \return bool
//  */
// static bool dlt645_07_check_cfn(ADDR_TYPE_DATA type, u08_t cfn)
// {
//     switch (type) {
//     case WILDCARD_ADDR: {
//         /* 只支持读命令 */
//         if (cfn == DLT645_07_CFN_READ_DATA ||
//             cfn == DLT645_07_CFN_READ_FOLLOW_DATA ||
//             cfn == DLT645_07_CFN_READ_ADDR) {
//             return true;
//         }
//         break;
//     }
//     case BROADCAST_ADDR: {
//         /* 只支持广播对时和冻结 */
//         if (cfn == DLT645_07_CFN_TIME_SYNC || cfn == DLT645_07_CFN_FROZEN) {
//             return true;
//         }
//         break;
//     }
//     case NORMAL_ADDR: {
//         return true;

//     } break;
//     default:
//         break;
//     }

//     return false;
// }

// /**
//  *  \brief 检查写数据的密码
//  *  \param 报文密码
//  *  \param 本地密码
//  *  \return bool
//  */
// static bool dlt645_07_check_passwd(u08_t *passwd1, u08_t *passwd2)
// {
//     if (02 == passwd1[0] || 04 == passwd1[0]) {
//         if (0 == memcmp(&passwd1[1], passwd2, 3))
//             return true;
//     }

//     return false;
// }

/**
 *  \brief 检测报文合理性
 *  \param 报文缓冲区
 *  \param 长度
 *  \return true 合理
 *  \return false 不合理
 */
static bool dlt645_07_check_buf(PROTOCOL_DLT645_07_DATA *pdata, u08_t *pbuf,
                                int len)
{
    PROTOCOL_DLT645_07_SEND_FRAME_DATA *psend = &pdata->send_frame;
    /* 长度 */
    if (len < 10)
    {
        return false;
    }

    /* 头 */
    if (0x68 == pbuf[0] && 0x68 != pbuf[7])
    {
        add2print_queue("error 0x68");
        return false;
    }

    ADDR_TYPE_DATA addr_type;
    /* 地址 */
    addr_type = (ADDR_TYPE_DATA)dlt645_07_check_addr(pdata, &pbuf[1]);
    if (psend->cfn != (u08_t)DLT645_07_CFN_READ_ADDR)
    {
        if (addr_type == ERROR_ADDR)
        {
            add2print_queue("error addr");
            return false;
        }
    }

    // /* 长度 */
    // u08_t l = pbuf[9];
    // if (l + 12 != len) {
    // 	add2print_queue("error len");
    //     return false;
    // }

    /* CS */
    u08_t cs = cal_cs(pbuf, len - 2);
    if (cs != pbuf[len - 2])
    {
        add2print_queue("error cs");
        return false;
    }

    /* 结束 */
    if (0x16 != pbuf[len - 1])
    {
        add2print_queue("error 0x16");
        return false;
    }

    return true;
}

/**
 *  \brief 找到fe后有效数据的位置
 *  \param 报文缓冲区
 *  \param 长度
 *  \return >=0 0x68的位置
 *  \return <0 错误
 */
static int vaild_buf_pos(u08_t *pbuf, int len)
{
    int pos = 0;
    while (len - pos > 0)
    {
        if (pbuf[pos] == 0x68)
            break;

        pos++;
    }

    if (pos < len)
        return pos;

    return -1;
}

/**
 *  \brief 处理数据
 *  \param pubf
 *  \return len
 */
static int dlt645_07_process_data(unsigned char *pbuf, int len)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();
    PROTOCOL_DLT645_07_RECV_FRAME_DATA *precv = &pdata->recv_frame;
    precv->cfn = pbuf[DLT645_CFN];
    precv->data_len = pbuf[DLT645_LEN];

    if ((precv->cfn & 0x80) == 0)
    {
        zlog_error(m_logc, "收报方向错误");
        return -1;
    }

    if ((precv->cfn & 0x40) != 0)
    {
        add2print_queue("error cfn\n");
        return -1;
    }

    (void)len;
    if (precv->data_len >= 4)
    {
        memset(precv->data, 0, PROTOCOL_DLT645_MAX_BUF_LEN);
        sub33h(&pbuf[DLT645_DI0], precv->data, precv->data_len,
               DLT645_BCD_POSITIVE);

        char tmp[1024] = "";
        for (int i = 0; i < precv->data_len; i++)
        {
            sprintf(&tmp[strlen(tmp)], "%.2x ", precv->data[i]);
        }
        zlog_info(m_logc, " recv_len= %d  data = %s", precv->data_len, tmp);
        sub33h(&pbuf[DLT645_DI0], precv->DI, 4, DLT645_BCD_POSITIVE);

        precv->dwDI = pack_dword_di(precv->DI);

        zlog_info(m_logc, "DI=%.2x %.2x %.2x %.2x", precv->DI[0], precv->DI[1],
                  precv->DI[2], precv->DI[3]);
    }
    bool b = dlt645_07_process_update(pdata);
    if (b)
        pdata->send_frame.update_sent = 0;

    return b;
}

int dlt645_07_process_frame(unsigned char *pbuf, int len)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();

    /* 找到0x68 */
    int pos = vaild_buf_pos(pbuf, len);
    if (pos < 0)
    {
        zlog_error(m_logc, "pos error ; recv data is null");
        return -1;
    }
    /* 检测报文合理性 */
    if (!dlt645_07_check_buf(pdata, &pbuf[pos], len - pos))
    {
        zlog_error(m_logc, "check error");
        return -1;
    }

    return dlt645_07_process_data(&pbuf[pos], len);
}

int dlt645_07_set_addr(unsigned char *pbuf)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();

    int i;
    for (i = 0; i < 6; i++)
    {
        pdata->addr[i] = pbuf[i];
    }

    return 0;
}

u08_t *dlt645_07_get_addr(void)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();

    return pdata->addr;
}

int dlt645_07_set_passwd(unsigned char *pbuf)
{
    PROTOCOL_DLT645_07_DATA *pdata = get_pdata();

    if (0 == memcmp(pbuf, pdata->passwd, 4))
    {
        return 0;
    }

    int i;
    for (i = 0; i < 4; i++)
    {
        pdata->passwd[i] = pbuf[i];
    }

    return 0;
}

int dlt645_07_set_user(unsigned char *pbuf)
{
    if (0 == memcmp(pbuf, DEFAULT_USER, 4))
    {
        return 0;
    }

    int i;
    for (i = 0; i < 4; i++)
    {
        DEFAULT_USER[i] = pbuf[i];
    }

    return 0;
}
