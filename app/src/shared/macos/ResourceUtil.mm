#include "shared/util/ResourceUtil.hpp"
#include "shared/AppConfig.hpp"

#import <Foundation/Foundation.h>

#include <cstdio>
#include <mach-o/dyld.h>

#include "include/base/cef_logging.h"

namespace shared
{

namespace util
{

bool ResourceUtil::uncachedAmIBundled()
{
    return [[[NSBundle mainBundle] bundlePath] hasSuffix:@".app"];
}

bool ResourceUtil::amIBundled()
{
    static bool result = uncachedAmIBundled();
    return result;
}

bool ResourceUtil::getResourceDir(std::string &dir)
{
    // retrieve the executable directory
    uint32_t pathSize = 0;
    _NSGetExecutablePath(nullptr, &pathSize);

    if (pathSize > 0)
    {
        dir.resize(pathSize);
        _NSGetExecutablePath(const_cast<char *>(dir.c_str()), &pathSize);
    }

    if (amIBundled())
    {
        // trim executable name up to the last separator
        std::string::size_type lastSeparator = dir.find_last_of("/");
        dir.resize(lastSeparator);
        dir.append("/../Resources/" + ASSETS_PATH);

        return true;
    }

    dir.append("/Resources/" + ASSETS_PATH);

    return true;
}

} // namespace util
} // namespace shared
