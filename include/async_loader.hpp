#pragma once

#include <boost/asio.hpp>
#include "request.hpp"
#include "thread_pool.hpp"


class AsyncLoader {
    private:
        ThreadPool& pool_;

    public:
        explicit AsyncLoader(ThreadPool& pool);

        boost::asio::awaitable<void> load(const Request& request);
};
