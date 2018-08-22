local function printResponse (response)
	print("isPending: ", response:isPending())
	print("status: ", response:getStatus())
	print("reason: ", response:getReason())
	print("content: ", response:getContent())
end

local function doRequest (requestData)
	print("request begins...")

	local response = http.request(requestData)

	while response:isPending() do -- a real Lua script could call coroutine.yield() here
		print("\nwaiting...\n")
		printResponse(response)
		os.execute("timeout 1 > nul")
	end

	print("\ndone!\n")
	printResponse(response)
end

local ok, message = pcall(function ()
	http = require "ingamehttpclient"

	doRequest {
		url = "https://reqres.in/api/users/2"
	}

	doRequest {
		url = "https://reqres.in/api/users",
		method = "POST",
		body = '{ "name": "John Doe", "job": "Lua scripter" }',
		headers = {
			["Content-Type"] = "application/json"
		}
	}
end)

if not ok then
	print("error: " .. message)
end
