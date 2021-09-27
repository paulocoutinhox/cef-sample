#pragma once

#include <string>

#include "include/base/cef_bind.h"
#include "include/base/cef_callback.h"
#include "include/base/cef_logging.h"
#include "include/cef_urlrequest.h"

#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

namespace app
{

namespace net
{

class RequestClient : public CefURLRequestClient
{

public:
    typedef base::Callback<void(CefRefPtr<CefMessageRouterBrowserSide::Callback> /* browserCallback */, CefURLRequest::ErrorCode /*errorCode*/, const std::string & /*downloadData*/)> RequestCallback;

    RequestClient(CefRefPtr<CefMessageRouterBrowserSide::Callback> browserCallback, const RequestCallback &requestCallback);
    virtual ~RequestClient();

    void OnRequestComplete(CefRefPtr<CefURLRequest> request) override;
    void OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override;
    void OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override;
    void OnDownloadData(CefRefPtr<CefURLRequest> request, const void *data, size_t dataLength) override;
    bool GetAuthCredentials(bool isProxy, const CefString &host, int port, const CefString &realm, const CefString &scheme, CefRefPtr<CefAuthCallback> callback) override;

    void detach();
    const std::string &getDownloadData();

private:
    int64 uploadTotal;
    int64 downloadTotal;
    std::string downloadData;

    RequestCallback requestCallback;
    CefRefPtr<CefMessageRouterBrowserSide::Callback> browserCallback;

    IMPLEMENT_REFCOUNTING(RequestClient);
    DISALLOW_COPY_AND_ASSIGN(RequestClient);
};

} // namespace net
} // namespace app
