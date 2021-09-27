#include "shared/macos/ProcessEntrypoint.hpp"

#include "include/cef_app.h"
#include "include/wrapper/cef_library_loader.h"

#include "shared/AppFactory.hpp"
#include "shared/util/EntrypointUtil.hpp"

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined automatically.
// Pass -DUSE_SANDBOX=OFF to the CMake command-line to disable use of the sandbox.

#if defined(CEF_USE_SANDBOX)
#include "include/cef_sandbox_mac.h"
#endif

namespace shared
{

// entry point function for sub-processes
int main(int argc, char *argv[])
{
#if defined(CEF_USE_SANDBOX)
    // initialize the macOS sandbox for this helper process
    CefScopedSandboxContext sandboxContext;

    if (!sandboxContext.Initialize(argc, argv))
    {
        return 1;
    }
#endif
    // load the CEF framework library at runtime instead of linking directly as required by the macOS sandbox implementation
    CefScopedLibraryLoader libraryLoader;

    if (!libraryLoader.LoadInHelper())
    {
        return 1;
    }

    // provide CEF with command-line arguments
    CefMainArgs mainArgs(argc, argv);

    // create a temporary CommandLine object
    CefRefPtr<CefCommandLine> commandLine = shared::util::EntrypointUtil::createCommandLine(mainArgs);

    // create a CefApp of the correct process type
    CefRefPtr<CefApp> app;

    switch (shared::util::EntrypointUtil::getProcessType(commandLine))
    {
    case shared::util::ProcessType::PROCESS_TYPE_RENDERER:
        app = AppFactory::createRendererProcessApp();
        break;
    case shared::util::ProcessType::PROCESS_TYPE_OTHER:
        app = AppFactory::createOtherProcessApp();
        break;
    default:
        break;
    }

    // execute the sub-process
    return CefExecuteProcess(mainArgs, app, nullptr);
}

} // namespace shared
