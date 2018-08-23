#include "stdafx.h"
#include "Request.h"

using Poco::Exception;
using Poco::SharedPtr;
using Poco::URI;
using Poco::Net::Context;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

static void performRequest(
	std::shared_ptr<Response> ptrResponse,
	const std::string url,
	const std::string method,
	const std::string body,
	const std::map<std::string, std::string> headers
)
{
	DEBUG_PRINT("request thread started");

	try
	{
		SharedPtr<HTTPClientSession> ptrSession;

		const URI uri(url);

		if (uri.getScheme() == "https")
		{
			Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "");
			ptrSession = new HTTPSClientSession(uri.getHost(), uri.getPort(), ptrContext);
		}
		else if (uri.getScheme() == "http")
		{
			ptrSession = new HTTPClientSession(uri.getHost(), uri.getPort());
		}
		else
		{
			throw std::invalid_argument("URL scheme must be http or https");
		}

		HTTPRequest request(method, uri.getPath(), HTTPMessage::HTTP_1_1);
		HTTPResponse response;

		if (body.length() > 0)
		{
			request.setContentLength64(body.length());
		}

		for (auto& kv : headers)
		{
			request.set(kv.first, kv.second);
		}

		DEBUG_PRINT("sending the request");

		std::ostream& requestStream = ptrSession->sendRequest(request);

		if (body.length() > 0)
		{
			requestStream << body;
		}

		DEBUG_PRINT("receiving the response");

		std::istream& responseStream = ptrSession->receiveResponse(response);

		ptrResponse->status = std::make_shared<HTTPResponse::HTTPStatus>(response.getStatus());
		ptrResponse->reason = std::make_shared<std::string>(response.getReason());

		// TODO: limit max response content size
		std::string responseContent(std::istreambuf_iterator<char>(responseStream), {});
		ptrResponse->content = std::make_shared<std::string>(responseContent);
	}
	catch (Poco::Exception& e)
	{
		ptrResponse->reason = std::make_shared<std::string>(e.displayText());
	}
	catch (std::exception& e)
	{
		ptrResponse->reason = std::make_shared<std::string>(e.what());
	}
	catch (...)
	{
		ptrResponse->reason = std::make_shared<std::string>("unknown error");
	}

	ptrResponse->isPending = false;

	DEBUG_PRINT("request thread finished");
}

std::shared_ptr<Response> Request::start() const
{
	std::shared_ptr<Response> ptrResponse = std::make_shared<Response>();

	std::thread requestThread = std::thread(performRequest, ptrResponse,
		url, method, body, headers);

	requestThread.detach();

	return ptrResponse;;
}


