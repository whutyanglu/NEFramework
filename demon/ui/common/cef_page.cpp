#include "stdafx.h"
#include "cef_page.h"

CefBoxMap CefPage::shared_browsers_;

CefPage::CefPage(int index, UiPage * back, 
	ui::PaintManager *pm, ui::Box * parent, std::string url, OnGoBack on_goback, OnGoForward on_gonext)
	: UiPage(index, back, on_goback, on_gonext)
	, url_(url)
{
	auto browser = shared_browsers_.find(index);
	if (browser == shared_browsers_.end()) {
		cef_box_ = std::make_shared<CefBox>(url_);
		cef_box_->Init(L"/common/cef_box.xml", pm, parent);
		shared_browsers_.insert({ index, cef_box_ });
	}
	else {
		cef_box_ = browser->second;
	}

	if (back != nullptr) {
		CefPage *page = dynamic_cast<CefPage*>(back);
		if (page != nullptr) {
			page->cef_box_->SetVisible(false);
		}
	}
	
	cef_box_->SetVisible(true);
}

CefPage::~CefPage()
{
}

void CefPage::OnShow()
{
	cef_box_->SetVisible(true);
}

UiPage* CefPage::BeforeGoBack()
{
	if (cef_box_->CanGoBack()) {
		cef_box_->GoBack();
		return this;
	}

	if (page_back_ != nullptr) {
		cef_box_->SetVisible(false);
		CefPage *page = dynamic_cast<CefPage*>(page_back_);
		if (page != nullptr) {
			page->cef_box_->SetVisible(true);
		}
	}
	return page_back_;
}

UiPage* CefPage::BeforeGoForward()
{
	if (cef_box_->CanGoForward()) {
		cef_box_->GoForward();
		return this;
	}

	if (page_forward_ != nullptr) {
		cef_box_->SetVisible(false);
		CefPage *page = dynamic_cast<CefPage*>(page_forward_);
		if (page != nullptr) {
			page->cef_box_->SetVisible(true);
		}
	}
	return page_forward_;
}

void CefPage::Refresh()
{
	cef_box_->Refresh();
}

ui::Box * CefPage::Box()
{
	return cef_box_->Box();
}

void CefPage::SetForward(UiPage *page)
{
	cef_box_->SetVisible(false);
	return __super::SetForward(page);
}

void CefPage::SetVisible(bool visible)
{
	cef_box_->SetVisible(visible);
}
