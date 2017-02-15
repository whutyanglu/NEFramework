// demon.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "demon.h"
#include <comdef.h>
#include "main_thread.h"
#include "thread/curl_thread.h"
#include "thread/misc_thread.h"
#include "ui/login/login_form.h"
#include "ui/common/directory_tree_view.h"
#include "api/net_api.h"
#include "shared/app_dump.h"
#include "shared/user.h"
#include "shared/cef/cef_nef_app.h"
#include "shared/cef/cef_browser_control.h"
#include "shared/cef/client_app.h"
#include "shared/cef/client_app_browser.h"
#include "shared/cef/client_app_other.h"
#include "shared/cef/client_app_renderer.h"
#include "shared/cef/main_message_loop_external_pump.h"
#include "shared/cef/main_message_loop_multithreaded_win.h"

using namespace ui;

void OnLoginFormClose()
{
	client::MainMessageLoop::Get()->Quit();
}

void OnLoginCallback()
{
	GlobalManager::SingletonShow<MainForm>();
}

void OnCefContextInitialized()
{
	QLOG_APP(L"OnCefContextInitialized");
}

using namespace ui;
using namespace shared;

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(kThreadUI);
	PreMessageLoop();

	REGIST_DUICONTROL(CefBrowserControl);
	using namespace std::placeholders;

	std::wstring app_crash = QCommand::Get(kCmdAppCrash);
	if (app_crash.empty()) {
		GlobalManager::SingletonShow<LoginForm>(
			nbase::Bind(OnLoginCallback),
			nbase::Bind(OnLoginFormClose));
	}
	else {
		std::wstring content(L"程序崩溃了，崩溃日志：");
		content.append(app_crash);
	}
}

void MainThread::Cleanup()
{
	PostMessageLoop();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
}

void MainThread::PreMessageLoop()
{

}

void MainThread::PostMessageLoop()
{

}


std::vector<std::unique_ptr<nbase::FrameworkThread>> StartThreads()
{
	std::unique_ptr<nbase::FrameworkThread> curl = std::make_unique<CurlThread>(kThreadCurl, "Curl http thread");
	std::unique_ptr<nbase::FrameworkThread> msic = std::make_unique<MiscThread>(kThreadGlobalMisc, "Curl http thread");

	curl->Start();
	msic->Start();

	std::vector<std::unique_ptr<nbase::FrameworkThread>> vecs;
	vecs.push_back(std::move(curl));
	vecs.push_back(std::move(msic));
	return vecs;
};


int CEF_Main(HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	using namespace shared;
	using namespace std::placeholders;
	using namespace client;

	nbase::AtExitManager at_manager;

	CComModule _Module;
	_Module.Init(NULL, hInstance);

	_wsetlocale(LC_ALL, L"chs");
	std::wstring cmdline;
	nbase::win32::MBCSToUnicode(lpszCmdLine, cmdline);

	::OleInitialize(NULL);

	srand((unsigned int)time(NULL));

	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	// Enable High-DPI support on Windows 7 or newer.
	CefEnableHighDPISupport();

	CefMainArgs main_args(hInstance);

	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	// Create a ClientApp of the correct type.
	CefRefPtr<CefApp> app;
	ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
	if (process_type == ClientApp::BrowserProcess)
		app = new ClientAppBrowser();
	else if (process_type == ClientApp::RendererProcess)
		app = new ClientAppRenderer();
	else if (process_type == ClientApp::OtherProcess)
		app = new ClientAppOther();

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app, NULL);
	if (exit_code >= 0)
		return exit_code;

	CefSettings settings;
	// Populate the settings based on command line arguments.
	//settings.external_message_pump = true;
	settings.external_message_pump = true;

	//// Create the main message loop object.
	scoped_ptr<MainMessageLoop> message_loop;
	message_loop = MainMessageLoopExternalPump::Create();
	//message_loop.reset(new  MainMessageLoopMultithreadedWin);

	if (!CefInitialize(main_args, settings, app, NULL))
		return -1;

#ifdef DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(L"Command:\n%s\n\n", cmdline.c_str());
#else
	QLogImpl::GetInstance()->SetLogFile(L"msyk.log");
#endif
	QLogImpl::GetInstance()->SetLogLevel(LV_APP);
	QLOG_APP(L"===============app start===============");

	REGIST_DUICONTROL(CefBrowserControl);
	REGIST_DUICONTROL(DirectoryTreeNode);
	REGIST_DUICONTROL(DirectoryTreeView)

	std::wstring themedir = QPath::GetAppPath();
	PaintManager::SetInstance(hInstance);
	PaintManager::SetResourceType(UILIB_FILE);
	PaintManager::SetResourcePath(themedir + L"themes\\default");
	if (!GlobalManager::Start(themedir + L"themes\\default")) {
		return -1;
	}

#if defined NETAPI_TEST
	netapi_test_start();
#endif

	auto threads = StartThreads();

	auto login_ptr = GlobalManager::CreateForm<LoginForm>(
		nbase::Bind(OnLoginCallback),
		nbase::Bind(OnLoginFormClose));

	message_loop->Run();

	CefShutdown();
	message_loop.reset();

	login_ptr.reset();
	GlobalManager::Shutdown();
	threads.clear();

	QLOG_APP(L"app exit");
	//// 是否重新运行程序
	//std::wstring restart = QCommand::Get(kCmdRestart);
	//if (!restart.empty()) {
	//	std::wstring cmd;
	//	std::wstring acc = QCommand::Get(kCmdAccount);
	//	if (!acc.empty())
	//		cmd.append(nbase::StringPrintf(L" /%s %s ", kCmdAccount.c_str(), acc.c_str()));
	//	std::wstring exit_why = QCommand::Get(kCmdExitWhy);
	//	if (!exit_why.empty())
	//		cmd.append(nbase::StringPrintf(L" /%s %s ", kCmdExitWhy.c_str(), exit_why.c_str()));
	//	QCommand::RestartApp(cmd);
	//}

	_Module.Term();
	::OleUninitialize();

	return 0;

}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpszCmdLine, int nCmdShow)
{
	CEF_Main(hInstance, lpszCmdLine, nCmdShow);

	return 0;
}
