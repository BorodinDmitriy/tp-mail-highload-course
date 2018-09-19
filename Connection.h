#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <sys/sendfile.h>


using boost::asio::ip::tcp;

class Connection: public boost::enable_shared_from_this<Connection> {
public:
    typedef boost::shared_ptr<Connection> connection_pointer;

    tcp::socket& get_socket() {
        return socket;
    };

    void start();

    Connection(boost::asio::io_service&);

    ~Connection();

private:

    void handle_read(const boost::system::error_code &, size_t);
    void handle_write(const boost::system::error_code&, size_t);

    void stop();

    tcp::socket socket;


};


#endif // CONNECTION_H

