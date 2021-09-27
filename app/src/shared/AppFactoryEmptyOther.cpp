#include "shared/AppFactory.hpp"

namespace shared
{

CefRefPtr<CefApp> AppFactory::createOtherProcessApp()
{
    return nullptr;
}

} // namespace shared
