#include <iostream>
#include <boost/thread/thread.hpp>
#include "Connection.h"

Connection::Connection(boost::asio::io_service &io_service): socket(io_service) {
}

Connection::~Connection() {
    stop();
}

void Connection::stop() {
    if (socket.is_open()) {
        socket.close();
    }
}

void Connection::start() {
    socket.non_blocking(true);
    socket.native_non_blocking(true);

    socket.async_read_some(
            boost::asio::buffer(new char[10], 10),
            boost::bind(&Connection::handle_read, shared_from_this(),
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)

    );
}

void Connection::handle_read(const boost::system::error_code &error, size_t size) {
    if (error) {
        stop();
        return;
    }
    std::cout << "bla2" << std::endl;
    socket.async_write_some(boost::asio::buffer("OK", 2), boost::bind(&Connection::handle_write, shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred));
}

void Connection::handle_write(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        stop();
        return;
    }
}


