#include "stdafx.h"
#include "knowledge_point_tile_item_box.h"

KnowledgePointTileItemBox::KnowledgePointTileItemBox(int width, int height, OnClick on_click, OnHover on_hover, OnLeave on_leave)
	: width_(width)
	, height_(height)
	, on_click_(on_click)
	, on_hover_(on_hover)
	, on_leave_(on_leave)
{
}

void KnowledgePointTileItemBox::OnInit()
{
	auto self = this;
	if (width_ > 0) {
		box_->SetFixedWidth(width_);
	}
	if (height_ > 0) {
		box_->SetFixedHeight(height_);
	}

	control_selected_ = FindSubContrl(L"control_selected");
	control_selected_->SetVisible(false);
	control_selected_->AttachResize([this](const ui::EventArgs &arg) {
		RECT rect_pos;
		RECT rect_padding = control_selected_->GetPadding();
		RECT rect_parent = box_->GetPos();
		int  width = control_selected_->GetFixedWidth();
		int  height = control_selected_->GetFixedHeight();

		rect_pos.left = rect_parent.right - rect_padding.right - width;
		rect_pos.right = rect_pos.left + width;
		rect_pos.top = rect_parent.top + rect_padding.top;
		rect_pos.bottom = rect_pos.top + height;

		control_selected_->SetPos(rect_pos);

		return true;
	});

	box_->AttachMouseLeave([this, self](const ui::EventArgs &arg) {
		is_mouse_enter_ = false;
		if (state_ != state_selected) {
			state_ = state_normal;
			if (on_leave_) {
				on_leave_(self);
			}
		}
		
		return true; 
	});

	box_->AttachMouseEnter([this, self](const ui::EventArgs &arg) {
		if (!is_mouse_enter_) {
			is_mouse_enter_ = true;
			if (state_ != state_selected) {
				state_ = state_hot;
				if (on_hover_) {
					on_hover_(self);
				}
			}
		}
		return true;
	});

	box_->AttachButtonDown([this, self](const ui::EventArgs &arg) {
		is_button_down_ = true;
		return true;
	});

	box_->AttachButtonUp([this, self](const ui::EventArgs &arg) {
		if (is_button_down_) {
			is_button_down_ = false;
			if (state_ == state_selected) {
				state_ = state_normal;
			
			}
			else {
				state_ = state_selected;
			}

			if (on_click_) {
				on_click_(self, state_ == state_selected);
			}
		}
		return true;
	});
}
