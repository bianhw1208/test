#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include <memory>
#include <string>

namespace Infra {

// 线程任务的基础类型
using ThreadProc = std::function<void(void*)>;
using ThreadTask = std::function<void(void)>;  // 无参数的任务类型

class CThread {
public:
    explicit CThread(const std::string& name = "unknown") 
        : m_name(name), m_running(false) {}
    
    virtual ~CThread() {
        stop();
    }

    // 启动线程
    bool start(const ThreadProc& proc, void* param = nullptr) {
        if (m_running) return false;
        
        m_running = true;
        m_thread = std::thread([this, proc, param]() {
            while (m_running) {
                proc(param);
            }
        });
        return true;
    }

    // 停止线程
    void stop() {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    // 检查线程是否在运行
    bool looping() const { return m_running; }

    // 线程睡眠辅助函数
    static void sleep(unsigned long milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

private:
    std::string m_name;
    std::thread m_thread;
    std::atomic<bool> m_running;
};

} // namespace Infra 