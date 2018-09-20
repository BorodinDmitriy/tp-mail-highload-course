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

bool url_decode(const std::string &input, std::string &result) {
    result.clear();
    for (int i = 0; i < input.size(); ++i) {
        if (input[i] == '%') {
            if (i + 3 <= input.size()) {
                int val = 0;
                std::istringstream is(input.substr(i + 1, 2));
                if (is >> std::hex >> val) {
                    result += static_cast<char>(val);
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
        else if (input[i] == '+') {
            result += ' ';
        }
        else {
            result += input[i];
        }
    }
    return true;
}

string remove_query(const std::string &path) {
    return path.substr(0, path.find_last_of('?')); 
}

const std::string current_date_time() {
    time_t now = time(0);
    struct tm time_struct;
    char buffer[80];
    time_struct = *localtime(&now);
    strftime(buffer, sizeof(buffer), "%a, %d %B %Y %X GMT", &time_struct);

    return buffer;
}

void Response::head(string root_directory,string path, std::function<void (const string&)> headers_sender) {
    get(root_directory, path, headers_sender, nullptr, false);
}

void Response::get(string root_directory, string path_orig, std::function<void (const string&)> headers_sender, std::function<void (int, size_t)> file_sender, bool needs_to_send_file) {

    string path;
    string path_without_query = remove_query(path_orig);

    if (!url_decode(path_without_query, path)) {
        bad_request(headers_sender); 
    }

    fs::path relative_path(root_directory + path);
    bool does_directory_exist = false;
    if (is_directory(relative_path)) { 
        relative_path /= "index.html";
        does_directory_exist = true;
    }

    boost::system::error_code err_code;
    fs::path absolute_path = canonical(relative_path, root_directory, err_code);
    if (err_code) {
        if (does_directory_exist) {
            forbidden(headers_sender); 
        } else {
            not_found(headers_sender); 
        }
        return;
    }

    if(!check_root(absolute_path.string(), root_directory)) {
        not_found(headers_sender); 
        return;
    }

    boost::uintmax_t size_of_file = file_size(absolute_path, err_code);

    string content_type = file_type_definition(absolute_path.string());
    string length = CONTENT_LENGTH + std::to_string(size_of_file);

    headers_sender(OK + headers() + content_type + length + "\r\n\r\n");

    if (needs_to_send_file) {
        int fd = open(absolute_path.c_str(), O_RDONLY);
        file_sender(fd, size_of_file);
        close(fd);
    }
}

string Response::file_type_definition(string filename) {

    string type = filename.substr(filename.find_last_of('.') + 1);

    const string html_string = string("html");
    const string txt_string = string("txt");
    const string css_string = string("css");
    const string gif_string = string("gif");
    const string jpeg_string = string("jpeg");
    const string jpg_string = string("jpg");
    const string js_string = string("js");
    const string png_string = string("png");
    const string swf_string = string("swf");

    if (type == html_string) return CONTENT_TYPE_HTML;
    if (type == css_string) return CONTENT_TYPE_CSS;
    if (type == gif_string) return CONTENT_TYPE_GIF;
    if (type == jpeg_string) return CONTENT_TYPE_JPEG;
    if (type == jpg_string) return CONTENT_TYPE_JPEG;
    if (type == js_string) return CONTENT_TYPE_JS;
    if (type == png_string) return CONTENT_TYPE_PNG;
    if (type == swf_string) return CONTENT_TYPE_SWF;

    return CONTENT_TYPE_HTML;
}

bool Response::check_root(string root_directory, string path) {
    return boost::algorithm::contains(root_directory, path);
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