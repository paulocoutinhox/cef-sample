#include "shared/util/ClientUtil.hpp"

#include <string>
#include <windows.h>

#include "include/cef_browser.h"

namespace shared
{

namespace util
{

void ClientUtil::platformTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title)
{
    CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
    SetWindowText(hwnd, std::wstring(title).c_str());
}

} // namespace util
} // namespace shared
