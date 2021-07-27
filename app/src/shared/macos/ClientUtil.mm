#include "shared/util/ClientUtil.hpp"

#import <Cocoa/Cocoa.h>

#include "include/cef_browser.h"

namespace shared
{

namespace util
{

void ClientUtil::platformTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title)
{
    NSView *view = (NSView *)browser->GetHost()->GetWindowHandle();
    NSWindow *window = [view window];
    std::string titleStr(title);
    NSString *str = [NSString stringWithUTF8String:titleStr.c_str()];
    [window setTitle:str];
}

} // namespace util
} // namespace shared
