#include "stdafx.h"
#include "cef_box.h"
#include "shared/cef/cef_nef_app.h"

CefBox::CefBox(std::string url)
	: url_(url)
{
}

void CefBox::OnInit()
{
	browser_ = dynamic_cast<shared::CefBrowserControl*>(FindSubContrl(L"cef_browser"));
	if (browser_ && !url_.empty()) {
		Navigate(url_);
	}
}

void CefBox::SetVisible(bool visible)
{
	if (browser_) {
		browser_->SetVisible(visible);
	}

	__super::SetVisible(visible);
}

bool CefBox::CanGoBack()
{
	if (browser_) {
		return browser_->CanGoBack();
	}
	return false;
}

bool CefBox::CanGoForward()
{
	if (browser_) {
		return browser_->CanGoForward();
	}
	
	return false;
}

void CefBox::GoBack()
{
	if (browser_) {
		browser_->GoBack();
	}
}

void CefBox::GoForward()
{
	if (browser_) {
		browser_->GoForward();
	}
}

void CefBox::Refresh()
{
	if (browser_) {
		browser_->Refresh();
	}
}

void CefBox::Navigate(const std::string & url, shared::js_callback cb, std::string message_name, std::string message)
{
	if (browser_ != NULL) {
		browser_->Navagate(url, cb, message_name, message);
	}
}

void CefBox::ExecuteJavaScript(const std::string & code, const std::string & script_url, int start_line)
{
	if (browser_ != NULL) {
		browser_->ExecuteJavaScript(code, script_url, start_line);
	}
}
