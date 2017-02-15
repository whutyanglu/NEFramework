#include "StdAfx.h"
#include "HBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(HBox)
	HBox::HBox() : sep_width_(0), btn_state_(0), imm_mode_(false)
	{
		last_mouse_pt_.x = last_mouse_pt_.y = 0;
		::ZeroMemory(&new_pos_, sizeof(new_pos_));
	}

	std::wstring HBox::GetClass() const
	{
		return _T("HBox");
	}

	LPVOID HBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_HBOX) == 0 ) return static_cast<HBox*>(this);
		return Box::GetInterface(pstrName);
	}

	UINT HBox::GetControlFlags() const
	{
		if( IsEnabled() && sep_width_ != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	int HBox::GetFixedWidth()
	{
		int width = 0;
		if (cxy_fired_sz_.cx == -1) {
			int count = GetCount();
			int visible_count = 0;
			for (int i = 0; i < count; ++i)
			{
				Control *ctl = GetItemAt(i);
				if (ctl->IsVisible()) {
					++visible_count;
					width += ctl->GetFixedWidth();
					width += (ctl->GetPadding().left + ctl->GetPadding().right);
					width += child_padding_;
				}
			}

			if (visible_count > 1) {
				width -= child_padding_;
			}

			width += (rc_inset_.right + rc_inset_.left);
			cxy_fired_sz_.cx = width;
		}

		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_.cx);
		}
		return cxy_fired_sz_.cx;
		
	}

	int HBox::GetFixedHeight()
	{
		return __super::GetFixedHeight();
	}

	void HBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		RECT rc_inset = HBox::rc_inset_;
		GetManager()->GetDPIObj()->Scale(&rc_inset);
		rc.left += rc_inset.left;
		rc.top += rc_inset.top;
		rc.right -= rc_inset.right;
		rc.bottom -= rc_inset.bottom;
		if (vertical_scrollbar_ && vertical_scrollbar_->IsVisible()) {
			rc.right -= vertical_scrollbar_->GetFixedWidth();
		}
		if (horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible()) {
			rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
		}

		if (items_.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if (horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible()) {
			szAvailable.cx += horizontal_scrollbar_->GetScrollRange();
		}
		if (vertical_scrollbar_ && vertical_scrollbar_->IsVisible()) {
			szAvailable.cy += vertical_scrollbar_->GetScrollRange();
		}

		int cyNeeded = 0;
		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		SIZE szControlAvailable;
		int iControlMaxWidth = 0;
		int iControlMaxHeight = 0;
		for( int it1 = 0; it1 < items_.GetSize(); it1++ ) {
			Control* pControl = static_cast<Control*>(items_[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			szControlAvailable = szAvailable;
			RECT rcPadding = pControl->GetPadding();
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cx == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			cxFixed += sz.cx + pControl->GetPadding().left + pControl->GetPadding().right;

			sz.cy = MAX(sz.cy, 0);
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			cyNeeded = MAX(cyNeeded, sz.cy + rcPadding.top + rcPadding.bottom);
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * child_padding_;

		// Place elements
		int cxNeeded = 0;
		int cxExpand = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			iPosX -= horizontal_scrollbar_->GetScrollPos();
		}
		int iEstimate = 0;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}
			
			iEstimate += 1;
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cx -= rcPadding.left;

			szControlAvailable = szRemaining;
			szControlAvailable.cy -= rcPadding.top + rcPadding.bottom;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
      cxFixedRemaining = cxFixedRemaining - (rcPadding.left + rcPadding.right);
			if (iEstimate > 1) cxFixedRemaining = cxFixedRemaining - child_padding_;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cx == 0 ) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
				} 
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else {
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}

			sz.cy = pControl->GetMaxHeight();
			if( sz.cy == 0 ) sz.cy = szAvailable.cy - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy > szControlAvailable.cy ) sz.cy = szControlAvailable.cy;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();

			UINT iChildAlign = GetChildVAlign(); 
			if (iChildAlign == DT_VCENTER) {
				int iPosY = (rc.bottom + rc.top) / 2;
				if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
					iPosY += vertical_scrollbar_->GetScrollRange() / 2;
					iPosY -= vertical_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY - sz.cy/2, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy - sz.cy/2 };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_BOTTOM) {
				int iPosY = rc.bottom;
				if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
					iPosY += vertical_scrollbar_->GetScrollRange();
					iPosY -= vertical_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY - rcPadding.bottom - sz.cy, iPosX + sz.cx + rcPadding.left, iPosY - rcPadding.bottom };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosY = rc.top;
				if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
					iPosY -= vertical_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + sz.cx + rcPadding.left, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}

			iPosX += sz.cx + child_padding_ + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			szRemaining.cx -= sz.cx + child_padding_ + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * child_padding_;

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void HBox::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && !imm_mode_ ) {
			RECT rcSeparator = GetThumbRect(true);
			RenderEngine::DrawColor(hDC, rcSeparator, 0xAA000000);
		}
	}

	void HBox::SetSepWidth(int iWidth)
	{
		sep_width_ = iWidth;
	}

	int HBox::GetSepWidth() const
	{
		return sep_width_;
	}

	void HBox::SetSepImmMode(bool bImmediately)
	{
		if( imm_mode_ == bImmediately ) return;
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && !imm_mode_ && pm_ != NULL ) {
			pm_->RemovePostPaint(this);
		}

		imm_mode_ = bImmediately;
	}

	bool HBox::IsSepImmMode() const
	{
		return imm_mode_;
	}

	void HBox::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();
		if( _tcsicmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcsicmp(pstrValue, _T("true")) == 0);
		else Box::SetAttribute(strName, strValue);
	}

	void HBox::DoEvent(EventArgs& event)
	{
		if( sep_width_ != 0 ) {
			if(event.type_ == kEventMouseButtonDown && IsEnabled()){
				RECT rcSeparator = GetThumbRect(false);
				if( ::PtInRect(&rcSeparator, event.mouse_point_) ) {
					btn_state_ |= UISTATE_CAPTURED;
					last_mouse_pt_ = event.mouse_point_;
					new_pos_ = rect_;
					if( !imm_mode_ && pm_ ) pm_->AddPostPaint(this);
					return;
				}
			}
			if(event.type_ == kEventMouseButtonUp ){
				if( (btn_state_ & UISTATE_CAPTURED) != 0) {
					btn_state_ &= ~UISTATE_CAPTURED;
					rect_ = new_pos_;
					if( !imm_mode_ && pm_ ) pm_->RemovePostPaint(this);
					NeedParentUpdate();
					return;
				}
			}
			if( event.type_ == kEventMouseMove ){
				if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
					LONG cx = event.mouse_point_.x - last_mouse_pt_.x;
					last_mouse_pt_ = event.mouse_point_;
					RECT rc = new_pos_;
					if( sep_width_ >= 0 ) {
						if( cx > 0 && event.mouse_point_.x < new_pos_.right - sep_width_ ) return;
						if( cx < 0 && event.mouse_point_.x > new_pos_.right ) return;
						rc.right += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( new_pos_.right - new_pos_.left <= GetMinWidth() ) return;
							rc.right = rc.left + GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( new_pos_.right - new_pos_.left >= GetMaxWidth() ) return;
							rc.right = rc.left + GetMaxWidth();
						}
					}
					else {
						if( cx > 0 && event.mouse_point_.x < new_pos_.left ) return;
						if( cx < 0 && event.mouse_point_.x > new_pos_.left - sep_width_ ) return;
						rc.left += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( new_pos_.right - new_pos_.left <= GetMinWidth() ) return;
							rc.left = rc.right - GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( new_pos_.right - new_pos_.left >= GetMaxWidth() ) return;
							rc.left = rc.right - GetMaxWidth();
						}
					}

					CDuiRect rcInvalidate = GetThumbRect(true);
					new_pos_ = rc;
					cxy_fired_sz_.cx = new_pos_.right - new_pos_.left;

					if( imm_mode_ ) {
						rect_ = new_pos_;
						NeedParentUpdate();
					}
					else {
						rcInvalidate.Join(GetThumbRect(true));
						rcInvalidate.Join(GetThumbRect(false));
						if( pm_ ) pm_->Invalidate(rcInvalidate);
					}
					return;
				}
			}
			if( event.type_ == kEventSetCursor ){
				RECT rcSeparator = GetThumbRect(false);
				if( IsEnabled() && ::PtInRect(&rcSeparator, event.mouse_point_) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
					return;
				}
			}
		}
		Box::DoEvent(event);
	}

	RECT HBox::GetThumbRect(bool bUseNew) const
	{
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( sep_width_ >= 0 ) return CDuiRect(new_pos_.right - sep_width_, new_pos_.top, new_pos_.right, new_pos_.bottom);
			else return CDuiRect(new_pos_.left, new_pos_.top, new_pos_.left - sep_width_, new_pos_.bottom);
		}
		else {
			if( sep_width_ >= 0 ) return CDuiRect(rect_.right - sep_width_, rect_.top, rect_.right, rect_.bottom);
			else return CDuiRect(rect_.left, rect_.top, rect_.left - sep_width_, rect_.bottom);
		}
	}
}
