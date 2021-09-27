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

    void OnWindowCreated(CefRefPtr<CefWindow> window) override;
    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
    bool CanClose(CefRefPtr<CefWindow> window) override;
    CefSize GetPreferredSize(CefRefPtr<CefView> view) override;
    CefSize GetMinimumSize(CefRefPtr<CefView> view) override;

private:
    CefRefPtr<CefBrowserView> browserView;

    IMPLEMENT_REFCOUNTING(BrowserWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(BrowserWindowDelegate);
};

} // namespace util
} // namespace shared
