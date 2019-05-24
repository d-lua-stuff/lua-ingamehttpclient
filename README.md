# In-game Lua HTTP library

A tiny HTTP client library that could be used as a part of the any game that supports Lua 5.3 scripting. It consists of just one DLL file ~~and supports domain whitelisting (TODO)~~.

If you do not need a minimal API surface and the number of files is not an issue, then another library such as [luasocket](https://github.com/diegonehab/luasocket) would most likely be a better choice.

Comments and suggestions about C++ style, Lua API and other topics are welcome! This script's author's brain was rotted by years of C#...

See [test.lua](./test.lua) for some examples.

# Building

## On Windows

Run `build-lib.cmd` to build x64 [POCO](https://github.com/pocoproject/poco) and Lua 5.3.4. The `VS140COMNTOOLS` environment variable must be defined. Then build `ingamehttpclient.sln` in Visual Studio.

## On Linux or Mac

Not supported at the moment, but it would require minimal code changes. NetSSL_OpenSSL would be used instead of NetSSL_Win.

# Debugging

Debug build uses `OutputDebugString` for logging. This output can be viewed with [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview).

It is recommended to set project's debugging settings as follows:

![Configuration Properties | Debugging](doc/debug_settings.png)

This will run a test script and leave an interactive Lua prompt open.
