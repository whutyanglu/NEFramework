#ifndef MSYK_UI_DYNAMIC_BOX_H__
#define MSYK_UI_DYNAMIC_BOX_H__

#include "base/callback/callback.h"
#include "duilib/Core/DlgBuilder.h"

class DynamicBox : public nbase::SupportWeakCallback
{
public:
	DynamicBox();
	virtual ~DynamicBox();

	void Init(const std::wstring &xml, ui::PaintManager *pm, ui::Box *parent, ui::ControlBuilderCallback cb = nullptr);
	virtual void OnInit();
	virtual void SetVisible(bool visible = true);

	ui::Control* FindSubContrl(const std::wstring &name);
	virtual ui::Box*	 Box() { return box_; }

protected:
	ui::Box * box_ = NULL;
	ui::PaintManager *pm_ = NULL;
	std::string id_;
};

#endif // ~MSYK_UI_DYNAMIC_BOX_H__