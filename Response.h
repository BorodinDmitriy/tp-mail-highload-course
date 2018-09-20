#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <boost/asio.hpp>

using std::string;

class Response 
{
private:
	string headers();
public:
	const string OK = string("HTTP/1.1 200 OK\r\n");
	const string BAD_REQUEST = string("HTTP/1.1 400 Bad Request\r\n");
	const string FORBIDDEN = string("HTTP/1.1 403 Forbidden\r\n");
	const string NOT_FOUND = string("HTTP/1.1 404 Not Found\r\n");
	const string NOT_ALLOWED = string("HTTP/1.1 405 Method Not Allowed\r\n");

	const string DATE = string("Date: ");
    const string SERVER = string("Server: Highload Static Server/1.0 (Ubuntu 14.04)\r\n");
    const string CONNECTION = string("Connection: close\r\n");
    const string CONTENT_LENGTH = string("Content-Length: ");
    const string CONTENT_TYPE_HTML = string("Content-Type: text/html\r\n");

    void get(string, std::function<void (const string&)>);
    void head(string, std::function<void (const string&)>);

    void not_found(std::function<void (const string&)>);
    void not_allowed(std::function<void (const string&)>);
    void forbidden(std::function<void (const string&)>);
    void bad_request(std::function<void (const string&)>);
};

#endif // RESPONSE_H