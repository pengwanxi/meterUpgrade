
/**
 *   \file protocol_gw1376_2_AFN.h
 *   \brief 组织AFN数据
 */
#ifndef _PROTOCOL_GW1376_2_AFN_H_
#define _PROTOCOL_GW1376_2_AFN_H_

#include "gw1376_2_data.h"
#include "zlog.h"
#include "define.h"

#define GW1376_2_AF00_IS_CONFIRM 0x00       // 确认
#define GW1376_2_AF01_INIT 0X01             // 初始化
#define GW1376_2_AF02_TRANS_DATA 0X02       // 数据转发
#define GW1376_2_AF03_QUERY 0x03            // 查询数据
#define GW1376_2_AF05_CTL_COMMAND 0x05      // 控制命令
#define GW1376_2_AF06_ACT_REPORT 0x06       // 主动上报
#define GW1376_2_AF10_ROUTER_QUERY 0x10     // 路由查询
#define GW1376_2_AF11_ROUTER_SET 0x11       // 路由设置
#define GW1376_2_AF14_ROUTER_DATA_READ 0x14 // 路由数据抄读

/*数据单元标识定义*/

/*AFN00 确认 取消*/
typedef enum Protocol_GW103762_AFN00_Fn
{
    GW1376_2_AFN00_NULL = 0, /* 空 */
    GW1376_2_AFN00_F1,       /* 确认 */
    GW1376_2_AFN00_F2,       /* 否认 */
} Protocol_GW103762_AFN00_Fn;

/*AFN01 初始化*/
typedef enum Protocol_GW103762_AFN01_Fn
{
    GW1376_2_AFN01_NULL = 0, /* 空 */
    GW1376_2_AFN01_F1,       /* 硬件初始化 */
    GW1376_2_AFN01_F2,       /* 参数初始化 */
    GW1376_2_AFN01_F3,       /* 数据初始化 */
} Protocol_GW103762_AFN01_Fn;

/*AFN02 数据转发*/
typedef enum Protocol_GW103762_AFN02_Fn
{
    GW1376_2_AFN02_NULL = 0, /* 空 */
    GW1376_2_AFN02_F1,       /* 转发通信协议数据帧 */
} Protocol_GW103762_AFN02_Fn;

/*AFN03 控制命令*/
typedef enum Protocol_GW103762_AFN03_Fn
{
    GW1376_2_AFN03_NULL = 0,   /* 空 */
    GW1376_2_AFN03_F1,         /* 厂商代码和版本信息 */
    GW1376_2_AFN03_F2,         /* 噪声值 */
    GW1376_2_AFN03_F3,         /* 从节点侦听信息 */
    GW1376_2_AFN03_F4,         /* 主节点地址 */
    GW1376_2_AFN03_F5,         /* 主节点状态字和通信速率 */
    GW1376_2_AFN03_F6,         /* 主节点干扰状态 */
    GW1376_2_AFN03_F7,         /* 读取从节点监控最大超时时间 */
    GW1376_2_AFN03_F8,         /* 查询无线通信参数 */
    GW1376_2_AFN03_F9,         /* 通信延时相关广播通信时长 */
    GW1376_2_AFN03_F10,        /* 本地通信模块运行模式信息 */
    GW1376_2_AFN03_F11,        /* 本地通信模块 AFN 索引 */
    GW1376_2_AFN03_F12,        /* 查询本地主节点通信模块 ID 号信息 */
    GW1376_2_AFN03_F16 = 16,   /* 查询宽带载波通信参数 */
    GW1376_2_AFN03_F100 = 100, /* 查询场强门限 */
} Protocol_GW103762_AFN03_Fn;

/*AFN05 控制命令*/
typedef enum Protocol_GW103762_AFN05_Fn
{
    GW1376_2_AFN05_NULL = 0,   /* 空 */
    GW1376_2_AFN05_F1,         /* 设置主节点地址 */
    GW1376_2_AFN05_F2,         /* 允许/禁止从节点上报 */
    GW1376_2_AFN05_F3,         /* 启动广播 */
    GW1376_2_AFN05_F4,         /* 设置从节点监控最大超时时间 */
    GW1376_2_AFN05_F5,         /* 设置无线通信参数 */
    GW1376_2_AFN05_F6,         /* 允许/禁止台区识别 */
    GW1376_2_AFN05_F16 = 16,   /* 设置宽带载波通信参数 */
    GW1376_2_AFN05_F100 = 100, /* 设置场强门限 */
    GW1376_2_AFN05_F101 = 101, /* 设置中心节点时间 */
    GW1376_2_AFN05_F200 = 200, /* 允许/禁止拒绝节点上报 */
} Protocol_GW103762_AFN05_Fn;

/*AFN10 路由查询*/
typedef enum Protocol_GW103762_AFN10_Fn
{
    GW1376_2_AFN10_NULL = 0,   /* 空 */
    GW1376_2_AFN10_F1,         /* 从节点数量 */
    GW1376_2_AFN10_F2,         /* 从节点信息 */
    GW1376_2_AFN10_F3,         /* 指定从节点的上一级中继路由信息 */
    GW1376_2_AFN10_F4,         /* 路由运行状态 */
    GW1376_2_AFN10_F5,         /* 未抄读成功的从节点信息 */
    GW1376_2_AFN10_F6,         /* 主动注册的从节点信息 */
    GW1376_2_AFN10_F7,         /* 查询从节点 ID 号信息 */
    GW1376_2_AFN10_F9 = 9,     /* 查询 HPLC 网络规模 */
    GW1376_2_AFN10_F21 = 21,   /* 查询网络拓扑信息 */
    GW1376_2_AFN10_F31 = 31,   /* 查询相线信息 */
    GW1376_2_AFN10_F40 = 40,   /* 流水线查询 ID 信息 */
    GW1376_2_AFN10_F100 = 100, /* 查询无线网络规模 */
    GW1376_2_AFN10_F101 = 101, /* 查询微功率无线从节点信息 */
    GW1376_2_AFN10_F111 = 111, /* 查询网络信息 */
    GW1376_2_AFN10_F112 = 112, /* 查询宽带载波芯片信息 */
} Protocol_GW103762_AFN10_Fn;

/*AFN11 控制命令*/
typedef enum Protocol_GW103762_AFN11_Fn
{
    GW1376_2_AFN11_NULL = 0,   /* 空 */
    GW1376_2_AFN11_F1,         /* 添加从节点 */
    GW1376_2_AFN11_F2,         /* 删除从节点 */
    GW1376_2_AFN11_F3,         /* 设置从节点固定中继路径 */
    GW1376_2_AFN11_F4,         /* 设置路由工作模式 */
    GW1376_2_AFN11_F5,         /* 激活从节点主动注册 */
    GW1376_2_AFN11_F6,         /* 终止从节点主动注册 */
    GW1376_2_AFN11_F100 = 100, /* 设置网络规模 */
    GW1376_2_AFN11_F101 = 101, /* 启动网络维护进程 */
    GW1376_2_AFN11_F102 = 102, /* 启动组网 */
} Protocol_GW103762_AFN11_Fn;

/*AFN14 控制命令*/
typedef enum Protocol_GW103762_AFN14_Fn
{
    GW1376_2_AFN14_NULL = 0, /* 空 */
    GW1376_2_AFN14_F1,       /* 路由请求抄读内容 */
    GW1376_2_AFN14_F2,       /* 路由请求集中器时钟 */
    GW1376_2_AFN14_F3,       /* 请求依通信延时修正通信数据 */
    GW1376_2_AFN14_F4,       /* 路由请求交采信息 */
} Protocol_GW103762_AFN14_Fn;

typedef struct GW103762DataTypeFunc
{
    PROTOCOL_GW1376_2_DATA_TYPE dataType;
    int (*FuncHandler)(PROTOCOL_GW1376_2_DATA *, int (*MqttPublish)(unsigned char *buffer, int bufferLen));
    int (*MqttPublish)(unsigned char *buffer, int bufferLen);
} GW103762DataTypeFunc_t;

typedef struct GW103762DataActReportFunc
{
    int AFN;
    int Fn;
    int (*FuncHandler)(PROTOCOL_GW1376_2_DATA *, int (*MqttPublish)(unsigned char *buffer, int bufferLen));
    int (*MqttPublish)(unsigned char *buffer, int bufferLen);
} GW103762DataActReportFunc_t;

void SetZlogPointer(zlog_category_t *zlog);
std::vector<METER_INFO> GetMeterInfo();

int protocol_gw1376_TotalAFN_up(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief 计算fn
 *  \param DT1
 *  \param DT2
 *  \return unsigned char
 */
unsigned char protocol_gw1376_2_cal_fn(unsigned char DT1, unsigned char DT2);

/**
 *  \brief 分解fn
 *  \param DT1
 *  \param DT2
 *  \return 0 成功
 *  \return <0 失败
 */
int protocol_gw1376_2_dec_fn(unsigned char fn, unsigned char *DT1,
                             unsigned char *DT2);

/*****************************************************************************/
/*                                   AFN00 数据确认                          */
/*****************************************************************************/
int protocol_gw1376_AFN00_up(PROTOCOL_GW1376_2_DATA *pdata);

/*****************************************************************************/
/*                                   AFN01 数据初始化                        */
/*****************************************************************************/
/**
 *  \brief  AFN01 F1 硬件初始化（复位）
 *  \param param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN01_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief  AFN01 F1 从节点档案信息初始化（参数初始化）
 *  \param param
 *  \return <0 失败
 *  \return =0 成功
 */
int GW1376AFN01Fn02down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN01_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief  AFN01 F1 数据区初始化（节点侦听信息）
 *  \param param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN01_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AF01 上送
 *
 *  跳转AFN00
 *
 *  \param
 *  \return
 */
int protocol_gw1376_AFN01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));
/*****************************************************************************/
/*                                   AFN02 数据转发                          */
/*****************************************************************************/
/**
 *  \brief AFN02 F1 数据下发
 *
 *  数据转发
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int GW1376AFN02Fn01down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN02_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN02 F1 数据处理
 *
 *  数据转发
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN02_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/*****************************************************************************/
/*                                   AFN03 查询数据                          */
/*****************************************************************************/
/**
 *  \brief AFN02 F1 数据下发
 *
 *  读取从节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN03_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN02 F1 数据处理
 *
 *  读取从节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN03_Fn03_up(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN02 F1 数据下发
 *
 *  读取主节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int GW1376AFN03Fn04down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN03_Fn04_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN02 F1 数据处理
 *
 *  读取主节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN03_Fn04_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/**
 *  \brief AFN02 F1 数据下发
 *
 *  本地通信模块运行模式信息
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN03_Fn10_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN02 F1 数据处理
 *
 *  本地通信模块运行模式信息
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN03_Fn10_up(PROTOCOL_GW1376_2_DATA *pdata);

/*****************************************************************************/
/*                                   AFN05 控制命令                          */
/*****************************************************************************/
/**
 *  \brief AFN05 F01 数据下发
 *
 *  设置主节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int GW1376AFN05Fn01down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN05_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN05 F01 数据处理
 *
 *  设置主节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/**
 *  \brief AFN05 F2 数据下发
 *
 *  子节点主动上报
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN05 F2 数据处理
 *
 *  子节点主动上报
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN05 F3 广播处理
 *
 *  读取主节点地址
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn03_up(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN05 F2 广播处理
 *
 *  子节点主动上报
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN02 F4 数据下发
 *
 *  设置超时时间
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn04_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN05 F4 数据处理
 *
 *  设置超时时间
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN05_Fn04_up(PROTOCOL_GW1376_2_DATA *pdata);

/*****************************************************************************/
/*                                   AFN06  主动上报                          */
/*****************************************************************************/
/**
 *  \brief AFN06 F2 数据下发
 *
 *  上报抄读数据
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */

int protocol_gw1376_AFN06_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/*****************************************************************************/
/*                                   AFN10 路由查询                         */
/*****************************************************************************/
/**
 *  \brief AFN10 F1 数据下发
 *
 *  读取从节点数量
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */

int GW1376AFN10Fn01down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN10_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN10 F1 数据处理
 *
 *  读取从节点数量
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN10_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/**
 *  \brief AFN10 F2 数据下发
 *
 *  查询从节点信息
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int GW1376AFN10Fn02down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN10_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN10 F2 数据处理
 *
 *  读取从节点信息处理
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN10_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/*****************************************************************************/
/*                                   AFN11 路由设置                         */
/*****************************************************************************/
/**
 *  \brief AFN11 F1 数据下发
 *
 *  添加从节点
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */

int GW1376AFN11Fn01down(unsigned char *buffer, int bufferLen);

int protocol_gw1376_AFN11_Fn01_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN11 F1 数据处理
 *
 *  添加从节点
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN11_Fn01_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/**
 *  \brief AFN11 F2 数据下发
 *
 *  删除从节点
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */

int GW1376AFN11Fn02down(unsigned char *buffer, int bufferLen);

int protocol_gw1376_AFN11_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

/**
 *  \brief AFN11 F1 数据处理
 *
 *  删除从节点
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN11_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));

/**
 *  \brief AFN11 F1 数据下发
 *
 *  添加从节点中继
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN11_Fn03_down(PROTOCOL_GW1376_2_DATA *pdata);

/**
 *  \brief AFN11 F1 数据处理
 *
 *  添加从节点中继
 *
 *  \param
 *  \return <0 失败
 *  \return =0 成功
 */
int protocol_gw1376_AFN11_Fn03_up(PROTOCOL_GW1376_2_DATA *pdata);

/*****************************************************************************/
/*                                   AFN14 路由数据抄读类                     */
/*****************************************************************************/

int GW1376AFN14Fn02down(unsigned char *buffer, int bufferLen);
int protocol_gw1376_AFN14_Fn02_up(PROTOCOL_GW1376_2_DATA *pdata, int (*MqttPublish)(unsigned char *buffer, int bufferLen));
int protocol_gw1376_AFN14_Fn02_down(PROTOCOL_GW1376_2_DATA *pdata, unsigned char *buffer, int bufferLen);

#endif /* _PROTOCOL_GW1376_2_AFN_H_ */
