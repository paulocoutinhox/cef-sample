#include "shared/Entrypoint.hpp"

#import <Cocoa/Cocoa.h>

#include "include/cef_application_mac.h"
#include "include/wrapper/cef_helpers.h"
#import "include/wrapper/cef_library_loader.h"

#include "shared/AppFactory.hpp"
#include "shared/ClientManager.hpp"

// receives notifications from the application
@interface SharedAppDelegate : NSObject <NSApplicationDelegate>
- (void)createApplication:(id)object;
- (void)tryToTerminateApplication:(NSApplication *)app;
@end

// provide the CefAppProtocol implementation required by CEF
@interface SharedApplication : NSApplication <CefAppProtocol>
{
@private
    BOOL handlingSendEvent_;
}
@end

@implementation SharedApplication
- (BOOL)isHandlingSendEvent
{
    return handlingSendEvent_;
}

- (void)setHandlingSendEvent:(BOOL)handlingSendEvent
{
    handlingSendEvent_ = handlingSendEvent;
}

- (void)sendEvent:(NSEvent *)event
{
    CefScopedSendingEvent sendingEventScoper;
    [super sendEvent:event];
}

// |-terminate:| is the entry point for orderly "quit" operations in Cocoa. This
// includes the application menu's quit menu item and keyboard equivalent, the
// application's dock icon menu's quit menu item, "quit" (not "force quit") in
// the Activity Monitor, and quits triggered by user logout and system restart
// and shutdown.
//
// The default |-terminate:| implementation ends the process by calling exit(),
// and thus never leaves the main run loop. This is unsuitable for Chromium
// since Chromium depends on leaving the main run loop to perform an orderly
// shutdown. We support the normal |-terminate:| interface by overriding the
// default implementation. Our implementation, which is very specific to the
// needs of Chromium, works by asking the application delegate to terminate
// using its |-tryToTerminateApplication:| method.
//
// |-tryToTerminateApplication:| differs from the standard
// |-applicationShouldTerminate:| in that no special event loop is run in the
// case that immediate termination is not possible (e.g., if dialog boxes
// allowing the user to cancel have to be shown). Instead, this method tries to
// close all browsers by calling CloseBrowser(false) via
// ClientHandler::CloseAllBrowsers. Calling CloseBrowser will result in a call
// to ClientHandler::DoClose and execution of |-performClose:| on the NSWindow.
// DoClose sets a flag that is used to differentiate between new close events
// (e.g., user clicked the window close button) and in-progress close events
// (e.g., user approved the close window dialog). The NSWindowDelegate
// |-windowShouldClose:| method checks this flag and either calls
// CloseBrowser(false) in the case of a new close event or destructs the
// NSWindow in the case of an in-progress close event.
// ClientHandler::OnBeforeClose will be called after the CEF NSView hosted in
// the NSWindow is dealloc'ed.
//
// After the final browser window has closed ClientHandler::OnBeforeClose will
// begin actual tear-down of the application by calling CefQuitMessageLoop.
// This ends the NSApplication event loop and execution then returns to the
// main() function for cleanup before application termination.
//
// The standard |-applicationShouldTerminate:| is not supported, and code paths
// leading to it must be redirected.
- (void)terminate:(id)sender
{
    SharedAppDelegate *delegate = static_cast<SharedAppDelegate *>([NSApp delegate]);
    [delegate tryToTerminateApplication:self];

    // Return, don't exit.
    // The application is responsible for exiting on its own.
}
@end

@implementation SharedAppDelegate

// create the application on the UI thread
- (void)createApplication:(id)object
{
    [NSApplication sharedApplication];
    [[NSBundle mainBundle] loadNibNamed:@"MainMenu"
                                  owner:NSApp
                        topLevelObjects:nil];

    // set the delegate for application events
    [[NSApplication sharedApplication] setDelegate:self];
}

- (void)tryToTerminateApplication:(NSApplication *)app
{
    shared::ClientManager *manager = shared::ClientManager::getInstance();

    if (manager && !manager->isClosing())
    {
        manager->closeAllBrowsers(false);
    }
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    return NSTerminateNow;
}
@end

namespace shared
{

// entrypoint function for the browser process
int main(int argc, char *argv[])
{
    // Load the CEF framework library at runtime instead of linking directly as required by the macOS sandbox implementation
    CefScopedLibraryLoader libraryLoader;

    if (!libraryLoader.LoadInMain())
    {
        return 1;
    }

    // initialize the AutoRelease pool
    NSAutoreleasePool *autopool = [[NSAutoreleasePool alloc] init];

    // provide CEF with command-line arguments
    CefMainArgs mainArgs(argc, argv);

    // create a CefApp for the browser process
    CefRefPtr<CefApp> app = AppFactory::createBrowserProcessApp();

    // initialize the SharedApplication instance
    [SharedApplication sharedApplication];

    // create the singleton manager instance
    ClientManager manager;

    // specify CEF global settings here
    CefSettings settings;

    // Initialize CEF for the browser process.
    // The first browser instance will be created in CefBrowserProcessHandler::OnContextInitialized() after CEF has been initialized.
    CefInitialize(mainArgs, settings, app, NULL);

    // create the application delegate
    NSObject *delegate = [[SharedAppDelegate alloc] init];
    [delegate performSelectorOnMainThread:@selector(createApplication:)
                               withObject:nil
                            waitUntilDone:NO];

    // Run the CEF message loop.
    // This will block until CefQuitMessageLoop() is called.
    CefRunMessageLoop();

    // shutdown CEF
    CefShutdown();

    // release the delegate
    [delegate release];

    // release the AutoRelease pool
    [autopool release];

    return 0;
}

} // namespace shared
