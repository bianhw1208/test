#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <condition_variable>
#include <thread>
#include <functional>
#include <stdexcept>
#include <memory>

#include "Thread.h"
namespace Infra {

class CThreadPool {
public:
	static CThreadPool* instance() {
		static CThreadPool ins;
		return &ins;
	}

	CThreadPool(size_t threads = 4) {
		for (size_t i = 0; i < threads; ++i) {
			workers.emplace_back([this] {
				while (true) {
					ThreadTask task;
					{
						std::unique_lock<std::mutex> lock(queue_mutex);
						condition.wait(lock, [this] {
							return stop || !tasks.empty();
						});
						if (stop && tasks.empty()) {
							return;
						}
						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
			});
		}
	}

	~CThreadPool() {
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread& worker : workers) {
			worker.join();
		}
	}

	// 运行任务
	void run(const ThreadTask& task) {
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			tasks.push(task);
		}
		condition.notify_one();
	}

private:
	std::vector<std::thread> workers;
	std::queue<ThreadTask> tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop = false;
};

} // namespace Infra

#endif
