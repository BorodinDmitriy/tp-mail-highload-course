#ifndef SERVER_H
#define SERVER_H

#include "Connection.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

class Server {
public:
    Server(int, size_t);

    void start();
private:

    void start_accept();

    void handle_accept(Connection::connection_pointer new_connection, const boost::system::error_code &error);

    boost::asio::io_service io_service;
    tcp::acceptor acceptor;
    boost::thread_group thread_pool;
    size_t threads_count;
};

#endif // SERVER_H
