#include "shared/util/BrowserWindowDelegate.hpp"

namespace shared
{

namespace util
{

void BrowserWindowDelegate::OnWindowCreated(CefRefPtr<CefWindow> window)
{
    // add the browser view and show the window
    window->AddChildView(browserView);
    window->Show();

    // give keyboard focus to the browser view
    browserView->RequestFocus();
}

void BrowserWindowDelegate::OnWindowDestroyed(CefRefPtr<CefWindow> window)
{
    browserView = NULL;
}

bool BrowserWindowDelegate::CanClose(CefRefPtr<CefWindow> window)
{
    // allow the window to close if the browser says it's OK
    CefRefPtr<CefBrowser> browser = browserView->GetBrowser();

    if (browser)
    {
        return browser->GetHost()->TryCloseBrowser();
    }

    return true;
}

CefSize BrowserWindowDelegate::GetPreferredSize(CefRefPtr<CefView> view)
{
    // preferred window size
    return CefSize(800, 600);
}

CefSize BrowserWindowDelegate::GetMinimumSize(CefRefPtr<CefView> view)
{
    // minimum window size
    return CefSize(200, 100);
}

} // namespace util
} // namespace shared
