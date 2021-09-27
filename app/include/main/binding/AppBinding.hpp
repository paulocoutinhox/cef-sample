#pragma once

#include <string>

#include "include/cef_urlrequest.h"
#include "include/wrapper/cef_message_router.h"

namespace app
{
namespace binding
{

// handle messages in the browser process
class AppBinding : public CefMessageRouterBrowserSide::Handler
{
public:
    AppBinding() {}
    virtual ~AppBinding() {}

    // called due to cefQuery execution
    virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback) override;

    // called on AppClient
    static void init(CefRefPtr<CefMessageRouterBrowserSide> router);

    // tasks
    bool onTaskReverseData(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback, const std::string &messageName, const std::string &requestMessage);
    bool onTaskNetworkRequest(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 queryId, const CefString &request, bool persistent, CefRefPtr<Callback> callback, const std::string &messageName, const std::string &requestMessage);

    // callback
    void onRequestComplete(CefRefPtr<Callback> callback, CefURLRequest::ErrorCode errorCode, const std::string &downloadData);

private:
    DISALLOW_COPY_AND_ASSIGN(AppBinding);
};

} // namespace binding
} // namespace app
