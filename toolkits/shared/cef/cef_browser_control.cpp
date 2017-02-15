#include "stdafx.h"
#include "cef_browser_control.h"
#include "main_message_loop.h"
#include "internal/browser_window_std_win.h"
#include "internal/client_handler_std.h"

namespace shared
{

using namespace ui;
using namespace client;

typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

namespace
{

std::string handle_url;
std::string handle_message_name;
std::string handle_message;
js_callback handle_callback;

// Handle messages in the browser process.
class Handler : public CefMessageRouterBrowserSide::Handler {
public:
	Handler() {}

	// Called due to cefQuery execution in binding.html.
	virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id,
		const CefString& request,
		bool persistent,
		CefRefPtr<Callback> callback) OVERRIDE {
		if (handle_url.empty()) {
			return false;
		}
		// Only handle messages from the test URL.
		const std::string& url = frame->GetURL();
		if (url.find(handle_url) != 0)
			return false;

		const std::string& message_name = request;
		if (message_name.find(handle_message_name) == 0) {
			if (handle_callback) {
				handle_callback(message_name, handle_message);
			}
			callback->Success(handle_message);
			return true;
		}

		return false;
	}
};

}  // namespace

IMPLEMENT_DUICONTROL(CefBrowserControl)

CefBrowserControl::CefBrowserControl(std::string url)
	:url_(url)
{
}

CefBrowserControl::~CefBrowserControl()
{
}

void CefBrowserControl::Navagate(std::string url, js_callback cb, std::string message_name, std::string message)
{
	url_ = url;
	handle_url = url;
	handle_message_name = message_name;
	handle_message = message;
	handle_callback = cb;
	if (is_created_) {
		browser_window_->GetBrowser()->GetMainFrame()->LoadURL(url);
	}
}

void CefBrowserControl::ExecuteJavaScript(const std::string& code,  const std::string& script_url,int start_line)
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			browser->GetMainFrame()->ExecuteJavaScript(code, script_url, start_line);
		}
	}
}

bool CefBrowserControl::CanGoBack()
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			return browser->CanGoBack();
		}
	}

	return false;
}

bool CefBrowserControl::CanGoForward()
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			return browser->CanGoForward();
		}
	}

	return false;
}

void CefBrowserControl::GoBack()
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			if (browser->CanGoBack()) {
				browser->GoBack();
			}
		}
	}
}

void CefBrowserControl::GoForward()
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			if (browser->CanGoForward()) {
				browser->GoForward();
			}
		}
	}
}

void CefBrowserControl::Refresh()
{
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			browser->Reload();
		}
	}
}

bool CefBrowserControl::IsClosing() const
{
	if (browser_window_) {
		return browser_window_->IsClosing();
	}
	return false;
}

void CefBrowserControl::Close(bool quit)
{
	quit_ = quit;
	if (browser_window_) {
		CefRefPtr<CefBrowser> browser = browser_window_->GetBrowser();
		if (browser) {
			browser->GetHost()->CloseBrowser(false);
		}
	}
}

std::wstring CefBrowserControl::GetClass() const
{
	return L"CefBrowserControl";
}

LPVOID CefBrowserControl::GetInterface(std::wstring pstrName)
{
	if (_tcsicmp(pstrName.c_str(), L"CefBrowserControl") == 0) {
		return static_cast<CefBrowserControl*>(this);
	}

	return Control::GetInterface(pstrName);
}

void CefBrowserControl::SetVisible(bool bVisible)
{
	if (browser_window_) {
		bVisible ? browser_window_->Show() : browser_window_->Hide();
	}
	
	__super::SetVisible(bVisible);
}

void CefBrowserControl::DoInit()
{
	AttachResize(nbase::Bind(&CefBrowserControl::InitBrowser, this));
}

void CefBrowserControl::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	
}

void CefBrowserControl::OnBrowserWindowDestroyed()
{
	if (quit_) {
		MainMessageLoop::Get()->Quit();
	}
}

void CefBrowserControl::OnSetAddress(const std::string & url)
{
}

void CefBrowserControl::OnSetTitle(const std::string & title)
{
}

void CefBrowserControl::OnSetFullscreen(bool fullscreen)
{
}

void CefBrowserControl::OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{
}

void CefBrowserControl::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
{
}

bool CefBrowserControl::InitBrowser()
{
	if (!is_created_) {
		is_created_ = true;
		browser_window_.reset(new BrowserWindowStdWin(this, url_));

		// Create the native root window on the main thread.
		if (CURRENTLY_ON_MAIN_THREAD()) {
			CreateBrowser();
		}
		else {
			MAIN_POST_CLOSURE(
				base::Bind(&CefBrowserControl::CreateBrowser, this));
		}
	}
	else {
		OnSize();
	}

	return true;
}

void CefBrowserControl::CreateBrowser()
{
	RECT rect = GetPos();
	CefRect cef_rect(rect.left, rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top);

	CefBrowserSettings settings;
	browser_window_->CreateBrowser(pm_->GetPaintWindow(), cef_rect, settings, NULL, new Handler);
}

void CefBrowserControl::OnSize(bool minimized)
{
	if (minimized) {
		if (browser_window_) {
			browser_window_->Hide();
			return;
		}
	}

	if (browser_window_) {
		browser_window_->Show();
	}

	RECT rect = GetPos();
	HWND browser_hwnd = NULL;
	if (browser_window_) {
		browser_window_->SetBounds(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
}


} // shared