#pragma once

#include <boost/asio/awaitable.hpp>
#include "request.hpp"
#include "thread_pool.hpp"


class AsyncLoader {
    private:
        ThreadPool& pool_;

    public:
        explicit AsyncLoader(ThreadPool& pool);

        boost::asio::awaitable<std::string> load(const Request& request);
};
