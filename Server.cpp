#include "Server.h"

Server::Server(int port, size_t t_count): acceptor(io_service, tcp::endpoint(tcp::v4(), port)), threads_count(t_count) {
    acceptor.non_blocking(true);
}

void Server::start() {
    for (int i = 0; i < threads_count; i++) {
        thread_pool.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));
        io_service.post(boost::bind(&Server::start_accept, this));
    }
    std::cout << "Server starts with " << threads_count << " threads" << std::endl;
    thread_pool.join_all();
}

void Server::start_accept() {
    Connection::connection_pointer new_connection(new Connection(io_service));

    acceptor.async_accept(new_connection->get_socket(),
            boost::bind(&Server::handle_accept, this, new_connection, boost::asio::placeholders::error)
    );
}

void Server::handle_accept(Connection::connection_pointer new_conn, const boost::system::error_code &error) {
    if (!acceptor.is_open()) {
        return;
    }

    if (!error) {
        new_conn->start();
    }

    start_accept();
}
