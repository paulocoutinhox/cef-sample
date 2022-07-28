#include "main/AppClient.hpp"
#include "main/RequestDumpResourceProvider.hpp"
#include "main/binding/AppBinding.hpp"
#include "shared/AppConfig.hpp"
#include "shared/util/ClientUtil.hpp"
#include "shared/util/ResourceUtil.hpp"

#include <sstream>

#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_stream_resource_handler.h"

namespace app
{

AppClient::AppClient()
{
    resourceManager = new CefResourceManager();
    setupResourceManager(resourceManager);
}

void AppClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title)
{
    // call the default shared implementation
    shared::util::ClientUtil::onTitleChange(browser, title);
}

void AppClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    // call the default shared implementation
    shared::util::ClientUtil::onAfterCreated(browser);

    // create message router
    if (!messageRouter)
    {
        CefMessageRouterConfig config;
        messageRouter = CefMessageRouterBrowserSide::Create(config);

        app::binding::AppBinding::init(messageRouter);
    }
}

bool AppClient::DoClose(CefRefPtr<CefBrowser> browser)
{
    // call the default shared implementation
    return shared::util::ClientUtil::doClose(browser);
}

void AppClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    // call the default shared implementation
    return shared::util::ClientUtil::onBeforeClose(browser);
}

CefRefPtr<CefResourceRequestHandler> AppClient::GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_navigation, bool is_download, const CefString &request_initiator, bool &disable_default_handling)
{
    CEF_REQUIRE_IO_THREAD();
    return this;
}

cef_return_value_t AppClient::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

    return resourceManager->OnBeforeResourceLoad(browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler> AppClient::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

    return resourceManager->GetResourceHandler(browser, frame, request);
}

bool AppClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    return messageRouter->OnProcessMessageReceived(browser, frame, source_process, message);
}

void AppClient::setupResourceManager(CefRefPtr<CefResourceManager> resourceManager)
{
    if (!CefCurrentlyOn(TID_IO))
    {
        // execute on the browser IO thread
        CefPostTask(TID_IO, base::BindOnce(setupResourceManager, resourceManager));
        return;
    }

    // add the provider for dumping request contents
    resourceManager->AddProvider(new app::RequestDumpResourceProvider(APP_ORIGIN + "dump.html"), 0, std::string());

    // add the Provider for bundled resource files
    if (USE_LOCAL_RESOURCES)
    {
        std::string resourceDir;

        if (shared::util::ResourceUtil::getResourceDir(resourceDir))
        {
            resourceManager->AddDirectoryProvider(APP_ORIGIN, resourceDir, 100, std::string());
        }
    }
}

} // namespace app
