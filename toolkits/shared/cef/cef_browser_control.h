#ifndef  SHARED_CEF_CEF_BROWSHER_H__
#define SHARED_CEF_CEF_BROWSHER_H__

#include "internal/browser_window.h"
using namespace client;
namespace shared
{

// Note : 同步接口，请在回调内再POST到主线程
using js_callback = std::function<void(std::string, std::string)>;

class CefBrowserControl : virtual CefBase
						, public ui::Control
						, public BrowserWindow::Delegate
{
public:
	DECLARE_DUICONTROL(CefBrowserControl)

	CefBrowserControl(std::string url = "");
	virtual ~CefBrowserControl();

	void Navagate(std::string url, js_callback cb = nullptr, std::string message_name = "", std::string message = "");
	void ExecuteJavaScript(const std::string& code, const std::string& script_url, int start_line = 0);

	bool CanGoBack();
	bool CanGoForward();
	void GoBack();
	void GoForward();
	void Refresh();

	bool IsClosing() const;
	void Close(bool quit = false);

	virtual std::wstring GetClass() const override;
	virtual LPVOID GetInterface(std::wstring pstrName) override;
	virtual void SetVisible(bool bVisible = true) override;
	virtual void DoInit();

	// BrowserWindow::Delegate methods.
	void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
	void OnBrowserWindowDestroyed() override;
	void OnSetAddress(const std::string& url) override;
	void OnSetTitle(const std::string& title) override;
	void OnSetFullscreen(bool fullscreen) override;
	void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) override;
	void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) override;

private:
	bool InitBrowser();
	void CreateBrowser();
	void OnSize(bool minimized = false);

private:
	std::string url_;
	bool is_created_ = false;
	RECT start_rect_;
	scoped_ptr<BrowserWindow> browser_window_;
	bool quit_ = false;

	IMPLEMENT_REFCOUNTING(CefBrowserControl)
};

} // namespace shared

#endif // ! SHARED_CEF_CEF_BROWSHER_H__
