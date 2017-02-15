#include "stdafx.h"
#include "course_navigation_box.h"

CousreNavigationBox::CousreNavigationBox(OnNavigationItemSelected cb)
	:on_item_selected_(cb)
{
}

void CousreNavigationBox::OnInit()
{
	list_ = dynamic_cast<ui::List *>(FindSubContrl(L"list_course_navigation"));
	list_->AttachItemSelected([this](const ui::EventArgs& arg) {
		int index = arg.w_param_;
		if (on_item_selected_) {
			on_item_selected_((NavigationIndex)index);
		}
		return true; 
	});
}

void CousreNavigationBox::SelectedItem(NavigationIndex index)
{
	ASSERT(index >= 0 && index < list_->GetCount());
	if (index >= 0 && index < list_->GetCount()) {
		ui::IListItem *item = dynamic_cast<ui::IListItem*>(list_->GetItemAt(index));
		if (!item->IsSelected()) {
			list_->SelectItem((int)index);
		}
	}
}
