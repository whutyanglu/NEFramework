#include "stdafx.h"
#include "ui_page.h"

UiPage::UiPage(int index, UiPage * page, OnGoBack on_goback, OnGoForward on_gonext)
	: index_(index)
	, page_back_(page)
	, on_goback_(on_goback)
	, on_goforward_(on_gonext)
{
	if (page_back_ != nullptr) {
		UiPage *page_forward = page_back_->GetForward();
		if (page_forward != nullptr) {
			page_forward->SetBack(this);
			page_forward_ = page_forward;
		}
		page_back_->SetForward(this);
	}
}

bool UiPage::GoBack()
{
	UiPage *page = BeforeGoBack();
	if (page == nullptr) {
		return false;
	}

	if (on_goback_) {
		on_goback_(page);
	}
	page->OnShow();

	return true;
}

bool UiPage::GoForward()
{
	UiPage *page = BeforeGoForward();
	if (page == nullptr) {
		return false;
	}

	if (on_goforward_) {
		on_goforward_(page);
	}
	page->OnShow();

	return true;
}
