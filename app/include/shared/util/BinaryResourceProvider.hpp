#pragma once

#include "include/wrapper/cef_byte_read_handler.h"
#include "include/wrapper/cef_resource_manager.h"
#include "include/wrapper/cef_stream_resource_handler.h"

namespace shared
{

namespace util
{

class BinaryResourceProvider : public CefResourceManager::Provider
{
public:
    explicit BinaryResourceProvider(const std::string &rootURL) : rootURL(rootURL)
    {
        DCHECK(!rootURL.empty());
    }

    bool OnRequest(scoped_refptr<CefResourceManager::Request> request) override;

private:
    std::string rootURL;

    DISALLOW_COPY_AND_ASSIGN(BinaryResourceProvider);
};

} // namespace util
} // namespace shared
