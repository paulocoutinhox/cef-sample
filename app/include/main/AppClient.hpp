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

    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title) override;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString &request_initiator, bool &disable_default_handling) override;

    cef_return_value_t OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override;

    CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

private:
    CefRefPtr<CefResourceManager> resourceManager;
    CefRefPtr<CefMessageRouterBrowserSide> messageRouter;

    static void setupResourceManager(CefRefPtr<CefResourceManager> resourceManager);

    IMPLEMENT_REFCOUNTING(AppClient);
    DISALLOW_COPY_AND_ASSIGN(AppClient);
};

} // namespace app
