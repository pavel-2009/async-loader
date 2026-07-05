#include "async_loader.hpp"
#include "request.hpp"
#include "thread_pool.hpp"

#include <boost/asio.hpp>
#include <future>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>



AsyncLoader::AsyncLoader(ThreadPool& pool) : pool_(pool) {}


boost::asio::awaitable<std::string> AsyncLoader::load(const Request& request) {

    auto executor = co_await boost::asio::this_coro::executor;

    pool_.submit([executor, request]() {
        std::cout << "[AsyncLoader, Thread " << std::this_thread::get_id() << "] Loading request " << request.id << " from " << request.filename << "\n" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate a delay for loading

        std::ifstream file(request.filename);
        std::string result;
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                result += line + "\n";
            }
            file.close();
        } else {
            result = "Failed to open file: " + request.filename;
        };

        boost::asio::post(executor, [result]() {
            std::cout << "[AsyncLoader, Thread " << std::this_thread::get_id() << "] Finished loading request. Result:\n" << result << "\n" << std::endl;
        });

    });

    co_return result;
};
    