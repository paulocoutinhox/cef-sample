#include "main/v8/AppExtension.hpp"

#include <chrono>
#include <ctime>
#include <iostream>

namespace app
{

namespace v8
{

bool AppExtension::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception)
{
    if (name == "getStuffMessage")
    {
        retval = CefV8Value::CreateString("A stuff message from C++");
        return true;
    }
    else if (name == "getCurrentTime")
    {
        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char buf[100] = {0};
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %r", std::localtime(&now));
        std::string value = std::string(buf);
        retval = CefV8Value::CreateString(value.c_str());
        return true;
    }

    return false;
}

void AppExtension::init()
{
    // register a V8 extension with the below JavaScript code that calls native methods implemented in the handler
    std::string code =
        ""
        "var MyApp;"
        "if (!MyApp) {"
        "  MyApp = {};"
        "}"
        ""
        "(function() {"
        "  MyApp.getStuffMessage = function() {"
        "    native function getStuffMessage();"
        "    return getStuffMessage();"
        "  };"
        "  MyApp.getCurrentTime = function() {"
        "    native function getCurrentTime();"
        "    return getCurrentTime();"
        "  };"
        ""
        "})();";

    CefRegisterExtension("v8/app", code, new app::v8::AppExtension());
}

} // namespace v8
} // namespace app
