#include <ctime>
#include <iostream>
#include <string>
#include "Server.h"

std::string makeTestResponseString() 
{
    std::time_t current_time = std::time(0);
    return std::ctime(&current_time);
}


int main(void) {
    try
    {
        Server server(8000, 256);
        server.start();
    }
    catch (std::exception& e)
    {
         std::cerr << e.what() << std::endl;
    }
    return 0;
}
