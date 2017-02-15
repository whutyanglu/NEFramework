#include "stdafx.h"
#include "cef_shared_page.h"

CefSharedPage::CefSharedPage(int index, UiPage* back, std::shared_ptr<CefBox> cefbox, std::string url, OnGoBack on_goback, OnGoForward on_gonext)
	: UiPage(index, back, on_goback, on_gonext)
	, cef_box_(cefbox)
	, url_(url)
{
	cef_box_->Navigate(url_);
}

CefSharedPage::~CefSharedPage()
{

}

void CefSharedPage::OnShow()
{
	if (cef_box_) {
		cef_box_->Navigate(url_);
	}
}

ui::Box* CefSharedPage::Box()
{
	if (cef_box_) {
		return cef_box_->Box();
	}

	return nullptr;
}
