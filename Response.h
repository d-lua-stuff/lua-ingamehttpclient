#pragma once

#include "stdafx.h"
#include "debug.h"

class Response
{
public:
	Response()
	{
		DEBUG_PRINT("ingamehttpclient: in Response()");
	}

	~Response()
	{
		DEBUG_PRINT("ingamehttpclient: in ~Response()");
	}

	std::atomic<bool> isPending;
	std::shared_ptr<Poco::Net::HTTPResponse::HTTPStatus> status;
	std::shared_ptr<std::string> reason;
	std::shared_ptr<std::string> content;

private:
};

using ResponseSharedPtr = std::shared_ptr<Response>;
