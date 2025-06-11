#include "shared/util/ResourceUtil.hpp"
#include "shared/AppConfig.hpp"

#include <libgen.h>
#include <linux/limits.h>
#include <string>
#include <unistd.h>

namespace shared
{

namespace util
{

bool ResourceUtil::getResourceDir(std::string &dir)
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);

    if (count != -1)
    {
        result[count] = '\0';
        const char *path = dirname(result);
        dir = std::string(path) + "/" + ASSETS_PATH;
        return true;
    }
    else
    {
        return false;
    }
}

} // namespace util
} // namespace shared
