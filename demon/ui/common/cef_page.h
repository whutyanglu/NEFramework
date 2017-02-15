#ifndef MSYK_UI_CEF_PAGE_H__
#define MSYK_UI_CEF_PAGE_H__

#include "ui_page.h"
#include "cef_box.h"

using CefBoxMap = std::map<int, std::shared_ptr<CefBox>>;

class CefPage final : public UiPage
{
public:
	CefPage(int index, UiPage *back, ui::PaintManager *pm, ui::Box* parent, std::string url = "",
		OnGoBack on_goback = nullptr, OnGoForward on_gonext = nullptr);
	~CefPage();

	virtual void OnShow() override;
	virtual UiPage* BeforeGoBack() override;
	virtual UiPage* BeforeGoForward() override;
	virtual void Refresh() override;
	virtual ui::Box* Box() override;
	virtual void SetForward(UiPage *page);
	virtual void SetVisible(bool visible = true);

private:
	std::string url_;
	std::shared_ptr<CefBox>	cef_box_;

	static CefBoxMap shared_browsers_;
};

#endif // MSYK_UI_CEF_PAGE_H__