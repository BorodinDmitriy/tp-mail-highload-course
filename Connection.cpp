#include <iostream>
#include <boost/thread/thread.hpp>
#include "Connection.h"

Connection::Connection(boost::asio::io_service &io_service, string root): socket(io_service), request(root), file_offset(0) {
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
            boost::asio::buffer(buf, buffer_size),
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
    request.parse(std::string(buf), size,
                         std::bind(&Connection::send_message, shared_from_this(), std::placeholders::_1),
                         std::bind(&Connection::send_file, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::send_message(const std::string &message) {

    socket.async_write_some(
             boost::asio::buffer(message, message.size()),
             boost::bind(&Connection::handle_write, shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred)

     );
}

void Connection::send_file(int fd, size_t size) {
    ssize_t result_size = 0;
    while (file_offset < size) {
        result_size = sendfile(socket.native_handle(), fd, &file_offset, file_part_size);
        if (result_size < 0) {
            std::cerr << "error: " << errno << std::endl;
            if (errno == 32 || errno == 104) {
                 stop();
                 return;
            }
        }
    }
}

void Connection::handle_write(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        stop();
        return;
    }
}


