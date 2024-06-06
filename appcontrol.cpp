#include "appcontrol.h"
#include "cJSON.h"
#include "globalbase.h"
#include "dlt645_07.h"

#include "gw1376_2_AFN.h"
#include "gw1376_2_public.h"
#include <thread>
#include <iostream>

#define UPDATE_BLOCK 180
AppControl *AppControl::m_pApp = NULL;

AppControl::AppControl(std::string fileName, std::string fileVersion)
{
    m_fileName = fileName;
    m_fileVersion = fileVersion;
    m_serialPort = new SerialPort(m_fileName);
}

AppControl::~AppControl()
{
    delete m_serialPort;
}

int AppControl::init()
{
    m_logc = zlog_get_category("appcontrol");

    int nRet = readConfig();
    if (nRet < 0)
    {
        zlog_error(m_logc, "config file read failed");
        return -1;
    }

    if (!m_serialPort->openPort())
    {
        zlog_error(m_logc, "open uart error!");
        return -1;
    }

    if (!m_serialPort->configurePort(m_serialPort->baudRate, m_serialPort->dataBit, m_serialPort->stopBit, m_serialPort->parity))
    {
        zlog_error(m_logc, "config uart error!");
        return -1;
    }

    return 0;
}

off_t AppControl::getFileSize(const std::string &filePath)
{
    struct stat stat_buf;
    int rc = stat(filePath.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void AppControl::GetFileOperateInfo()
{
    m_fileOperateInfo.fileName = m_fileName;
    m_fileOperateInfo.fileSize = m_fileSize;
    m_fileOperateInfo.fileVersion = m_fileVersion;
    m_fileOperateInfo.fileBlock = UPDATE_BLOCK;

    m_fileOperateInfo.total = (m_fileOperateInfo.fileSize / m_fileOperateInfo.fileBlock);
    if (0 != (m_fileOperateInfo.fileSize % m_fileOperateInfo.fileBlock))
    {
        m_fileOperateInfo.total += 1;
    }
    m_fileOperateInfo.current = 0;
}

void AppControl::start()
{
    unsigned char buf[2048] = {0};
    if (access(m_fileName.c_str(), F_OK) != 0)
    {
        zlog_error(m_logc, "file is not exist! filename : %s", m_fileName.c_str());
        return;
    }

    m_fileSize = getFileSize(m_fileName);
    zlog_info(m_logc, "m_fileSize : %d", m_fileSize);
    if (m_fileSize < 0)
    {
        return;
    }
    setZlogP(m_logc);
    SetZlogPointer(m_logc);
    GetFileOperateInfo();
    setFileInfo(&m_fileOperateInfo);

    int len;

    memset(buf, 0, sizeof(buf));
    buf[0] = 0x01;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    len = GW1376AFN05Fn01down(buf, 6);
    m_serialPort->sendData(buf, len);
    zlog_print(m_logc, "send buf [ %s ] ", buf, len);
    usleep(m_interval * 1000);
    memset(buf, 0, sizeof(buf));
    len = m_serialPort->receiveData(buf, sizeof(buf));
    zlog_print(m_logc, "recv buf [ %s ] ", buf, len);

    if (protocol_gw1376_2_process_cco_buf((char *)buf, len) != 1)
    {
        zlog_info(m_logc, "设置主节点失败 ");
        return;
    }
    zlog_info(m_logc, "设置主节点成功 ");

    memset(buf, 0, sizeof(buf));
    len = GW1376AFN01Fn02down(buf, sizeof(buf));
    m_serialPort->sendData(buf, len);
    zlog_print(m_logc, "send buf [ %s ] ", buf, len);
    usleep(m_interval * 1000);
    memset(buf, 0, sizeof(buf));
    len = m_serialPort->receiveData(buf, sizeof(buf));
    zlog_print(m_logc, "recv buf [ %s ] ", buf, len);

    if (protocol_gw1376_2_process_cco_buf((char *)buf, len) != 1)
    {
        zlog_info(m_logc, "参数初始化失败 ");
        return;
    }
    zlog_info(m_logc, "参数初始化成功 ");

    for (int i = 0; i < m_meterInfo.size(); i++)
    {
        // 添加档案
        memset(buf, 0, sizeof(buf));
        buf[0] = 1;
        memcpydir(buf + 1, m_meterInfo[i].addr, 6, DIR_REVERSE);
        buf[7] = 0x02;
        len = GW1376AFN11Fn01down(buf, 8);
        m_serialPort->sendData(buf, len);
        zlog_print(m_logc, "send buf [ %s ] ", buf, len);
        usleep(m_interval * 1000);
        memset(buf, 0, sizeof(buf));
        len = m_serialPort->receiveData(buf, sizeof(buf));
        zlog_print(m_logc, "recv buf [ %s ] ", buf, len);
        if (protocol_gw1376_2_process_cco_buf((char *)buf, len) != 1)
        {
            zlog_print(m_logc, "addr [ %s ] 添加档案失败 ", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));
            continue;
        }
        zlog_print(m_logc, "addr [ %s ] 添加档案成功 ", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));
    }

    // 查询从节点状态

    memset(buf, 0, sizeof(buf));
    int index = 0;
    buf[index++] = 0x00;
    buf[index++] = 0x00;
    buf[index++] = 0xff;
    len = GW1376AFN10Fn02down(buf, index);
    m_serialPort->sendData(buf, len);
    zlog_print(m_logc, "send buf [ %s ] ", buf, len);
    usleep(m_interval * 1000 * 2);
    memset(buf, 0, sizeof(buf));
    len = m_serialPort->receiveData(buf, sizeof(buf));
    zlog_print(m_logc, "recv buf [ %s ] ", buf, len);
    protocol_gw1376_2_process_cco_buf((char *)buf, len);

    std::vector<METER_INFO> meterInfo = GetMeterInfo();

    for (int i = 0; i < meterInfo.size(); i++)
    {
        memcpy(m_meterInfo[i].addr, meterInfo[i].addr, sizeof(meterInfo[i].addr));
        m_meterInfo[i].phase = meterInfo[i].phase;
        m_meterInfo[i].protoType = meterInfo[i].protoType;

        char tmpAddr[13] = {0};
        for (int k = 0; k < 6; k++)
        {
            sprintf(tmpAddr + 2 * k, "%02x", m_meterInfo[i].addr[k]);
        }
        if (m_meterInfo[i].phase > 0)
        {
            zlog_debug(m_logc, "addr : [ %s ] , phase : [ %d ] , can be upgrade", tmpAddr, m_meterInfo[i].phase);
        }
        else
        {
            zlog_debug(m_logc, "addr : [ %s ] , phase : [ %d ] , can`t upgrade", tmpAddr, m_meterInfo[i].phase);
        }
    }

    for (int i = 0; i < m_meterInfo.size(); i++)
    {
        if (m_meterInfo[i].update == true)
        {
            continue;
        }
        if (m_meterInfo[i].phase <= 0)
        {
            zlog_debug(m_logc, "phase not suport ! can`t update");
            zlog_debug_print(m_logc, "addr : [%s]  update failed! \n\n", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));
            continue;
        }
        zlog_debug(m_logc, "phase : [ %d ] , can be upgraded", m_meterInfo[i].phase);
        zlog_debug_print(m_logc, "addr : [ %s] start update!", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));

        // 升级开始
        setCurrent(0);
        setUpdateStep(UPDATE_STEP_AUTO);
        dlt645_07_set_addr(m_meterInfo[i].addr);
        char reveAddr[6] = {0};
        memcpydir(reveAddr, m_meterInfo[i].addr, 6, DIR_REVERSE);
        set_addr_region_dest(reveAddr, get_pdata());

        while (true)
        {
            // usleep(20 * 1000);
            memset(buf, 0, sizeof(buf));
            buf[0] = 0x00;
            len = dlt645_07_get_frame(buf + 2, len);
            buf[1] = len;
            len += 2;
            // zlog_print(m_logc, "dlt645_07_get_frame buf [ %s ] ", buf, len);
            len = GW1376AFN02Fn01down(buf, len);
            // zlog_print(m_logc, "GW1376AFN02Fn01down buf [ %s ] ", buf, len);
            m_serialPort->sendData(buf, len);
            zlog_print(m_logc, "send buf [ %s ] ", buf, len);
            usleep(m_interval * 1000);
            memset(buf, 0, sizeof(buf));
            len = m_serialPort->receiveData(buf, sizeof(buf));
            zlog_print(m_logc, "recv buf [ %s ] ", buf, len);
            if (len > 0)
            {
                if (protocol_gw1376_2_process_cco_buf((char *)buf, len) < 0)
                {
                    continue;
                }
                PROTOCOL_GW1376_2_APPLY_REGION *papply_region =
                    protocol_gw1376_2_recv_apply_region_get(get_pdata());
                if (dlt645_07_process_frame((unsigned char *)papply_region->unit_buf + 2, papply_region->unit_len - 2) == false)
                {
                    zlog_print(m_logc, "addr : [%s] update failed!", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));
                    m_meterInfo[i].appVersion = "0";
                    m_meterInfo[i].update = false;
                    continue;
                }
            }

            if (getUpdateStep() == UPDATE_STEP_END)
            {

                m_meterInfo[i].appVersion = gerAppVersion();
                m_meterInfo[i].update = true;
                zlog_debug(m_logc, "update after version : [ %s ]", m_meterInfo[i].appVersion.c_str());
                zlog_debug_print(m_logc, "addr : [ %s]  update successed! \n\n", m_meterInfo[i].addr, sizeof(m_meterInfo[i].addr));

                break;
            }
        }
    }
}

int AppControl::readConfig()
{
    FILE *file;
    zlog_info(m_logc, "begin read configfile");
    char szPath[128] = {0};
    getcwd(szPath, sizeof(szPath));
    sprintf(szPath, CONFIGPATH);

    sprintf(szPath, "%sconfig.json", szPath);
    zlog_info(m_logc, "config is %s ", szPath);
    if (access(szPath, R_OK) == -1)
    {
        zlog_error(m_logc, "the config file is not exist ");
        return -1;
    }
    file = fopen(szPath, "r");
    if (file == NULL)
    {
        zlog_error(m_logc, "the config file open failed ");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    int nLen = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *szFile = new char[nLen];

    memset(szFile, 0, nLen);
    fread(szFile, 1, nLen, file);
    fclose(file);

    cJSON *root = cJSON_Parse(szFile);
    if (!root)
    {
        zlog_error(m_logc, "json config Invalid Json Error before: [%s]", cJSON_GetErrorPtr());
        delete[] szFile;
        szFile = NULL;
        return -1;
    }

    cJSON *element;
    cJSON *element_child;
    for (element = root->child; element != NULL; element = element->next)
    {
        if (element->type == cJSON_String)
        {
            if (strcmp(element->string, "appname") == 0)
            {
                m_appName = element->valuestring;
                zlog_info(m_logc, "m_appName is %s ", m_appName.c_str());
            }
        }
        else if (element->type == cJSON_Object)
        {

            if (strcmp(element->string, "uartConfigInfo") == 0)
            {
                for (element_child = element->child; element_child != NULL; element_child = element_child->next)
                {
                    if (strcmp(element_child->string, "mountname") == 0)
                    {
                        m_serialPort->portName = element_child->valuestring;
                        zlog_info(m_logc, "m_serialPort->portName is %s ", m_serialPort->portName.c_str());
                    }
                    else if (strcmp(element_child->string, "baudrate") == 0)
                    {
                        m_serialPort->baudRate = element_child->valueint;
                        zlog_info(m_logc, "m_serialPort->baudRate is %d ", m_serialPort->baudRate);
                    }
                    else if (strcmp(element_child->string, "databit") == 0)
                    {
                        m_serialPort->dataBit = element_child->valueint;
                        zlog_info(m_logc, "m_serialPort->dataBit is %d ", m_serialPort->dataBit);
                    }
                    else if (strcmp(element_child->string, "stopbit") == 0)
                    {
                        m_serialPort->stopBit = element_child->valueint;
                        zlog_info(m_logc, "m_serialPort->stopBit is %d ", m_serialPort->stopBit);
                    }
                    else if (strcmp(element_child->string, "parity") == 0)
                    {
                        strcpy(&m_serialPort->parity, element_child->valuestring);
                        zlog_info(m_logc, "m_serialPort->parity is %c ", m_serialPort->parity);
                    }
                    else if (strcmp(element_child->string, "interval") == 0)
                    {
                        m_interval = element_child->valueint;
                        zlog_info(m_logc, "m_interval is %d ", m_interval);
                    }
                }
            }
        }

        else if (element->type == cJSON_Array)
        {
            if (strcmp(element->string, "meterAddr") == 0)
            {
                for (int i = 0; i < cJSON_GetArraySize(element); i++)
                {
                    METER_INFO meterInfo;
                    cJSON *item = cJSON_GetArrayItem(element, i);

                    hexStringToAscii(item->valuestring, 12, meterInfo.addr);

                    meterInfo.appVersion = "0"; // 默认0
                    meterInfo.update = false;   // 默认没升
                    m_meterInfo.push_back(meterInfo);
                    zlog_print(m_logc, "meterInfo.addr = [%s]", meterInfo.addr, sizeof(meterInfo.addr));
                    zlog_info(m_logc, "meterInfo.appVersion = [%s], meterInfo.update = [%d]",
                              meterInfo.appVersion.c_str(), meterInfo.update);
                }
            }
        }
    }
    cJSON_Delete(root);
    return 0;
}

void AppControl::zlog_print(zlog_category_t *zlogU, char *msg, const unsigned char *buf, int bufSize)
{
    /*
    char *printBuf = (char *)malloc(bufSize * sizeof(char));
    if (printBuf == NULL)
    {
        zlog_error(zlogU, "malloc failed!");
        return;
    }*/
    char printBuf[2048] = {0};
    for (int i = 0; i < bufSize; i++)
    {
        sprintf(printBuf + (i * 3), "%02hhx ", buf[i]);
    }
    zlog_info(zlogU, msg, printBuf);
}

void AppControl::zlog_debug_print(zlog_category_t *zlogU, char *msg, const unsigned char *buf, int bufSize)
{
    /*
    char *printBuf = (char *)malloc(bufSize * sizeof(char));
    if (printBuf == NULL)
    {
        zlog_error(zlogU, "malloc failed!");
        return;
    }*/
    char printBuf[2048] = {0};
    for (int i = 0; i < bufSize; i++)
    {
        sprintf(printBuf + (i * 3), "%02hhx ", buf[i]);
    }
    zlog_debug(zlogU, msg, printBuf);
}
