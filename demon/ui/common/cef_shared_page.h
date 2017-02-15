#ifndef MSYK_UI_BASIC_INFO_PAGE_H__
#define MSYK_UI_CEF_SHARED_PAGE_H__

#include "ui_page.h"
#include "cef_box.h"

class CefSharedPage final : public UiPage
{
public:
	CefSharedPage(int index, UiPage *back, std::shared_ptr<CefBox> cefbox,  std::string url = "", 
		OnGoBack on_goback = nullptr, OnGoForward on_gonext = nullptr);
	~CefSharedPage();

	virtual void OnShow() override;
	virtual ui::Box* Box() override;

private:
	std::string url_;
	std::shared_ptr<CefBox>	cef_box_ = nullptr;
};


#endif // MSYK_UI_CEF_SHARED_PAGE_H__