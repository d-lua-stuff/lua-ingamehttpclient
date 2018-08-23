#pragma once

#include "stdafx.h"
#include "debug.h"

class Context // TODO: store configuration, active thread count and other things
{
public:
	Context()
	{
		DEBUG_PRINT("in Context()");

		Poco::Net::initializeSSL();
	}

	~Context()
	{
		DEBUG_PRINT("in ~Context()");

		Poco::Net::uninitializeSSL();
	}

	std::string someSetting = "someValue";
};

using ContextSharedPtr = std::shared_ptr<Context>;
