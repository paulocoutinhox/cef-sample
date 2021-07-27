#pragma once

#include "include/cef_client.h"
#include "include/views/cef_browser_view.h"

namespace shared
{

namespace util
{

class BrowserUtil
{

public:
    static void create(CefRefPtr<CefClient> client, const CefString &startupURL, const CefBrowserSettings &settings);
};

} // namespace util
} // namespace shared
