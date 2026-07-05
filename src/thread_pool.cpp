#include "../include/thread_pool.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>


ThreadPool::ThreadPool(std::size_t threadCount) {
    for (size_t i = 0; i < threadCount; i++)
    {
        workers_.emplace_back([this]{
            worker();
        });
    }
}


void ThreadPool::submit(std::function<void()> task) {
    {
        std::lock_guard lock(mutex);
        tasks_.push(std::move(task));
    };

    condition_.notify_one();
}


void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock lock(mutex);
            condition_.wait(lock, [this]{
                return stop_ || !tasks_.empty();
            });

            if (stop_ && tasks_.empty())
                return;

            task = std::move(tasks_.front());
            tasks_.pop();
        };

        task();
    }
}
