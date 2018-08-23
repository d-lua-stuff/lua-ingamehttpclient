#pragma once

#include "stdafx.h"
#include "debug.h"
#include "Response.h"

class Request {
public:
#if defined(_DEBUG)
	Request()
	{
		DEBUG_PRINT("in Request()");
	}

	~Request()
	{
		DEBUG_PRINT("in ~Request()");
	}
#endif

	std::string url;
	std::string method;
	std::string body;
	std::map<std::string, std::string> headers;

	ResponseSharedPtr start() const;
};
