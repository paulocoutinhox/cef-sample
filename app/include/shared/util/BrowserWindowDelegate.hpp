#pragma once

#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"

namespace shared
{

namespace util
{

// when using the Views framework this object provides the delegate implementation for the CefWindow that hosts the views-based browser
class BrowserWindowDelegate : public CefWindowDelegate
{

public:
    explicit BrowserWindowDelegate(CefRefPtr<CefBrowserView> browserView) : browserView(browserView) {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE;
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE;
    bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE;
    CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE;
    CefSize GetMinimumSize(CefRefPtr<CefView> view) OVERRIDE;

private:
    CefRefPtr<CefBrowserView> browserView;

    IMPLEMENT_REFCOUNTING(BrowserWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(BrowserWindowDelegate);
};

} // namespace util
} // namespace shared
