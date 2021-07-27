#include "shared/Entrypoint.hpp"

#include <X11/Xlib.h>

#include "include/base/cef_logging.h"

#include "shared/AppFactory.hpp"
#include "shared/ClientManager.hpp"
#include "shared/util/EntrypointUtil.hpp"

namespace shared
{

int XErrorHandlerImpl(Display *display, XErrorEvent *event)
{
    LOG(WARNING) << "X error received: "
                 << "type: " << event->type << ", "
                 << "serial: " << event->serial << ", "
                 << "error code: " << static_cast<int>(event->error_code) << ", "
                 << "request code: " << static_cast<int>(event->request_code) << ", "
                 << "minor code: " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display *display)
{
    return 0;
}

// entry point function for all processes
int main(int argc, char *argv[])
{
    // provide CEF with command-line arguments
    CefMainArgs mainArgs(argc, argv);

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
    int exitCode = CefExecuteProcess(mainArgs, app, NULL);

    if (exitCode >= 0)
    {
        // the sub-process has completed so return here
        return exitCode;
    }

    // install xlib error handlers so that the application won't be terminated on non-fatal errors
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    // create the singleton manager instance
    ClientManager manager;

    // specify CEF global settings here
    CefSettings settings;

    // Initialize CEF for the browser process.
    // The first browser instance will be created in CefBrowserProcessHandler::OnContextInitialized() after CEF has been initialized.
    CefInitialize(mainArgs, settings, app, NULL);

    // Run the CEF message loop.
    // This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // shut down CEF
    CefShutdown();

    return 0;
}

} // namespace shared
