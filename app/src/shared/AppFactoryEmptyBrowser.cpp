#include "shared/AppFactory.hpp"

namespace shared
{

CefRefPtr<CefApp> AppFactory::createBrowserProcessApp()
{
    return nullptr;
}

} // namespace shared