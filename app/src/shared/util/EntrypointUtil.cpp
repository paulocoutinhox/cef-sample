#include "shared/util/EntrypointUtil.hpp"

#if defined(OS_WIN)
#include <windows.h>
#endif

namespace shared
{

namespace util
{

CefRefPtr<CefCommandLine> EntrypointUtil::createCommandLine(const CefMainArgs &mainArgs)
{
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
    commandLine->InitFromString(::GetCommandLineW());
#else
    commandLine->InitFromArgv(mainArgs.argc, mainArgs.argv);
#endif
    return commandLine;
}

ProcessType EntrypointUtil::getProcessType(const CefRefPtr<CefCommandLine> &commandLine)
{
    // the command-line flag won't be specified for the browser process
    if (!commandLine->HasSwitch(kProcessType))
    {
        return ProcessType::PROCESS_TYPE_BROWSER;
    }

    const std::string &processType = commandLine->GetSwitchValue(kProcessType);

    if (processType == kRendererProcess)
    {
        return ProcessType::PROCESS_TYPE_RENDERER;
    }

#if defined(OS_LINUX)
    // On Linux the zygote process is used to spawn other process types.
    // Since we don't know what type of process it will be we give it the renderer app.
    if (processType == kZygoteProcess)
    {
        return PROCESS_TYPE_RENDERER;
    }
#endif

    return ProcessType::PROCESS_TYPE_OTHER;
}

} // namespace util
} // namespace shared
