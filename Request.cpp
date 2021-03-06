#include <iostream>
#include "Request.h"

using std::string;
using std::vector;

Request::Request(string root_directory) : root(root_directory) {
}

void Request::parse(string request, size_t size, std::function<void (const string&)> headers_sender, std::function<void (int, size_t)> file_sender) {

    std::istringstream request_string(request);
    request_string >> method;

    if (!is_get_or_head()) {
        response.not_allowed(headers_sender);
        return;
    }

    request_string >> url >> version;
    request_string.ignore(100, '\n'); // \n after HTTP/1.1
    request_string.ignore(100, '\n'); // Host: localhost:8000
    request_string.ignore(100, ':');  // Accept-Encoding:
    request_string >> encoding;
    request_string.ignore(100, '\n'); // \n after identity
    request_string.ignore(100, ':');  // Content-Length:
    request_string >> content_length;

    if (method == GET) {
        response.get(root, url, headers_sender, std::move(file_sender), true);
        return;
    } else if (method == HEAD) {
        response.head(root, url, headers_sender);
    }
    return;
}

bool Request::is_get_or_head() {
    return method == GET || method == HEAD;
}