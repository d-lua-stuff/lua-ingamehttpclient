local function printResponse (response)
	print("isPending: ", response:isPending())
	print("status: ", response:getStatus())
	print("reason: ", response:getReason())
	print("content: ", response:getContent())
end

local ok, message = pcall(function ()
	http = require "ingamehttpclient"

	local response = http.request("https://jsonplaceholder.typicode.com/posts/1")
	while response:isPending() do
		print("waiting...")
		printResponse(response)
		os.execute("timeout 1 > nul")
	end

	print("done!")
	printResponse(response)
end)

if not ok then
	print("error: " .. message)
end

