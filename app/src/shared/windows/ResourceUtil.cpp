#include "shared/util/ResourceUtil.hpp"
#include "shared/AppConfig.hpp"
#include "shared/util/BinaryResourceProvider.hpp"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_byte_read_handler.h"
#include "include/wrapper/cef_stream_resource_handler.h"

#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>

#include <cstring>

namespace shared
{

namespace util
{

bool ResourceUtil::getResourceDir(std::string &dir)
{
    TCHAR buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    CT2CA pszPath(buffer);

    std::string path(pszPath);
    std::string::size_type pos = path.find_last_of("\\/");
    std::string homePath = path.substr(0, pos);
    std::string assetsPath = homePath + "\\" + ASSETS_PATH;

    dir = assetsPath;

    return true;
}

int ResourceUtil::getResourceId(const std::string &resourcePath)
{
    // Implemented for individual executable targets as follows:
    // 1. Add the ID value in: resources/win/resource.h
    // 2. Add the ID to file path mapping in: resources/win/resource.rc
    // 3. Add the resource path to ID mapping in: app/src/shared/windows/ResourceUtil.cpp

    // Map of resource labels to BINARY id values.
    // Example: https://github.com/chromiumembedded/cef/blob/a4d71d8fe2d7422c8e48128f55a503753e2d01c8/tests/cefclient/browser/resource_util_win_idmap.cc
    static struct ResourceMap
    {
        const char *name;
        int id;
    } resourceMap[] = {{}};

    for (size_t i = 0; i < sizeof(resourceMap) / sizeof(ResourceMap); ++i)
    {
        if (!strcmp(resourceMap[i].name, resourcePath.data()))
        {
            return resourceMap[i].id;
        }
    }

    return 0;
}

bool ResourceUtil::loadBinaryResource(int binaryId, DWORD &dwSize, LPBYTE &pBytes)
{
    HINSTANCE hInst = GetModuleHandle(nullptr);
    HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(binaryId), MAKEINTRESOURCE(256));

    if (hRes)
    {
        HGLOBAL hGlob = LoadResource(hInst, hRes);

        if (hGlob)
        {
            dwSize = SizeofResource(hInst, hRes);
            pBytes = (LPBYTE)LockResource(hGlob);

            if (dwSize > 0 && pBytes)
            {
                return true;
            }
        }
    }

    return false;
}

CefResourceManager::Provider *ResourceUtil::createBinaryResourceProvider(const std::string &rootURL)
{
    // only URLs beginning with root URL will be handled by this provider
    return new BinaryResourceProvider(rootURL);
}

bool ResourceUtil::getResourceString(const std::string &resourcePath, std::string &outData)
{
    int resourceId = getResourceId(resourcePath);

    if (resourceId == 0)
    {
        return false;
    }

    DWORD dwSize;
    LPBYTE pBytes;

    if (loadBinaryResource(resourceId, dwSize, pBytes))
    {
        outData = std::string(reinterpret_cast<char *>(pBytes), dwSize);
        return true;
    }

    NOTREACHED(); // the resource should be found

    return false;
}

CefRefPtr<CefStreamReader> ResourceUtil::getResourceReader(const std::string &resourcePath)
{
    int resourceId = ResourceUtil::getResourceId(resourcePath);

    if (resourceId == 0)
    {
        return nullptr;
    }

    DWORD dwSize;
    LPBYTE pBytes;

    if (ResourceUtil::loadBinaryResource(resourceId, dwSize, pBytes))
    {
        return CefStreamReader::CreateForHandler(new CefByteReadHandler(pBytes, dwSize, nullptr));
    }

    NOTREACHED(); // the resource should be found

    return nullptr;
}

} // namespace util
} // namespace shared
