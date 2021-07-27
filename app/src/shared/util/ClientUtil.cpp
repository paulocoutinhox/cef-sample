#include "shared/util/ClientUtil.hpp"
#include "shared/ClientManager.hpp"

#include <sstream>
#include <string>

#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

namespace shared
{

namespace util
{

void ClientUtil::onTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title)
{
    CEF_REQUIRE_UI_THREAD();

#if defined(OS_WIN) || defined(OS_LINUX)
    // the views framework is currently only supported on Windows and Linux
    CefRefPtr<CefBrowserView> browserView = CefBrowserView::GetForBrowser(browser);

    if (browserView)
    {
        // set the title of the window using the views framework
        CefRefPtr<CefWindow> window = browserView->GetWindow();

        if (window)
        {
            window->SetTitle(title);
        }
    }
    else
#endif
    {
        // set the title of the window using platform APIs
        platformTitleChange(browser, title);
    }
}

void ClientUtil::onAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // add to the list of existing browsers
    ClientManager::getInstance()->onAfterCreated(browser);
}

bool ClientUtil::doClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling.
    // See the DoClose() documentation in the CEF header for a detailed destription of this process.
    ClientManager::getInstance()->doClose(browser);

    // Allow the close.
    // For windowed browsers this will result in the OS close event being sent.
    return false;
}

void ClientUtil::onBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // remove from the list of existing browsers
    ClientManager::getInstance()->onBeforeClose(browser);
}

} // namespace util
} // namespace shared
