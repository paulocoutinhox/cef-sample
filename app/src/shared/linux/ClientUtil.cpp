#include "shared/util/ClientUtil.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <string>

#include "include/base/cef_logging.h"
#include "include/cef_browser.h"

namespace shared
{

namespace util
{

void ClientUtil::platformTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title)
{
    std::string titleStr(title);

    // retrieve the X11 display shared with Chromium
    ::Display *display = cef_get_xdisplay();
    DCHECK(display);

    // retrieve the X11 window handle for the browser
    ::Window window = browser->GetHost()->GetWindowHandle();
    DCHECK(window != kNullWindowHandle);

    // retrieve the atoms required by the below XChangeProperty call
    const char *kAtoms[] = {"_NET_WM_NAME", "UTF8_STRING"};
    Atom atoms[2];

    int result = XInternAtoms(display, const_cast<char **>(kAtoms), 2, false, atoms);

    if (!result)
    {
        NOTREACHED();
    }

    // set the window title
    XChangeProperty(display, window, atoms[0], atoms[1], 8, PropModeReplace, reinterpret_cast<const unsigned char *>(titleStr.c_str()), titleStr.size());

    // TODO(erg): This is technically wrong.
    // So XStoreName and friends expect this in Host Portable Character Encoding instead of UTF-8, which I believe is Compound Text.
    // This shouldn't matter 90% of the time since this is the fallback to the UTF8 property above.
    XStoreName(display, browser->GetHost()->GetWindowHandle(), titleStr.c_str());
}

} // namespace util
} // namespace shared
