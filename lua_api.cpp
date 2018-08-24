#include "stdafx.h"
#include "Context.h"
#include "Request.h"

// some references
// https://stackoverflow.com/questions/38718475/lifetime-of-lua-userdata-pointers
// https://stackoverflow.com/questions/3481856/sending-variable-pointers-back-and-forth-between-c-and-lua
// https://gist.github.com/kizzx2/5221139

static const char *contextMetatableName = "ingamehttpclient_Context";
static const char *responseMetatableName = "ingamehttpclient_Response";

static const char *contextKeyInRegistry = "ingamehttpclient_Context_reg";

static int l_newindex_forbidden(lua_State *L)
{
	// without this, scripters could overwrite __gc and cause memory leaks

	luaL_error(L, "modification of this table is not allowed");
	return 0;
}

static ContextSharedPtr l_checkContext(lua_State *L, int n)
{
	ContextSharedPtr ptrContext = *static_cast<ContextSharedPtr*>(luaL_checkudata(L, n, contextMetatableName));

	if (ptrContext.get() == nullptr)
		luaL_error(L, "attempting to access a disposed context");

	return ptrContext;
}

static ContextSharedPtr l_getContext(lua_State *L)
{
	ContextSharedPtr ptrContext;

	lua_pushstring(L, contextKeyInRegistry);
	const int contextLuaType = lua_rawget(L, LUA_REGISTRYINDEX);

	if (contextLuaType == LUA_TNIL)
	{
		lua_pop(L, 1);

		void *buffer = lua_newuserdata(L, sizeof(ContextSharedPtr));
		new(buffer) ContextSharedPtr(new Context());

		luaL_setmetatable(L, contextMetatableName);

		lua_pushstring(L, contextKeyInRegistry);
		lua_insert(L, -2);
		lua_rawset(L, LUA_REGISTRYINDEX);
	}
	else
	{
		ptrContext = l_checkContext(L, -1);
	}

	lua_pop(L, 1);
	return ptrContext;
}

static int l_context_gc(lua_State *L)
{
	DEBUG_PRINT("in l_context_gc()");

	ContextSharedPtr ptrContext = l_checkContext(L, 1);
	ptrContext.reset();

	return 0;
}

static ResponseSharedPtr l_checkResponse(lua_State *L, int n)
{
	ResponseSharedPtr ptrResponse = *static_cast<ResponseSharedPtr*>(luaL_checkudata(L, n, responseMetatableName));

	if (ptrResponse.get() == nullptr)
		luaL_error(L, "attempting to access a disposed response");

	return ptrResponse;
}

static int l_response_gc(lua_State *L)
{
	DEBUG_PRINT("in l_response_gc()");

	ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);
	ptrResponse.reset();
	
	return 0;
}

static int l_response_tostring(lua_State *L)
{
	DEBUG_PRINT("in l_response_tostring()");

	const ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);

	if (ptrResponse->isPending)
		lua_pushstring(L, "response (pending)");
	else
		lua_pushstring(L, "response (ready)");

	return 1;
}

static int l_response_isPending(lua_State *L)
{
	DEBUG_PRINT("in l_response_isPending()");

	const ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);

	lua_pushboolean(L, ptrResponse->isPending);
	return 1;
}

static int l_response_getStatus(lua_State *L)
{
	DEBUG_PRINT("in l_response_getStatus()");

	const ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);
	const auto& ptrStatus = ptrResponse->status;

	if (ptrResponse->isPending || ptrStatus.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushinteger(L, *ptrStatus);

	return 1;
}

static int l_response_getReason(lua_State *L)
{
	DEBUG_PRINT("in l_response_getReason()");

	const ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);
	const auto& ptrReason = ptrResponse->reason;

	if (ptrResponse->isPending || ptrReason.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushstring(L, ptrReason->c_str());

	return 1;
}

static int l_response_getContent(lua_State *L)
{
	DEBUG_PRINT("in l_response_getContent()");

	const ResponseSharedPtr ptrResponse = l_checkResponse(L, 1);
	const auto& ptrContent = ptrResponse->content;

	if (ptrResponse->isPending || ptrContent.get() == nullptr)
		lua_pushnil(L);
	else
		lua_pushstring(L, ptrContent->c_str());

	return 1;
}

static int l_request(lua_State *L)
{
	DEBUG_PRINT("in l_request()");

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

		// get context

		ContextSharedPtr ptrContext = l_getContext(L); // unused for now

		// start the request and get the response object

		void *buffer = lua_newuserdata(L, sizeof(ResponseSharedPtr));
		new(buffer) ResponseSharedPtr(request.start());
	}
	catch (std::exception& e)
	{
		luaL_error(L, e.what());
	}
	catch (...)
	{
		luaL_error(L, "unknown error");
	}

	luaL_setmetatable(L, responseMetatableName);
	return 1;
}

static const struct luaL_Reg ingamehttpclient_context_meta[] = {
	{ "__gc", l_context_gc },
	{ NULL, NULL }
};

static const struct luaL_Reg ingamehttpclient_response_meta[] = {
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
	luaL_newmetatable(L, contextMetatableName);
	luaL_setfuncs(L, ingamehttpclient_context_meta, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");
	lua_pushcfunction(L, l_newindex_forbidden);
	lua_setfield(L, -2, "__newindex");
	
	luaL_newmetatable(L, responseMetatableName);
	luaL_setfuncs(L, ingamehttpclient_response_meta, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -1, "__index");
	lua_pushcfunction(L, l_newindex_forbidden);
	lua_setfield(L, -2, "__newindex");

	// luaL_newlib(L, ingamehttpclient_lib); // do not use: luaL_checkversion(L) will complain about multiple Lua VMs in <5.3.5, since games' Lua is often linked statically, and this library's Lua is too
	luaL_newlibtable(L, ingamehttpclient_lib);
	luaL_setfuncs(L, ingamehttpclient_lib, 0);
	return 1;
}
