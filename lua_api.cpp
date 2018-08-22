#include "stdafx.h"
#include "debug.h"
#include "Request.h"

// some references
// https://stackoverflow.com/questions/38718475/lifetime-of-lua-userdata-pointers
// https://stackoverflow.com/questions/3481856/sending-variable-pointers-back-and-forth-between-c-and-lua
// https://gist.github.com/kizzx2/5221139

static const char *metatableName = "ingamehttpclient_Response";

static ResponseSharedPtr *l_checkResponse(lua_State *L, int n)
{
	ResponseSharedPtr *ptrResponse = static_cast<ResponseSharedPtr*>(luaL_checkudata(L, n, metatableName));

	if (ptrResponse->get() == nullptr)
		luaL_error(L, "attempting to access a disposed response");

	return ptrResponse;
}

static int l_response_gc(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_gc()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);
	ptrResponse->reset();
	
	return 0;
}

static int l_response_newindex(lua_State *L)
{
	// without this, scripters could overwrite __gc and cause memory leaks

	luaL_error(L, "modification of this table is not allowed");
	return 0;
}

static int l_response_tostring(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_tostring()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);

	if ((*ptrResponse)->isPending)
		lua_pushstring(L, "response (pending)");
	else
		lua_pushstring(L, "response (ready)");

	return 1;
}

static int l_response_isPending(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_isPending()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);

	lua_pushboolean(L, (*ptrResponse)->isPending);
	return 1;
}

static int l_response_getStatus(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_getStatus()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);
	auto& ptrStatus = (*ptrResponse)->status;

	if ((*ptrResponse)->isPending || ptrStatus.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushinteger(L, *ptrStatus);

	return 1;
}

static int l_response_getReason(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_getReason()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);
	auto& ptrReason = (*ptrResponse)->reason;

	if ((*ptrResponse)->isPending || ptrReason.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushstring(L, ptrReason->c_str());

	return 1;
}

static int l_response_getContent(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_response_getContent()");

	ResponseSharedPtr *ptrResponse = l_checkResponse(L, 1);
	auto& ptrContent = (*ptrResponse)->content;

	if ((*ptrResponse)->isPending || ptrContent.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushstring(L, ptrContent->c_str());

	return 1;
}

static int l_request(lua_State *L)
{
	DEBUG_PRINT("ingamehttpclient: in l_request()");

	luaL_checktype(L, 1, LUA_TTABLE);

	try
	{
		Request request;

		// get request URL
		{
			lua_pushstring(L, "url");
			const int urlLuaType = lua_rawget(L, -2);

			if (LUA_TSTRING == urlLuaType)
			{
				const char *urlStr = luaL_checkstring(L, -1);
				request.url = urlStr;
			}
			else
			{
				throw std::invalid_argument("argument #1 key 'url' must be string");
			}

			lua_pop(L, 1);
		}

		// get request method
		{
			lua_pushstring(L, "method");
			const int methodUrlType = lua_rawget(L, -2);

			if (LUA_TSTRING == methodUrlType)
			{
				const char *methodStr = luaL_checkstring(L, -1);
				request.method = methodStr;
			}
			else if (LUA_TNIL == methodUrlType)
			{
				request.method = "GET";
			}
			else
			{
				throw std::invalid_argument("argument #1 key 'method' must be string or nil");
			}

			lua_pop(L, 1);
		}

		// get request content
		{
			lua_pushstring(L, "body");
			const int bodyLuaType = lua_rawget(L, -2);

			if (LUA_TSTRING == bodyLuaType)
			{
				const char *bodyStr = luaL_checkstring(L, -1);
				request.body = bodyStr;
			}
			else if (LUA_TNIL != bodyLuaType)
			{
				throw std::invalid_argument("argument #1 key 'body' must be string or nil");
			}

			lua_pop(L, 1);
		}

		//  get request headers

		{
			lua_pushstring(L, "headers");
			const int headerLuaType = lua_rawget(L, -2);

			if (LUA_TTABLE == headerLuaType)
			{
				lua_pushnil(L);
				while (lua_next(L, -2))
				{
					if (LUA_TSTRING != lua_type(L, -2))
					{
						throw new std::invalid_argument("header name must be string");
					}
					if (LUA_TSTRING != lua_type(L, -1))
					{
						throw new std::invalid_argument("header value must be string");
					}

					const char *keyStr = lua_tostring(L, -2);
					const char *valStr = lua_tostring(L, -1);

					request.headers[keyStr] = valStr;

					lua_pop(L, 1);
				}
			}
			else if (LUA_TNIL != headerLuaType)
			{
				throw std::invalid_argument("argument #1 key 'headers' must be table or nil");
			}

			lua_pop(L, 1);
		}

		// start the request and get the response object

		void* buffer = lua_newuserdata(L, sizeof(ResponseSharedPtr));
		ResponseSharedPtr *ptrResponse = new(buffer) ResponseSharedPtr(request.start());
	}
	catch (std::exception& e)
	{
		luaL_error(L, e.what());
	}
	catch (...)
	{
		luaL_error(L, "unknown error");
	}

	luaL_setmetatable(L, metatableName);
	return 1;
}

static const struct luaL_Reg ingamehttpclient_meta[] = {
	{ "__gc", l_response_gc },
	{ "__tostring", l_response_tostring },
	{ "isPending", l_response_isPending },
	{ "getStatus", l_response_getStatus },
	{ "getReason", l_response_getReason },
	{ "getContent", l_response_getContent },
	{ NULL, NULL }
};

static const struct luaL_Reg ingamehttpclient_lib[] = {
	{ "request", l_request },
	{ NULL, NULL }
};

extern "C" __declspec(dllexport)
int luaopen_ingamehttpclient(lua_State *L)
{
	// todo: instead of initializing here, store a bool in the registry
	initRequest();

	luaL_newmetatable(L, metatableName);
	luaL_setfuncs(L, ingamehttpclient_meta, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");
	lua_pushcfunction(L, l_response_newindex);
	lua_setfield(L, -2, "__newindex");
	
	// luaL_newlib(L, ingamehttpclient_lib); // do not use - luaL_checkversion(L) will complain about multiple Lua VMs in <5.3.5, since games' Lua is often linked statically, and this library's Lua is too
	luaL_newlibtable(L, ingamehttpclient_lib);
	luaL_setfuncs(L, ingamehttpclient_lib, 0);
	return 1;
}
