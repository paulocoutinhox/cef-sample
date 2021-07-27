#include "shared/util/BrowserUtil.hpp"
#include "shared/AppConfig.hpp"
#include "shared/util/BrowserWindowDelegate.hpp"

#include "include/cef_command_line.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

namespace shared
{

namespace util
{

void BrowserUtil::create(CefRefPtr<CefClient> client, const CefString &startupURL, const CefBrowserSettings &settings)
{
    CEF_REQUIRE_UI_THREAD();

#if defined(OS_WIN) || defined(OS_LINUX)
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::GetGlobalCommandLine();

    // Create the browser using the Views framework if "--use-views" is specified via the command-line.
    // Otherwise, create the browser using the native platform framework. The Views framework is currently only supported on Windows and Linux.
    const bool useViews = commandLine->HasSwitch("use-views");
#else
    const bool useViews = false;
#endif

    if (useViews)
    {
        // create the BrowserView
        CefRefPtr<CefBrowserView> browserView = CefBrowserView::CreateBrowserView(client, startupURL, settings, NULL, NULL, NULL);

        // create the window because it will show itself after creation
        CefWindow::CreateTopLevelWindow(new BrowserWindowDelegate(browserView));
    }
    else
    {
        // information used when creating the native window
        CefWindowInfo windowInfo;

#if defined(OS_WIN)
        // on Windows we need to specify certain flags that will be passed to CreateWindowEx()
        windowInfo.SetAsPopup(NULL, APP_NAME);
#endif

        // create the browser window
        CefBrowserHost::CreateBrowser(windowInfo, client, startupURL, settings, NULL, NULL);
    }
}

} // namespace util
} // namespace shared
