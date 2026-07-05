#include "async_loader.hpp"
#include "request.hpp"
#include "thread_pool.hpp"

#include <boost/asio.hpp>
#include <future>
#include <fstream>
#include <iostream>
#include <memory>


AsyncLoader::AsyncLoader(ThreadPool& pool) : pool_(pool) {}


boost::asio::awaitable<void> AsyncLoader::load(const Request& request) {
    auto promise = std::make_shared<std::promise<std::string>>();
    std::future<std::string> future = promise->get_future();

    pool_.submit([request, promise]() {
        std::cout << "[AsyncLoader] Loading request " << request.id << " from " << request.filename << std::endl;

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

        promise->set_value(result);
    });

    auto executor = co_await boost::asio::this_coro::executor;
    co_await boost::asio::post(executor, boost::asio::use_awaitable);

    co_return;
};
    