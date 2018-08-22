// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define POCO_STATIC
#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

// Windows.h is included by POCO's UnWindows.h

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

extern "C" {
#include <lua.h>
#include <lstate.h>
#include <lauxlib.h>
}
