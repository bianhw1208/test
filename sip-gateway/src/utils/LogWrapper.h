#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h> //support for async logging

#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/bin_to_hex.h"

// 日志配置结构体
struct LogConfig {
    std::string fileName;           // 日志文件路径
    size_t maxFileSize{50*1024*1024}; // 默认最大文件大小50MB
    size_t maxFiles{10};           // 默认最大文件数
    size_t queueSize{102400};      // 异步队列大小
    size_t threadCount{1};         // 异步线程数
    std::chrono::seconds flushInterval{3}; // 刷新间隔
};

class LogWrapper {
public:
    enum OutPosition {
        CONSOLE = 0x01,
        FILE = 0x02,
        CONSOLE_AND_FILE = 0x03,
    };

    enum OutMode {
        SYNC,
        ASYNC,
    };

    enum OutLevel {
        LEVEL_TRACE = spdlog::level::trace,
        LEVEL_DEBUG = spdlog::level::debug,
        LEVEL_INFO = spdlog::level::info,
        LEVEL_WARN = spdlog::level::warn,
        LEVEL_ERROR = spdlog::level::err,
        LEVEL_CRITICAL = spdlog::level::critical,
        LEVEL_OFF = spdlog::level::off,
    };

public:
    static LogWrapper& GetInstance();
    ~LogWrapper();

    // 使用结构体配置初始化
    bool Init(const LogConfig& config,
             const OutMode outMode = SYNC,
             const OutPosition outPos = CONSOLE_AND_FILE,
             const OutLevel outLevel = LEVEL_TRACE);

    // 设置日志级别
    void SetLevel(OutLevel level);
    
    // 获取当前日志级别
    OutLevel GetLevel() const;
    
    // 立即刷新日志
    void Flush();
    
    // 获取logger实例
    std::shared_ptr<spdlog::logger> GetLogger() const { return m_pLogger; }
    
    void UnInit();

private:
    LogWrapper();
    LogWrapper(const LogWrapper&) = delete;
    LogWrapper& operator=(const LogWrapper&) = delete;

    bool CreateSinks(const LogConfig& config, 
                    const OutPosition outPos,
                    std::vector<spdlog::sink_ptr>& sinks);

private:
    bool m_bInit{false};
    static std::shared_ptr<LogWrapper> instance_;
    static std::mutex mutex_;
    std::shared_ptr<spdlog::logger> m_pLogger;
    LogConfig m_config;
};

// 日志宏定义
#define LOG_NAME "multi_sink"
#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::info, __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(spdlog::get(LOG_NAME), spdlog::level::critical, __VA_ARGS__)
