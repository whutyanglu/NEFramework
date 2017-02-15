#include "cef_nef_app.h"
#include <string>

#include "cef_nef_client.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

#include "shared/threads.h"
#include "base/thread/thread_manager.h"

namespace shared
{

CefNefApp::CefNefApp(cef_context_initialized cci_cb)
	: ctx_inited_(cci_cb)
{
}

CefNefApp::~CefNefApp()
{

}

void CefNefApp::PreProcessTask()
{
//	CefDoMessageLoopWork();
}

void CefNefApp::PostProcessTask()
{
	//CefDoMessageLoopWork();
}

CefRefPtr<CefApp> CefNefApp::Start(HINSTANCE h, cef_context_initialized cb)
{
	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(h);

	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	// Create a ClientApp of the correct type.
	//CefRefPtr<CefApp> app;
	//ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
	//if (process_type == ClientApp::BrowserProcess)
	//	app = new ClientAppBrowser();
	//else if (process_type == ClientApp::RendererProcess)
	//	app = new ClientAppRenderer();
	//else if (process_type == ClientApp::OtherProcess)
	//	app = new ClientAppOther();

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, NULL);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return nullptr;
	}

	// Specify CEF global settings here.
	CefSettings settings;
	settings.windowless_rendering_enabled = false;
	settings.multi_threaded_message_loop = false;
	settings.external_message_pump = true;
	settings.no_sandbox = true;
	
	// SimpleApp implements application-level callbacks for the browser process.
	// It will create the first browser instance in OnContextInitialized() after
	// CEF has initialized.
	CefRefPtr<CefNefApp> app(new CefNefApp(cb));

	// Initialize CEF.
	CefInitialize(main_args, settings, app.get(), NULL);

	CefDoMessageLoopWork();

	return app;
}

void CefNefApp::Shutdown()
{
	CefShutdown();
}

void CefNefApp::QuitMessageLoop()
{
	CefQuitMessageLoop();
}

void CefNefApp::OnContextInitialized()
{
	if (ctx_inited_) {
		ctx_inited_();
	}


}

void CefNefApp::OnScheduleMessagePumpWork(int64 delay)
{
	nbase::ThreadManager::PostDelayedTask(kThreadUI, nbase::Bind(CefDoMessageLoopWork), 
		nbase::TimeDelta::FromMilliseconds(delay));
}

}