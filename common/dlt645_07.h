/**
 *   \file dlt645_07.h
 *   \brief 645的实现
 */
#ifndef _DLT645_07_H_
#define _DLT645_07_H_

#include "dlt645_07_api.h"
#include "zlog.h"
#define PROTOCOL_DLT645_MAX_BUF_LEN 1024
#define PROTOCOL_DLT645_MAX_DATA_LEN 200

// 升级阶段枚举
typedef enum _UPDATE_STEP
{
	UPDATE_STEP_AUTO = 0,
	UPDATE_STEP_FILE,
	UPDATE_STEP_CRC,
	UPDATE_STEP_STATE,
	UPDATE_STEP_CONTINUE = 5,
	UPDATE_STEP_VER,
	UPDATE_STEP_SIZE,
	UPDATE_STEP_END,
} UPDATE_STEP;

/* 接收数据 */
typedef struct _PROTOCOL_DLT645_07_RECV_FRAME_DATA
{
	u08_t cfn;
	u08_t data_len;
	u08_t DI[4];
	u08_t passwd[4];
	u08_t admin[4];
	u08_t seq;
	u32_t dwDI;
	u08_t data[PROTOCOL_DLT645_MAX_BUF_LEN];
} PROTOCOL_DLT645_07_RECV_FRAME_DATA;

/* 接收数据 */
typedef struct _PROTOCOL_DLT645_07_SEND_FRAME_DATA
{
	u08_t cfn;
	int update_sent; /* 是否已发送 */
	BOOL follow;	 /* 有后序数据 */
	u08_t error;	 /* 错误信息字erro */
	BOOL have_DI;	 /* 是否有DI */
	u08_t buf[PROTOCOL_DLT645_MAX_BUF_LEN];
	int len;
	u08_t data[PROTOCOL_DLT645_MAX_BUF_LEN];
	int unitlen;   /* 单个数据的长度 */
	int datascale; /* 数据变比 */
	int datanum;   /* 数据数量 */
	int datalen;   /* 数据的长度 */

} PROTOCOL_DLT645_07_SEND_FRAME_DATA;

/* 协议数据 */
typedef struct _PROTOCOL_DLT645_07_DATA
{
	u08_t addr[6];
	u08_t passwd[4];
	u08_t user[4];
	int thread_type;

	UPDATE_STEP updateStep;

	PROTOCOL_DLT645_07_RECV_FRAME_DATA recv_frame;
	PROTOCOL_DLT645_07_SEND_FRAME_DATA send_frame;

} PROTOCOL_DLT645_07_DATA;

typedef struct FILE_OPERATE
{
	std::string fileName;
	std::string fileVersion;
	off_t fileSize;
	int fileBlock;
	int total;
	int current;
	char ver[128];
	UPDATE_STEP updateStep; // 升级状态
	bool resStatus;			// 响应状态 正常响应 true 异常响应 false
	bool resStatusRecord;	// 用于日志打印
} FileOperateInfo;

typedef struct
{
	unsigned char addr[6];
	std::string name;
	int protoType;			   // 协议类型
	int phase;				   // 相位信息
	bool update;			   // 升级完成标志
	std::string appVersion;	   // 版本号
	FileOperateInfo fileOInfo; // 设备对应文件信息
	bool topoStatus;		   // 拓扑状态
	int nodeID;				   // 节点标识
	int proxyNodeID;		   // 代理节点标识
	int nodeInfo;			   // 节点信息

	bool slaveInfoInit; // 子节点信息初始化
} METER_INFO;

void setCurrent(int current);

void setZlogP(zlog_category_t *zlogp);

void setFileInfo(FileOperateInfo *info);

unsigned int getcrc(void);

char *gerAppVersion();

/**
 *  \brief 获取报文
 */
int dlt645_07_set_addr(unsigned char *pbuf);
u08_t *dlt645_07_get_addr(void);

/**
 *  \brief 获取报文
 */
int dlt645_07_set_passwd(unsigned char *pbuf);

/**
 *  \brief 获取报文
 */
int dlt645_07_set_user(unsigned char *pbuf);

/**
 *  \brief 获取报文
 */
int dlt645_07_get_frame(unsigned char *pbuf, int len);

/**
 *  \brief 处理报文
 */
int dlt645_07_process_frame(unsigned char *pbuf, int len);

/**
 *  \brief 获取打印
 */
char *get_print_queue(char *out);

UPDATE_STEP getUpdateStep();
void setUpdateStep(UPDATE_STEP update_step);

#endif /* _DLT645_07_H_ */
