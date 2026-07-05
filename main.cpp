#include "async_loader.hpp"
#include "request.hpp"
#include "thread_pool.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <future>
#include <fstream>
#include <iostream>

int main() {

    std::cout << "Starting async loader..." << std::endl;

    boost::asio::io_context io_context;

    ThreadPool thread_pool(3); // Create a thread pool with 3 threads
    AsyncLoader async_loader(thread_pool);

    Request request1(1, "file1.txt");
    Request request2(2, "file2.txt");
    Request request3(3, "file3.txt");
    Request request4(4, "file4.txt");
    Request request5(5, "file5.txt");
    Request request6(6, "file6.txt");

    boost::asio::co_spawn(io_context, async_loader.load(request1), boost::asio::detached);
    boost::asio::co_spawn(io_context, async_loader.load(request2), boost::asio::detached);
    boost::asio::co_spawn(io_context, async_loader.load(request3), boost::asio::detached);
    boost::asio::co_spawn(io_context, async_loader.load(request4), boost::asio::detached);
    boost::asio::co_spawn(io_context, async_loader.load(request5), boost::asio::detached);
    boost::asio::co_spawn(io_context, async_loader.load(request6), boost::asio::detached);

    io_context.run();

    std::cout << "All tasks submitted. Waiting for completion..." << std::endl;

    return 0;
    
}
