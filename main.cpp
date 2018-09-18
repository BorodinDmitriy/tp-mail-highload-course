#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

std::string makeTestResponseString() 
{
    std::time_t current_time = std::time(0);
    return std::ctime(&current_time);
}

int main(void) {
    try
    {
        // At least one io_service is needed in boost::asio program to initiate asynchronous operations
        boost::asio::io_service io_service;

        // Acceptor is needed for listening to new connections
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(),8000));

        for (;;)
        {
            // Create a socket
            tcp::socket socket(io_service);
            // Make acceptor wait and listen for new connections
            acceptor.accept(socket);
            // Make response string to send to a client
            std::string response = makeTestResponseString();

            // Error for writing in buffer
            boost::system::error_code error_code;
            // Write a response to client
            boost::asio::write(socket, boost::asio::buffer(response), error_code);
        }
    }
    catch (std::exception& e)
    {
         std::cerr << e.what() << std::endl;
    }
    return 0;
}
