#include "include/cef_app.h"
#include "main/AppRenderer.hpp"
#include "shared/AppFactory.hpp"

namespace shared
{

CefRefPtr<CefApp> AppFactory::createRendererProcessApp()
{
    return new app::RendererApp();
}

} // namespace shared
