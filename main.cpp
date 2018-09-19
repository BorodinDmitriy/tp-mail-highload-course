#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string makeTestResponseString() 
{
    std::time_t current_time = std::time(0);
    return std::ctime(&current_time);
}

class Connection : public boost::enable_shared_from_this<Connection>
{
    // Usage of shared_ptr and enable_shared_from_this
    // is needed to keep connection alive as long as operation lives
private:
    tcp::socket socket;
    std::string response_message;
    Connection(boost::asio::io_service& io_service) : socket(io_service) {}
    // write_handler is needed for proceeding further actions with connection
    void write_handler(const boost::system::error_code&, size_t /* amount of transferred bytes*/) {}
public:
    typedef boost::shared_ptr<Connection> connection_pointer;
    static connection_pointer create_connection(boost::asio::io_service& io_service) 
    {
        return connection_pointer(new Connection(io_service));
    }
    tcp::socket& socket() 
    {
        return socket;
    }
    // boost::asio::async_write() is used to serve data to a client
    // it is used because we ensure sending of entire data block
    void start() 
    {
        // The sending data is kept in response_message as we need
        // to keep the data valid until the async operation is complete
        response_message = makeTestResponseString();
        // When initiate the async operation 
        // and when we use boost::bind we need to specify only write_handler's 
        // params list, i.e. other could be omitted if needed
        boost::asio::async_write(socket, boost::asio::buffer(response_message), 
                                 boost::bind(&Connection::write_handler, shared_from_this(),
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

};


class Server {
private:
   tcp::acceptor acceptor;

   // make_accept creates a socket and initiates an async operation to wait for a connection
   void make_accept() {
       // Create a socket
       Connection::connection_pointer new_connection = Connection::create_connection(acceptor.get_io_service());
       // Initiate an async operation to wait for a new connection
       acceptor.async_accept(new_connection->socket(),boost::bind(&Server::accept_handler, this, new_connection, 
                                                                  boost::asio::placeholders::error));
   }

   // accept_handler is called when async operation initiated by 
   // make_accept() finishes. accept_handler() services a client request
   void accept_handler(Connection::connection_pointer new_connection, const boost::system::error_code& error_code)
   {
        if (!error_code) {
            new_connection->start();
        }
        // Call make_accept to initialize next accept operation
        make_accept();
   }
public:
   // Constructor initializes acceptor to listen TCP port 8000
   Server(boost::asio::io_service& io_service) : acceptor(io_service, tcp::endpoint(tcp::v4(),8000)) 
   {
       make_accept();
   }
};




int main(void) {
    try
    {
        // At least one io_service is needed in boost::asio program to initiate asynchronous operations
        boost::asio::io_service io_service;

        Server server(io_service);

        // Run io_service
        io_service.run();
    }
    catch (std::exception& e)
    {
         std::cerr << e.what() << std::endl;
    }
    return 0;
}
