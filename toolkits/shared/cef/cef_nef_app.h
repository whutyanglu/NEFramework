#ifndef SHARED_CEF_CEF_APP_H__
#define SHARED_CEF_CEF_APP_H__

#include "base/framework/message_loop.h"
#include "cef/include/cef_app.h"
#include <functional>

namespace shared
{

using cef_context_initialized = std::function<void()>;

class CefNefApp : public CefApp
				, public CefBrowserProcessHandler
				, public nbase::MessageLoop::TaskObserver
{
public:
	CefNefApp(cef_context_initialized cci_cb = nullptr);
	virtual ~CefNefApp();

	virtual void PreProcessTask();
	virtual void PostProcessTask();

	static CefRefPtr<CefApp> Start(HINSTANCE h, cef_context_initialized cb = nullptr);
	static void Shutdown();
	static void QuitMessageLoop();

	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
		return this;
	}
	virtual void OnContextInitialized() override;
	virtual void OnScheduleMessagePumpWork(int64 delay) override;


	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CefNefApp);

private:
	cef_context_initialized ctx_inited_ = nullptr;
};

} // namespace shared

#endif // ~SHARED_CEF_CEF_APP_H__