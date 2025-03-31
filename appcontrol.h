#ifndef __APPCONTROL_H__
#define __APPCONTROL_H__

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sys/stat.h>

#include "queuebuffer.h"
#include "define.h"
#include "timerlist.h"

#include "zlog.h"

#include "SerialPort.h"

#include "dlt645_07.h"

#include "cJSON.h"

#define MAXTIMEOUT 60
#define CONFIGPATH "/home/sysadm/meterUpgrade/conf/"
#define APP_CFG_JSON "/data/app/breaker/"
// #define CONFIGPATH "/home/pengwx/Code/meterUpgrade/conf/"
class AppControl
{
public:
    AppControl(std::string fileName, std::string fileVersion);
    ~AppControl();
    int init();
    void start();
    //    static void timerCallback(void *message);
    static AppControl *m_pApp;

    static void callbackMqttMessage(int type, char *topic, char *message);
    void parsingMqttMessage(int type, char *topic, char *message);
    off_t getFileSize(const std::string &filePath);

    void zlog_print(zlog_category_t *zlogU, char *msg, const unsigned char *buf, int bufSize);
    void zlog_debug_print(zlog_category_t *zlogU, char *msg, const unsigned char *buf, int bufSize);

private:
    int readConfig();
    int readBreakerConfig();
    int app_cfg_parse_devs(cJSON *devs);
    void dealMessage(DATAMESSAGE *message);

    void dealMessageThreadFunc();
    void GetFileOperateInfo();
    void GetSlaveNodeInfo(int addSubnodeNum, int addSubnodeNumYu); // 获取从节点信息
    void UpdateSlaveInfo(int addSubnodeNum, int addSubnodeNumYu);  // 更新从节点信息

    void GetNetworkTopoStatus(int addSubnodeNum, int addSubnodeNumYu); // 获取网络拓扑信息
    void UpdateTopoStatus(int addSubnodeNum, int addSubnodeNumYu);     // 更新网络拓扑状态
private:
    std::string m_devSn;   // 设备sn
    std::string m_appName; // APP名称

    /****电表升级*****/
    zlog_category_t *m_logc;

    int m_interval;
    SerialPort *m_serialPort;
    std::map<std::string, METER_INFO> m_meterInfoMap;
    std::vector<METER_INFO> m_addMeterInfo;
    std::string m_fileName;
    std::string m_fileVersion;
    off_t m_fileSize;
    FileOperateInfo m_fileOperateInfo;
    
};

#endif