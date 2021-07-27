#include "shared/ClientManager.hpp"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace shared
{

ClientManager *ClientManager::instance = NULL;

ClientManager::ClientManager() : isClosingInternal(false)
{
    instance = this;
}

ClientManager::~ClientManager()
{
    DCHECK(threadChecker.CalledOnValidThread());
    DCHECK(browserList.empty());

    instance = NULL;
}

ClientManager *ClientManager::getInstance()
{
    CEF_REQUIRE_UI_THREAD();
    DCHECK(instance);

    return instance;
}

void ClientManager::onAfterCreated(CefRefPtr<CefBrowser> browser)
{
    DCHECK(threadChecker.CalledOnValidThread());

    // add to the list of existing browsers
    browserList.push_back(browser);
}

void ClientManager::doClose(CefRefPtr<CefBrowser> browser)
{
    DCHECK(threadChecker.CalledOnValidThread());

    if (browserList.size() == 1U)
    {
        // the last browser window is closing
        isClosingInternal = true;
    }
}

void ClientManager::onBeforeClose(CefRefPtr<CefBrowser> browser)
{
    DCHECK(threadChecker.CalledOnValidThread());

    // remove from the list of existing browsers
    BrowserList::iterator bit = browserList.begin();

    for (; bit != browserList.end(); ++bit)
    {
        if ((*bit)->IsSame(browser))
        {
            browserList.erase(bit);
            break;
        }
    }

    if (browserList.empty())
    {
        // all browser windows have closed, so quit the application message loop
        CefQuitMessageLoop();
    }
}

void ClientManager::closeAllBrowsers(bool forceClose)
{
    DCHECK(threadChecker.CalledOnValidThread());

    if (browserList.empty())
    {
        return;
    }

    BrowserList::const_iterator it = browserList.begin();

    for (; it != browserList.end(); ++it)
    {
        (*it)->GetHost()->CloseBrowser(forceClose);
    }
}

bool ClientManager::isClosing() const
{
    DCHECK(threadChecker.CalledOnValidThread());
    return isClosingInternal;
}

} // namespace shared
