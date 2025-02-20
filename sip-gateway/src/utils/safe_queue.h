#pragma once

#include <queue>
#include <mutex>

namespace Gateway {

template <typename T>
class CSafeQueue
{
public:
    CSafeQueue() = default;

    ~CSafeQueue() = default;

    void Push(const T& in)
    {
        std::lock_guard<std::mutex> g(m_mutex);
        m_queue.push(in);
    }

    bool Pop(T& out)
    {
        std::lock_guard<std::mutex> g(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        out = m_queue.front();
        m_queue.pop();
        return true;
    }

private:
    std::queue<T> m_queue;
    std::mutex    m_mutex;
};

}
