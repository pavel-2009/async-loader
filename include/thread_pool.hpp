#pragma once

#include <condition_variable>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>


class ThreadPool {

    private:
        std::vector<std::jthread> workers_;

        std::queue<std::function<void()>> tasks_;

        std::mutex mutex;

        std::condition_variable condition_;

        bool stop_ = false;

    private:

        void worker();

    public:

        explicit ThreadPool(std::size_t threadCount);

        ~ThreadPool();

        void submit(std::function<void()> task);

};
