#include "shared/util/ResourceUtil.hpp"
#include "shared/AppConfig.hpp"

#include "include/cef_parser.h"
#include "include/wrapper/cef_stream_resource_handler.h"

namespace shared
{

namespace util
{

std::string ResourceUtil::getUrlWithoutQueryOrFragment(const std::string &url)
{
    // find the first instance of '?' or '#'
    const size_t pos = std::min(url.find('?'), url.find('#'));

    if (pos != std::string::npos)
    {
        return url.substr(0, pos);
    }

    return url;
}

std::string ResourceUtil::getResourcePath(const std::string &url)
{
    // Removes fragment and/or query component if it exists.
    // The URL must start with app origin.
    if (url.find(APP_ORIGIN) != 0U)
    {
        return std::string();
    }

    const std::string &urlNoQuery = getUrlWithoutQueryOrFragment(url);
    return urlNoQuery.substr(sizeof(APP_ORIGIN) - 1);
}

std::string ResourceUtil::getMimeType(const std::string &resourcePath)
{
    std::string mimeType;
    size_t sep = resourcePath.find_last_of(".");

    if (sep != std::string::npos)
    {
        mimeType = CefGetMimeType(resourcePath.substr(sep + 1));

        if (!mimeType.empty())
        {
            return mimeType;
        }
    }

    return "text/html";
}

CefRefPtr<CefResourceHandler> ResourceUtil::getResourceHandler(const std::string &resourcePath)
{
    CefRefPtr<CefStreamReader> reader = getResourceReader(resourcePath);

    if (!reader)
    {
        return NULL;
    }

    return new CefStreamResourceHandler(getMimeType(resourcePath), reader);
}

} // namespace util
} // namespace shared
