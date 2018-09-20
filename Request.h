#ifndef REQUEST_H
#define REQUEST_H

#include <cstdlib>
#include <boost/asio.hpp>
#include "Response.h"

using std::string;

class Request 
{
private:
	string method;
	string url;
	string version;
	string encoding;
	string content_length;

	const string GET = "GET";
	const string HEAD = "HEAD";

	Response response;
public:
	Request();

	void parse(string, size_t, std::function<void (const string&)>);

	bool is_get_or_head();

};

#endif // REQUEST_H