#include "stdafx.h"
#include "Response.h"

using Poco::Exception;
using Poco::SharedPtr;
using Poco::URI;
using Poco::Net::Context;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

void initRequest()
{
	Poco::Net::initializeSSL();
}

void performRequest(const char *uriStr, std::shared_ptr<Response> ptrResponse)
{
	DEBUG_PRINT("ingamehttpclient: request thread started");

	try
	{
		SharedPtr<HTTPClientSession> ptrSession;

		const URI uri(uriStr);

		if (uri.getScheme() == "https")
		{
			Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "");
			ptrSession = new HTTPSClientSession(uri.getHost(), uri.getPort(), ptrContext);
		}
		else
		{
			ptrSession = new HTTPClientSession(uri.getHost(), uri.getPort());
		}

		HTTPRequest request("GET", uri.getPath(), HTTPMessage::HTTP_1_1);
		HTTPResponse response;

		DEBUG_PRINT("ingamehttpclient: sending the request");

		ptrSession->sendRequest(request);
		std::istream& responseStream = ptrSession->receiveResponse(response);

		DEBUG_PRINT("ingamehttpclient: receiving the response");

		ptrResponse->status = std::make_shared<HTTPResponse::HTTPStatus>(response.getStatus());
		ptrResponse->reason = std::make_shared<std::string>(response.getReason());

		// TODO: limit max response content size
		std::string responseContent(std::istreambuf_iterator<char>(responseStream), {});
		ptrResponse->content = std::make_shared<std::string>(responseContent);
	}
	catch (Poco::Exception& e)
	{
		ptrResponse->reason = std::make_shared<std::string>(e.message());
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

	DEBUG_PRINT("ingamehttpclient: request thread finished");
}

std::shared_ptr<Response> startRequest(const char *uriStr)
{
	std::shared_ptr<Response> ptrResponse = std::make_shared<Response>();

	std::thread requestThread = std::thread(performRequest, uriStr, ptrResponse);
	requestThread.detach();

	return ptrResponse;;
}


