#pragma once

#include "include/cef_app.h"
#include "include/cef_client.h"

#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"

namespace app
{

class AppClient : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefRequestHandler, public CefResourceRequestHandler
{
public:
    AppClient();

    CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }

    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title) OVERRIDE;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString &request_initiator, bool &disable_default_handling) OVERRIDE;

    cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback) OVERRIDE;

    CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) OVERRIDE;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:
    CefRefPtr<CefResourceManager> resourceManager;
    CefRefPtr<CefMessageRouterBrowserSide> messageRouter;

    static void setupResourceManager(CefRefPtr<CefResourceManager> resourceManager);

    IMPLEMENT_REFCOUNTING(AppClient);
    DISALLOW_COPY_AND_ASSIGN(AppClient);
};

} // namespace app
