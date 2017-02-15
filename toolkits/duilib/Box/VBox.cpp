#include "StdAfx.h"
#include "VBox.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(VBox)
	VBox::VBox() : sep_height_(0), btn_state_(0), imm_mode_(false)
	{
		last_mouse_pt_.x = last_mouse_pt_.y = 0;
		::ZeroMemory(&new_pos_, sizeof(new_pos_));
	}

	std::wstring VBox::GetClass() const
	{
		return _T("VBox");
	}

	LPVOID VBox::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_VBOX) == 0 ) return static_cast<VBox*>(this);
		return Box::GetInterface(pstrName);
	}

	UINT VBox::GetControlFlags() const
	{
		if( IsEnabled() && sep_height_ != 0 ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	void VBox::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		rc = rect_;

		// Adjust for inset
		RECT m_rcInset = VBox::rc_inset_;
		PaintManager *pm = GetManager();
		if (pm) {
			CDPI *dpi = GetManager()->GetDPIObj();
			if (dpi) {
				dpi->Scale(&m_rcInset);
			}
		}
		
		
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();

		if( items_.GetSize() == 0) {
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) 
			szAvailable.cx += horizontal_scrollbar_->GetScrollRange();
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) 
			szAvailable.cy += vertical_scrollbar_->GetScrollRange();

		int cxNeeded = 0;
		int nAdjustables = 0;
		int cyFixed = 0;
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
			szControlAvailable.cx -= rcPadding.left + rcPadding.right;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				nAdjustables++;
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			cyFixed += sz.cy + pControl->GetPadding().top + pControl->GetPadding().bottom;

			sz.cx = MAX(sz.cx, 0);
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			cxNeeded = MAX(cxNeeded, sz.cx + rcPadding.left + rcPadding.right);
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * child_padding_;

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			iPosY -= vertical_scrollbar_->GetScrollPos();
		}

		int iEstimate = 0;
		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it2);
				continue;
			}

			iEstimate += 1;
			RECT rcPadding = pControl->GetPadding();
			szRemaining.cy -= rcPadding.top;

			szControlAvailable = szRemaining;
			szControlAvailable.cx -= rcPadding.left + rcPadding.right;
			iControlMaxWidth = pControl->GetFixedWidth();
			iControlMaxHeight = pControl->GetFixedHeight();
			if (iControlMaxWidth <= 0) iControlMaxWidth = pControl->GetMaxWidth(); 
			if (iControlMaxHeight <= 0) iControlMaxHeight = pControl->GetMaxHeight();
			if (szControlAvailable.cx > iControlMaxWidth) szControlAvailable.cx = iControlMaxWidth;
			if (szControlAvailable.cy > iControlMaxHeight) szControlAvailable.cy = iControlMaxHeight;
      cyFixedRemaining = cyFixedRemaining - (rcPadding.top + rcPadding.bottom);
			if (iEstimate > 1) cyFixedRemaining = cyFixedRemaining - child_padding_;
			SIZE sz = pControl->EstimateSize(szControlAvailable);
			if( sz.cy == 0 ) {
				iAdjustable++;
				sz.cy = cyExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if( iAdjustable == nAdjustables ) {
					sz.cy = MAX(0, szRemaining.cy - rcPadding.bottom - cyFixedRemaining);
				} 
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else {
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}

			sz.cx = pControl->GetMaxWidth();
			if( sz.cx == 0 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx > szControlAvailable.cx ) sz.cx = szControlAvailable.cx;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();

			UINT iChildAlign = GetChildAlign(); 
			if (iChildAlign == DT_CENTER) {
				int iPosX = (rc.right + rc.left) / 2;
				if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
					iPosX += horizontal_scrollbar_->GetScrollRange() / 2;
					iPosX -= horizontal_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX - sz.cx/2, iPosY + rcPadding.top, iPosX + sz.cx - sz.cx/2, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}
			else if (iChildAlign == DT_RIGHT) {
				int iPosX = rc.right;
				if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
					iPosX += horizontal_scrollbar_->GetScrollRange();
					iPosX -= horizontal_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX - rcPadding.right - sz.cx, iPosY + rcPadding.top, iPosX - rcPadding.right, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}
			else {
				int iPosX = rc.left;
				if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
					iPosX -= horizontal_scrollbar_->GetScrollPos();
				}
				RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top };
				pControl->SetPos(rcCtrl, false);
			}

			iPosY += sz.cy + child_padding_ + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;
			szRemaining.cy -= sz.cy + child_padding_ + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * child_padding_;

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void VBox::DoPostPaint(HDC hDC, const RECT& rcPaint)
	{
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && !imm_mode_ ) {
			RECT rcSeparator = GetThumbRect(true);
			RenderEngine::DrawColor(hDC, rcSeparator, 0xAA000000);
		}
	}

	int VBox::GetFixedHeight()
	{
		int height = cxy_fired_sz_.cy;
		if (height == -1) {
			int visible_count = 0;
			for (int i = 0; i < GetCount(); ++i)
			{
				Control *ctl = GetItemAt(i);
				if (ctl->IsVisible()) {
					++visible_count;
					height += ctl->GetFixedHeight();
					height += (ctl->GetPadding().top + ctl->GetPadding().bottom);
					height += child_padding_;
				}
			}

			if (visible_count > 1) {
				height -= child_padding_;
			}

			height += (rc_inset_.bottom - rc_inset_.top);
			//cxy_fired_sz_.cy = height;
		}

		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(height);
		}
		return height;
	}

	void VBox::SetSepHeight(int iHeight)
	{
		sep_height_ = iHeight;
	}

	int VBox::GetSepHeight() const
	{
		return sep_height_;
	}

	void VBox::SetSepImmMode(bool bImmediately)
	{
		if( imm_mode_ == bImmediately ) return;
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && !imm_mode_ && pm_ != NULL ) {
			pm_->RemovePostPaint(this);
		}

		imm_mode_ = bImmediately;
	}

	bool VBox::IsSepImmMode() const
	{
		return imm_mode_;
	}

	void VBox::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();
		if( _tcsicmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcsicmp(pstrValue, _T("true")) == 0);
		else Box::SetAttribute(pstrName, pstrValue);
	}

	void VBox::DoEvent(EventArgs& event)
	{
		if( sep_height_ != 0 ) {
			if( event.type_ == kEventMouseButtonDown && IsEnabled() )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( ::PtInRect(&rcSeparator, event.mouse_point_) ) {
					btn_state_ |= UISTATE_CAPTURED;
					last_mouse_pt_ = event.mouse_point_;
					new_pos_ = rect_;
					if( !imm_mode_ && pm_ ) pm_->AddPostPaint(this);
					return;
				}
			}
			if( event.type_ == kEventMouseButtonUp )
			{
				if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
					btn_state_ &= ~UISTATE_CAPTURED;
					rect_ = new_pos_;
					if( !imm_mode_ && pm_ ) pm_->RemovePostPaint(this);
					NeedParentUpdate();
					return;
				}
			}
			if( event.type_ == kEventMouseMove )
			{
				if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
					LONG cy = event.mouse_point_.y - last_mouse_pt_.y;
					last_mouse_pt_ = event.mouse_point_;
					RECT rc = new_pos_;
					if( sep_height_ >= 0 ) {
						if( cy > 0 && event.mouse_point_.y < new_pos_.bottom + sep_height_ ) return;
						if( cy < 0 && event.mouse_point_.y > new_pos_.bottom ) return;
						rc.bottom += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( new_pos_.bottom - new_pos_.top <= GetMinHeight() ) return;
							rc.bottom = rc.top + GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( new_pos_.bottom - new_pos_.top >= GetMaxHeight() ) return;
							rc.bottom = rc.top + GetMaxHeight();
						}
					}
					else {
						if( cy > 0 && event.mouse_point_.y < new_pos_.top ) return;
						if( cy < 0 && event.mouse_point_.y > new_pos_.top + sep_height_ ) return;
						rc.top += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( new_pos_.bottom - new_pos_.top <= GetMinHeight() ) return;
							rc.top = rc.bottom - GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( new_pos_.bottom - new_pos_.top >= GetMaxHeight() ) return;
							rc.top = rc.bottom - GetMaxHeight();
						}
					}

					CDuiRect rcInvalidate = GetThumbRect(true);
					new_pos_ = rc;
					cxy_fired_sz_.cy = new_pos_.bottom - new_pos_.top;

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
			if( event.type_ == kEventSetCursor )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( IsEnabled() && ::PtInRect(&rcSeparator, event.mouse_point_) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					return;
				}
			}
		}
		Box::DoEvent(event);
	}

	RECT VBox::GetThumbRect(bool bUseNew) const
	{
		if( (btn_state_ & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( sep_height_ >= 0 ) 
				return CDuiRect(new_pos_.left, MAX(new_pos_.bottom - sep_height_, new_pos_.top), 
				new_pos_.right, new_pos_.bottom);
			else 
				return CDuiRect(new_pos_.left, new_pos_.top, new_pos_.right, 
				MIN(new_pos_.top - sep_height_, new_pos_.bottom));
		}
		else {
			if( sep_height_ >= 0 ) 
				return CDuiRect(rect_.left, MAX(rect_.bottom - sep_height_, rect_.top), rect_.right, 
				rect_.bottom);
			else 
				return CDuiRect(rect_.left, rect_.top, rect_.right, 
				MIN(rect_.top - sep_height_, rect_.bottom));

		}
	}
}
