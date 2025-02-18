#include "LogWrapper.h"
#include <fstream>
#include <iostream>
#include <string>

std::shared_ptr<LogWrapper> LogWrapper::instance_ = nullptr;
std::mutex LogWrapper::mutex_;

LogWrapper& LogWrapper::GetInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        instance_.reset(new LogWrapper());
    });
    return *instance_;
}

LogWrapper::LogWrapper() = default;

LogWrapper::~LogWrapper() {
    if (m_bInit) {
        UnInit();
    }
}

bool LogWrapper::CreateSinks(const LogConfig& config, 
                           const OutPosition outPos,
                           std::vector<spdlog::sink_ptr>& sinks) {
    const char* pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%@,%!] %v \n";
    
    if (outPos & CONSOLE) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        /*console_sink->set_color(spdlog::level::trace, console_sink->white);
        console_sink->set_color(spdlog::level::debug, console_sink->cyan);
        console_sink->set_color(spdlog::level::info, console_sink->green);
        console_sink->set_color(spdlog::level::warn, console_sink->yellow);
        console_sink->set_color(spdlog::level::err, console_sink->red);
        console_sink->set_color(spdlog::level::critical, console_sink->bold);*/
        
        console_sink->set_pattern(pattern);
        sinks.push_back(console_sink);
    }
    
    if (outPos & FILE) {
        try {
            auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                config.fileName, 0, 0);
            daily_sink->set_pattern(pattern);
            sinks.push_back(daily_sink);
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Failed to create daily file sink: " << ex.what() << std::endl;
            return false;
        }
    }
    
    return true;
}

bool LogWrapper::Init(const LogConfig& config,
                     const OutMode outMode,
                     const OutPosition outPos,
                     const OutLevel outLevel) {
    if (m_bInit) {
        std::cerr << "Logger already initialized" << std::endl;
        return false;
    }

    try {
        std::vector<spdlog::sink_ptr> sinks;
        if (!CreateSinks(config, outPos, sinks)) {
            return false;
        }

        if (outMode == ASYNC) {
            spdlog::init_thread_pool(config.queueSize, config.threadCount);
            auto tp = spdlog::thread_pool();
            m_pLogger = std::make_shared<spdlog::async_logger>(
                LOG_NAME, sinks.begin(), sinks.end(), tp, 
                spdlog::async_overflow_policy::block);
        } else {
            m_pLogger = std::make_shared<spdlog::logger>(
                LOG_NAME, sinks.begin(), sinks.end());
        }

        m_pLogger->set_level(static_cast<spdlog::level::level_enum>(outLevel));
        m_pLogger->flush_on(spdlog::level::warn);
        spdlog::flush_every(config.flushInterval);
        spdlog::register_logger(m_pLogger);

        m_config = config;
        m_bInit = true;
        return true;
    }
    catch(const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return false;
    }
}

void LogWrapper::SetLevel(OutLevel level) {
    if (m_pLogger) {
        m_pLogger->set_level(static_cast<spdlog::level::level_enum>(level));
    }
}

LogWrapper::OutLevel LogWrapper::GetLevel() const {
    if (m_pLogger) {
        return static_cast<OutLevel>(m_pLogger->level());
    }
    return LEVEL_OFF;
}

void LogWrapper::Flush() {
    if (m_pLogger) {
        m_pLogger->flush();
    }
}

void LogWrapper::UnInit() {
    if (m_bInit) {
        spdlog::drop_all();
        spdlog::shutdown();
        m_bInit = false;
    }
}
