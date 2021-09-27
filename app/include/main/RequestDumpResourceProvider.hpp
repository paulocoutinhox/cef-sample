#pragma once

#include "include/wrapper/cef_resource_manager.h"
#include "shared/util/ResourceUtil.hpp"

namespace app
{

// demonstrate a custom provider implementation by dumping the request contents
class RequestDumpResourceProvider : public CefResourceManager::Provider
{
public:
    RequestDumpResourceProvider(const std::string &url);

    bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override;
    std::string dumpRequestContents(CefRefPtr<CefRequest> request);

private:
    std::string url;

    DISALLOW_COPY_AND_ASSIGN(RequestDumpResourceProvider);
};

} // namespace app
