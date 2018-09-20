#include <ctime>
#include <iostream>
#include <string>
#include "Response.h"
#include "boost/filesystem/operations.hpp"
#include "boost/progress.hpp"
#include "boost/algorithm/string.hpp"
#include <boost/lexical_cast.hpp>
#include "boost/date_time/time_facet.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

bool url_decode(const std::string &in, std::string &result) {
    result.clear();
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    result += static_cast<char>(value);
                    i += 2;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (in[i] == '+') {
            result += ' ';
        }
        else {
            result += in[i];
        }
    }
    return true;
}

string remove_query(const std::string &path) {
    std::size_t found = path.find_last_of('?');
    return path.substr(0, found); 
}

const std::string current_date_time() {
    std::time_t current_time = std::time(0);
    return std::ctime(&current_time);
}

void Response::head(string path, std::function<void (const string&)> headers_sender) {
    get(path, headers_sender);
}

void Response::get(string path_orig, std::function<void (const string&)> headers_sender) {

    string path;
    string path_without_query = remove_query(path_orig);

    if (!url_decode(path_without_query, path)) {
        bad_request(headers_sender); // wrong url
    }

    headers_sender(OK + headers() +  "\r\n\r\n");
}

void Response::not_found(std::function<void (const string&)> headers_sender) {
    headers_sender(NOT_FOUND + headers());
}

void Response::not_allowed(std::function<void(const string &)> headers_sender) {
    headers_sender(NOT_ALLOWED + headers());
}

void Response::forbidden(std::function<void(const string &)> headers_sender) {
    headers_sender(FORBIDDEN + headers());
}

void Response::bad_request(std::function<void(const string &)> headers_sender) {
    headers_sender(BAD_REQUEST + headers());
}

string Response::headers() {
    string current_date = DATE + current_date_time() + "\r\n"; 
    return current_date + SERVER + CONNECTION;
}