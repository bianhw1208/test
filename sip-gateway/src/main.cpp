#include "sipserver/gb28181_svr_manager.h"
#include "config_manager.h"
#include "threadpool.h"
#include "LogWrapper.h"

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace Zilu;
using namespace Protocol;

int path_check(const char* path) {
    if (access(path, 0) != 0) {
        return mkdir(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    }
    return 0;
}

void init_logs()
{
    path_check("./logs");

    // 创建日志配置
    LogConfig config;
    config.fileName = "logs/gbserver.log";
    config.maxFileSize = 100 * 1024 * 1024;  // 100MB
    config.maxFiles = 30;
    config.flushInterval = std::chrono::seconds(3);
    LogWrapper::GetInstance().Init(config, LogWrapper::OutMode::SYNC, LogWrapper::OutPosition::CONSOLE_AND_FILE);
}

int main(int argc, char* argv[])
{
    //初始化全局配置
    std::string path = "./config.ini";
    CConfigManager::instance()->Load(path.c_str());

    log_config_t log;
    CConfigManager::instance()->GetLogConfig(log);

    init_logs();

    ///初始化
    GB28181::CGB28181SvrManager::instance()->Init();
    GB28181::CGB28181SvrManager::instance()->Start();

    while (1) {
        sleep(2*1000);
    }

    return 0;
}
