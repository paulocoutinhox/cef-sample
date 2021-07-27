#include "main/AppRenderer.hpp"
#include "include/wrapper/cef_message_router.h"
#include "main/v8/AppExtension.hpp"

namespace app
{

CefRefPtr<CefRenderProcessHandler> RendererApp::GetRenderProcessHandler()
{
    return this;
}

void RendererApp::OnWebKitInitialized()
{
    CefMessageRouterConfig config;
    messageRouter = CefMessageRouterRendererSide::Create(config);

    app::v8::AppExtension::init();
}

void RendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    messageRouter->OnContextCreated(browser, frame, context);
}

void RendererApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    messageRouter->OnContextReleased(browser, frame, context);
}

bool RendererApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    return messageRouter->OnProcessMessageReceived(browser, frame, source_process, message);
}

} // namespace app
