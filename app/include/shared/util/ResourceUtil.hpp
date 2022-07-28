#pragma once

#include "include/cef_parser.h"
#include "include/wrapper/cef_stream_resource_handler.h"

#if defined(OS_WIN)
#include "include/wrapper/cef_resource_manager.h"
#endif

namespace shared
{

namespace util
{

class ResourceUtil
{

public:
    // returns URL without the query or fragment components, if any
    static std::string getUrlWithoutQueryOrFragment(const std::string &url);

    // return the path where resources are stored
    static std::string getResourcePath(const std::string &url);

    // determine the mime type based on the file path file extension
    static std::string getMimeType(const std::string &resourcePath);

    // return the resource handler by mime type
    static CefRefPtr<CefResourceHandler> getResourceHandler(const std::string &resourcePath);

    // return resource path
    static bool getResourceDir(std::string &dir);

    // retrieve resource path contents as a std::string or false if the resource is not found
    static bool getResourceString(const std::string &resourcePath, std::string &outData);

    // retrieve resource path contents as a CefStreamReader or returns null if the resource is not found
    static CefRefPtr<CefStreamReader> getResourceReader(const std::string &resourcePath);

#if defined(OS_MAC)
    static bool uncachedAmIBundled();

    static bool amIBundled();
#endif

#if defined(OS_POSIX)
    // determine if file exists
    static bool fileExists(const char *path);

    // read content of file as string
    static bool readFileToString(const char *path, std::string &data);
#endif

#if defined(OS_WIN)
    // returns the BINARY id value associated with resource path on Windows
    static int getResourceId(const std::string &resourcePath);

    // create a new provider for loading BINARY resources on Windows
    static CefResourceManager::Provider *createBinaryResourceProvider(const std::string &rootURL);

    // load resource by binary ID
    static bool loadBinaryResource(int binaryId, DWORD &dwSize, LPBYTE &pBytes);
#endif
};

} // namespace util
} // namespace shared
