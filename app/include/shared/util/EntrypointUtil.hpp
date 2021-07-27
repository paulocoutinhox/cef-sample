#pragma once

#include "include/cef_command_line.h"

namespace shared
{

namespace util
{

// process types that may have different CefApp instances
enum ProcessType
{
    PROCESS_TYPE_BROWSER,
    PROCESS_TYPE_RENDERER,
    PROCESS_TYPE_OTHER,
};

// these flags must match the Chromium values
const char kProcessType[] = "type";
const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
const char kZygoteProcess[] = "zygote";
#endif

// functionality common to all program entry point implementations
class EntrypointUtil
{

public:
    // create a new CommandLine object for use before CEF initialization
    static CefRefPtr<CefCommandLine> createCommandLine(const CefMainArgs &mainArgs);

    // determine the process type based on command-line arguments
    static ProcessType getProcessType(const CefRefPtr<CefCommandLine> &commandLine);
};

} // namespace util
} // namespace shared
