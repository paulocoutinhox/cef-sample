#include "shared/Entrypoint.hpp"

#include <windows.h>

#include "include/cef_sandbox_win.h"

#include "shared/AppFactory.hpp"
#include "shared/ClientManager.hpp"
#include "shared/util/EntrypointUtil.hpp"

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined automatically if using the required compiler version.
// Pass -DUSE_SANDBOX=OFF to the CMake command-line to disable use of the sandbox.

namespace shared
{

// entry point function for all processes
int APIENTRY wWinMain(HINSTANCE hInstance)
{
    // enable High-DPI support on Windows 7 or newer
    CefEnableHighDPISupport();

    void *sandboxInfo = nullptr;

#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object.
    // This is necessary for sandbox support on Windows.
    // See cef_sandbox_win.h for complete details.

    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    // provide CEF with command-line arguments
    CefMainArgs mainArgs(hInstance);

    // create a temporary CommandLine object
    CefRefPtr<CefCommandLine> commandLine = shared::util::EntrypointUtil::createCommandLine(mainArgs);

    // create a CefApp of the correct process type
    CefRefPtr<CefApp> app;

    switch (shared::util::EntrypointUtil::getProcessType(commandLine))
    {
    case shared::util::ProcessType::PROCESS_TYPE_BROWSER:
        app = AppFactory::createBrowserProcessApp();
        break;
    case shared::util::ProcessType::PROCESS_TYPE_RENDERER:
        app = AppFactory::createRendererProcessApp();
        break;
    case shared::util::ProcessType::PROCESS_TYPE_OTHER:
        app = AppFactory::createOtherProcessApp();
        break;
    }

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc) that share the same executable.
    // This function checks the command-line and, if this is a sub-process, executes the appropriate logic.
    int exitCode = CefExecuteProcess(mainArgs, app, sandboxInfo);

    if (exitCode >= 0)
    {
        // the sub-process has completed so return here
        return exitCode;
    }

    // create the singleton manager instance
    ClientManager manager;

    // specify CEF global settings here
    CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    // Initialize CEF for the browser process.
    // The first browser instance will be created in CefBrowserProcessHandler::OnContextInitialized() after CEF has been initialized.
    CefInitialize(mainArgs, settings, app, sandboxInfo);

    // Run the CEF message loop.
    // This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // shutdown CEF
    CefShutdown();

    return 0;
}

} // namespace shared
