#pragma once

#include "include/views/cef_browser_view.h"

namespace shared
{

namespace util
{

class ClientUtil
{

public:
    static void onTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title);
    static void onAfterCreated(CefRefPtr<CefBrowser> browser);
    static bool doClose(CefRefPtr<CefBrowser> browser);
    static void onBeforeClose(CefRefPtr<CefBrowser> browser);
    static void platformTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title);
};

} // namespace util
} // namespace shared
