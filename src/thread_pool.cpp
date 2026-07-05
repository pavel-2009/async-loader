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

        std::cout << "Task submitted to thread pool. Total tasks in queue: " << tasks_.size() << std::endl;
    };

    condition_.notify_one();
    std::cout << "Notified one worker thread." << std::endl;
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

        std::cout << "Worker thread executing task. Remaining tasks in queue: " << tasks_.size() << std::endl;

        // Checking free threads and executing the task

        std::jthread::id current_thread_id = std::this_thread::get_id();
        {
            std::lock_guard lock(mutex);
            for (const std::jthread& worker : workers_) {
                if (!worker.joinable()) {
                    std::cout << "Worker thread is free. Executing task." << std::endl;
                    break;
                }

                current_thread_id = std::this_thread::get_id();

            }

        }

        std::jthread& current_worker = *std::find_if(workers_.begin(), workers_.end(),
            [current_thread_id](const std::jthread& worker) {
                return worker.get_id() == current_thread_id;
            });

        current_worker.detach(); // Detach the current worker thread to allow it to run independently
        current_worker = std::jthread([task = std::move(task)]() mutable {
            task();
        });

        std::cout << "Worker thread finished executing task." << std::endl;
    }
}
