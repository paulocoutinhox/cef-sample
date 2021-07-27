#include "shared/util/BinaryResourceProvider.hpp"
#include "shared/AppConfig.hpp"
#include "shared/util/ResourceUtil.hpp"

#include "include/base/cef_logging.h"

#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>

#include <cstring>

namespace shared
{

namespace util
{

bool BinaryResourceProvider::OnRequest(scoped_refptr<CefResourceManager::Request> request)
{
    CEF_REQUIRE_IO_THREAD();

    const std::string &url = request->url();

    if (url.find(rootURL) != 0L)
    {
        // not handled by this provider
        return false;
    }

    CefRefPtr<CefResourceHandler> handler;

    const std::string &relativePath = url.substr(rootURL.length());

    if (!relativePath.empty())
    {
        CefRefPtr<CefStreamReader> stream = shared::util::ResourceUtil::getResourceReader(relativePath.data());

        if (stream.get())
        {
            handler = new CefStreamResourceHandler(request->mime_type_resolver().Run(url), stream);
        }
    }

    request->Continue(handler);

    return true;
}

} // namespace util
} // namespace shared
