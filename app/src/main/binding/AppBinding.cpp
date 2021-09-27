#include "main/binding/AppBinding.hpp"
#include "main/net/RequestClient.hpp"
#include "shared/AppConfig.hpp"

#include <algorithm>

namespace app
{
namespace binding
{

bool AppBinding::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback)
{
    // only handle messages from application url
    const std::string &url = frame->GetURL();

    if (!(url.rfind(APP_ORIGIN, 0) == 0))
    {
        return false;
    }

    {
        const std::string &messageName = "App::ReverseData";
        const std::string &requestMessage = request;

        if (requestMessage.rfind(messageName, 0) == 0)
        {
            return onTaskReverseData(browser, frame, queryId, request, persistent, callback, messageName, requestMessage);
        }
    }

    {
        const std::string &messageName = "App::NetworkRequest";
        const std::string &requestMessage = request;

        if (requestMessage.rfind(messageName, 0) == 0)
        {
            return onTaskNetworkRequest(browser, frame, queryId, request, persistent, callback, messageName, requestMessage);
        }
    }

    return false;
}

bool AppBinding::onTaskNetworkRequest(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback, const std::string &messageName, const std::string &requestMessage)
{
    // create a CefRequest object
    CefRefPtr<CefRequest> httpRequest = CefRequest::Create();

    // set the request URL
    httpRequest->SetURL(requestMessage.substr(messageName.size() + 1));

    // set the request method (supported methods include GET, POST, HEAD, DELETE and PUT)
    httpRequest->SetMethod("POST");

    // optionally specify custom headers
    CefRequest::HeaderMap headerMap;
    headerMap.insert(std::make_pair("X-My-Header", "My Header Value"));
    httpRequest->SetHeaderMap(headerMap);

    // 1. optionally specify upload content
    // 2. the default "Content-Type" header value is "application/x-www-form-urlencoded"
    // 3. set "Content-Type" via the HeaderMap if a different value is desired
    const std::string &uploadData = "arg1=val1&arg2=val2";

    CefRefPtr<CefPostData> postData = CefPostData::Create();
    CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();

    element->SetToBytes(uploadData.size(), uploadData.c_str());
    postData->AddElement(element);
    httpRequest->SetPostData(postData);

    // optionally set flags
    httpRequest->SetFlags(UR_FLAG_SKIP_CACHE);

    // create client and load
    const app::net::RequestClient::RequestCallback &requestCallback = base::Bind(&AppBinding::onRequestComplete, base::Unretained(this));
    CefRefPtr<CefURLRequest> urlrequest = frame->CreateURLRequest(httpRequest, new app::net::RequestClient(callback, requestCallback));

    return true;
}

bool AppBinding::onTaskReverseData(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback, const std::string &messageName, const std::string &requestMessage)
{
    std::string result = requestMessage.substr(messageName.size() + 1);
    std::reverse(result.begin(), result.end());
    callback->Success(result);

    return true;
}

void AppBinding::onRequestComplete(CefRefPtr<Callback> callback, CefURLRequest::ErrorCode errorCode, const std::string &downloadData)
{
    CEF_REQUIRE_UI_THREAD();

    if (errorCode == ERR_NONE)
    {
        callback->Success(downloadData);
    }
    else
    {
        callback->Failure(errorCode, "Request failed!");
    }

    callback = nullptr;
}

void AppBinding::init(CefRefPtr<CefMessageRouterBrowserSide> router)
{
    router->AddHandler(new AppBinding(), false);
}

} // namespace binding
} // namespace app
