#include "../include/thread_pool.hpp"

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <iostream>


ThreadPool::ThreadPool(std::size_t threadCount) {

    std::cout << "Creating thread pool with " << threadCount << " threads." << std::endl;
    for (size_t i = 0; i < threadCount; i++)
    {
        std::cout << "Creating thread " << i + 1 << std::endl;
        workers_.emplace_back([this]{
            worker();
        });
    }
}


ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(mutex);
        stop_ = true;
    };

    condition_.notify_all();

    for (std::jthread& worker : workers_) {
        if (worker.joinable())
            worker.join();
    }
}


void ThreadPool::submit(std::function<void()> task) {
    {
        std::lock_guard lock(mutex);
        tasks_.push(std::move(task));

        std::cout << "Task submitted to thread pool. Total tasks in queue: " << tasks_.size() << "\n" << std::endl;
    };

    condition_.notify_one();
    std::cout << "Notified one worker thread." << "\n" << std::endl;
}


void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;

        // Checking tasks list and waiting for new tasks
        {
            std::unique_lock lock(mutex);
            condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

            if (stop_ && tasks_.empty())
                return;

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        std::cout << "Worker thread " << std::this_thread::get_id() << " executing task. Remaining tasks in queue: " << tasks_.size() << "\n" << std::endl;

        task();        

        std::cout << "Worker thread " << std::this_thread::get_id() << " finished executing task." << "\n" << std::endl;
    }
}
