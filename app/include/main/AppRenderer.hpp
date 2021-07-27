#pragma once

#include "include/cef_app.h"
#include "include/wrapper/cef_message_router.h"

namespace app
{

class RendererApp : public CefApp, public CefRenderProcessHandler
{
public:
    RendererApp() {}

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    void OnWebKitInitialized() override;
    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

private:
    CefRefPtr<CefMessageRouterRendererSide> messageRouter;

    IMPLEMENT_REFCOUNTING(RendererApp);
    DISALLOW_COPY_AND_ASSIGN(RendererApp);
};

} // namespace app
