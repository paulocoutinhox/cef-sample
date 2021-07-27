#pragma once

#include <list>

#include "include/base/cef_thread_checker.h"
#include "include/cef_browser.h"

namespace shared
{

// manages multiple CefBrowser instances and all methods must be called on the main application thread (browser process UI thread)
class ClientManager
{
public:
    ClientManager();
    ~ClientManager();

    // returns the singleton instance of this object
    static ClientManager *getInstance();

    // called from CefLifeSpanHandler methods
    void onAfterCreated(CefRefPtr<CefBrowser> browser);
    void doClose(CefRefPtr<CefBrowser> browser);
    void onBeforeClose(CefRefPtr<CefBrowser> browser);

    // request that all existing browser windows close
    void closeAllBrowsers(bool forceClose);

    // returns true if the last browser instance is closing
    bool isClosing() const;

private:
    static ClientManager *instance;
    base::ThreadChecker threadChecker;
    bool isClosingInternal;

    // list of existing browsers
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList browserList;
};

} // namespace shared
