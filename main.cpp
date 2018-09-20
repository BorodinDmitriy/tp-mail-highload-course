#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include "Server.h"

int main(void) {
    try
    {
        int port = 80;
        string root = "/var/www/html";
        int threads = 256;

        std::ifstream conf_file_stream("/etc/httpd.conf");

        if (conf_file_stream.is_open()) {
            char* buf = new char[100];
            std::streamsize n = 100;

            for (int i = 0; i < 3 && conf_file_stream.good(); i++) {
                conf_file_stream.getline(buf, n);
                string config_data(buf);

                std::size_t found_bytes_count = config_data.find_last_of(' ');

                if (strstr(config_data.substr(0, found_bytes_count).c_str(), "listen")) {
                    port = std::atoi(config_data.substr(found_bytes_count + 1).c_str());
                }

                if (strstr(config_data.substr(0, found_bytes_count).c_str(), "document_root")) {
                    root = config_data.substr(found_bytes_count + 1);
                }

                if (strstr(config_data.substr(0, found_bytes_count).c_str(), "threads_limit")) {
                    threads = std::atoi(config_data.substr(found_bytes_count + 1).c_str());
                }

            }

            conf_file_stream.close();
            delete (buf);
        }

        Server server(port, root, threads);
        server.start();
    }
    catch (std::exception& e)
    {
         std::cerr << e.what() << std::endl;
    }
    return 0;
}
