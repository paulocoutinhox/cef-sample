#include "shared/util/ResourceUtil.hpp"

#include <cstdio>

namespace shared
{

namespace util
{

bool ResourceUtil::fileExists(const char *path)
{
    FILE *f = fopen(path, "rb");

    if (f)
    {
        fclose(f);
        return true;
    }

    return false;
}

bool ResourceUtil::readFileToString(const char *path, std::string &data)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        return false;
    }

    char buf[1 << 16];
    size_t len;

    while ((len = fread(buf, 1, sizeof(buf), file)) > 0)
    {
        data.append(buf, len);
    }

    fclose(file);

    return true;
}

bool ResourceUtil::getResourceString(const std::string &resourcePath, std::string &outData)
{
    std::string path;

    if (!getResourceDir(path))
    {
        return false;
    }

    path.append("/");
    path.append(resourcePath);

    return readFileToString(path.c_str(), outData);
}

CefRefPtr<CefStreamReader> ResourceUtil::getResourceReader(const std::string &resourcePath)
{
    std::string path;

    if (!getResourceDir(path))
    {
        return nullptr;
    }

    path.append("/");
    path.append(resourcePath);

    if (!fileExists(path.c_str()))
    {
        return nullptr;
    }

    return CefStreamReader::CreateForFile(path);
}

} // namespace util
} // namespace shared
