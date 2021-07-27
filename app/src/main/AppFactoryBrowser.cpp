#include "include/cef_app.h"
#include "main/AppBrowser.hpp"
#include "shared/AppFactory.hpp"

namespace shared
{

CefRefPtr<CefApp> AppFactory::createBrowserProcessApp()
{
    return app::BrowserApp::sharedHandler();
}

} // namespace shared
