#pragma once

#include "include/cef_v8.h"

namespace app
{

namespace v8
{

// implementation of the V8 handler class for the "app" extension
class AppExtension : public CefV8Handler
{
public:
    AppExtension() {}
    virtual ~AppExtension() {}

    // return true if the method was handled
    virtual bool Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception) override;

    static void init();

private:
    IMPLEMENT_REFCOUNTING(AppExtension);
};

} // namespace v8
} // namespace app
