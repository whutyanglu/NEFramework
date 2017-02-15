#ifndef MSYK_UI_UIPAGE_H__
#define MSYK_UI_UIPAGE_H__

#include "base/callback/callback.h"
#include "dynamic_box.h"

class UiPage;

using OnGoBack = std::function<void(UiPage*)>;
using OnGoForward = std::function<void(UiPage*)>;

class UiPage :  public DynamicBox /*public nbase::SupportWeakCallback*/
{
public:
	UiPage(int index, UiPage *back = nullptr, OnGoBack on_goback = nullptr, OnGoForward on_gonext = nullptr);
	virtual ~UiPage() {}

	virtual void SetForward(UiPage *page) {page_forward_ = page;}
	virtual void Refresh() {}
	virtual UiPage * BeforeGoBack() { return page_back_; }
	virtual UiPage * BeforeGoForward() { return page_forward_; }

	UiPage *GetBack() { return page_back_; }
	UiPage *GetForward() { return page_forward_; }
	int  GetIndex() { return index_; }
	void SetBack(UiPage *page) { page_back_ = page; }
	bool GoBack();
	bool GoForward();
	
protected:
	virtual void OnShow() {}
	
protected:
	UiPage *page_forward_ = nullptr;
	UiPage *page_back_ = nullptr;
	OnGoBack on_goback_ = nullptr;
	OnGoForward on_goforward_ = nullptr;
	int index_;
};


#endif // ~MSYK_UI_UIPAGE_H__