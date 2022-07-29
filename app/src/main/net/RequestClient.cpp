#include "main/net/RequestClient.hpp"
#include <iostream>

namespace app
{

namespace net
{

RequestClient::RequestClient(CefRefPtr<CefMessageRouterBrowserSide::Callback> browserCallback, RequestCallback requestCallback)
{
    CEF_REQUIRE_UI_THREAD();

    this->browserCallback = browserCallback;
    this->requestCallback = std::move(requestCallback);

    DCHECK(this->browserCallback);
    DCHECK(!this->requestCallback.is_null());
}

RequestClient::~RequestClient()
{
    detach();
}

void RequestClient::detach()
{
    CEF_REQUIRE_UI_THREAD();

    browserCallback = nullptr;

    if (!this->requestCallback.is_null())
    {
        this->requestCallback.Reset();
    }
}

void RequestClient::OnRequestComplete(CefRefPtr<CefURLRequest> request)
{
    /*
    CefURLRequest::Status status = request->GetRequestStatus();
    CefURLRequest::ErrorCode errorCode = request->GetRequestError();
    CefRefPtr<CefResponse> response = request->GetResponse();
    */

    CefURLRequest::ErrorCode errorCode = request->GetRequestError();

    CEF_REQUIRE_UI_THREAD();

    if (!requestCallback.is_null())
    {
        std::move(requestCallback).Run(browserCallback, errorCode, downloadData);
        std::move(requestCallback).Reset();
    }
}

void RequestClient::OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total)
{
    uploadTotal = total;
}

void RequestClient::OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total)
{
    downloadTotal = total;
}

void RequestClient::OnDownloadData(CefRefPtr<CefURLRequest> request, const void *data, size_t dataLength)
{
    CEF_REQUIRE_UI_THREAD();
    downloadData += std::string(static_cast<const char *>(data), dataLength);
}

bool RequestClient::GetAuthCredentials(bool isProxy, const CefString &host, int port, const CefString &realm, const CefString &scheme, CefRefPtr<CefAuthCallback> callback)
{
    return false; // not handled
}

const std::string &app::net::RequestClient::getDownloadData()
{
    return downloadData;
}

} // namespace net
} // namespace app
