#include <stdio.h>
#include <thread>
#include "appcontrol.h"
#include "zlog.h"
#include <iostream>

static int getProcessIdByName(const std::string &processName)
{
    std::string command = "pidof " + processName;
    std::string result;
    char buffer[128];
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error: Unable to execute command." << std::endl;
        return -1;
    }
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != nullptr)
        {
            result += buffer;
        }
    }
    pclose(pipe);
    if (result == "")
    {
        return -1;
    }
    return std::stoi(result);
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        // printf("./meterUpgrade 文件名 版本号 \n");
        return 0;
    }

    zlog_category_t *c;
    std::string path = CONFIGPATH;
    path = path + "zlog.conf";
    int zinit = zlog_init(path.c_str());
    // std::cout << "zlog path" << path << std::endl;
    // printf("zlog init %d\n", zinit);
    c = zlog_get_category("main");
    /*
    std::string processName = "meterUpgrade";
    int upProcessId = getProcessIdByName(processName);
    zlog_info(c, "meterUpgrade = %d", upProcessId);

    if (upProcessId > 0)
    {
        std::string cmd = "kill -9 " + std::to_string(upProcessId);
        zlog_info(c, "cmd : %s", cmd.c_str());
        system(cmd.c_str());
        sleep(1);
    }*/

    std::string firstArgument = argv[1];
    zlog_info(c, "update file name : %s", firstArgument.c_str());

    std::string secondArgument = argv[2];
    zlog_info(c, "update file version : %s", secondArgument.c_str());

    AppControl *app = new AppControl(firstArgument, secondArgument);
    int nInit = app->init();
    if (nInit < 0)
    {
        zlog_error(c, "init error");
        return -1;
    }
    app->start();
}
