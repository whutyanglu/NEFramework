#include "StdAfx.h"
#include "../Core/DlgBuilder.h"

namespace ui
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(Box)

		Box::Box()
		: child_padding_(0),
		child_align_(DT_LEFT),
		child_v_align_(DT_TOP),
		auto_destroy_(true),
		delayed_destroy_(true),
		child_mouse_enabled_(true),
		vertical_scrollbar_(NULL),
		horizontal_scrollbar_(NULL),
		scroll_steop_size_(0)
	{
		::ZeroMemory(&rc_inset_, sizeof(rc_inset_));
	}

	Box::~Box()
	{
		delayed_destroy_ = false;
		RemoveAll();
		if( vertical_scrollbar_ ) {
			delete vertical_scrollbar_;
			vertical_scrollbar_ = NULL;
		}
		if( horizontal_scrollbar_ ) {
			delete horizontal_scrollbar_;
			horizontal_scrollbar_ = NULL;
		}
	}

	std::wstring Box::GetClass() const
	{
		return _T("Box");
	}

	LPVOID Box::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), _T("IBox")) == 0 ) return static_cast<IBox*>(this);
		else if( _tcsicmp(pstrName.c_str(), DUI_CTR_BOX) == 0 ) return static_cast<Box*>(this);
		return Control::GetInterface(pstrName);
	}

	Control* Box::GetItemAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= items_.GetSize() ) return NULL;
		return static_cast<Control*>(items_[iIndex]);
	}

	int Box::GetItemIndex(Control* pControl) const
	{
		for( int it = 0; it < items_.GetSize(); it++ ) {
			if( static_cast<Control*>(items_[it]) == pControl ) {
				return it;
			}
		}

		return -1;
	}

	bool Box::SetItemIndex(Control* pControl, int iIndex)
	{
		for( int it = 0; it < items_.GetSize(); it++ ) {
			if( static_cast<Control*>(items_[it]) == pControl ) {
				NeedUpdate();            
				items_.Remove(it);
				return items_.InsertAt(iIndex, pControl);
			}
		}

		return false;
	}

	int Box::GetCount() const
	{
		return items_.GetSize();
	}

	bool Box::Add(Control* pControl)
	{
		if( pControl == NULL) return false;

		if( pm_ != NULL ) pm_->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return items_.Add(pControl);   
	}

	bool Box::AddAt(Control* pControl, int iIndex)
	{
		if( pControl == NULL) return false;

		if( pm_ != NULL ) pm_->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return items_.InsertAt(iIndex, pControl);
	}

	bool Box::Remove(Control* pControl)
	{
		if( pControl == NULL) return false;

		for( int it = 0; it < items_.GetSize(); it++ ) {
			if( static_cast<Control*>(items_[it]) == pControl ) {
				NeedUpdate();
				if( auto_destroy_ ) {
					if( delayed_destroy_ && pm_ ) pm_->AddDelayedCleanup(pControl);             
					else delete pControl;
				}
				return items_.Remove(it);
			}
		}
		return false;
	}

	bool Box::RemoveAt(int iIndex)
	{
		Control* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return Box::Remove(pControl);
		}

		return false;
	}

	void Box::RemoveAll()
	{
		for( int it = 0; auto_destroy_ && it < items_.GetSize(); it++ ) {
			Control* pItem = static_cast<Control*>(items_[it]);
			if( delayed_destroy_ && pm_ ) {
				pm_->AddDelayedCleanup(pItem);             
			}
			else {
				delete pItem;
				pItem = NULL;
			}
		}
		items_.Empty();
		NeedUpdate();
	}

	int Box::GetFixedWidth()
	{
		int width = 0;
		int tmp_width = 0;
		if (cxy_fired_sz_.cx == -1) {
			for (int i = 0; i < GetCount(); ++i)
			{
				Control *ctl = GetItemAt(i);
				tmp_width = ctl->GetFixedWidth() + ctl->GetPadding().left + ctl->GetPadding().right;
				width = tmp_width > width ? tmp_width : width;
			}

			width += (rc_inset_.right + rc_inset_.left);
			cxy_fired_sz_.cx = width;
		}
		
		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_.cx);
		}

		return cxy_fired_sz_.cx;
	}

	int Box::GetFixedHeight()
	{
		int height = 0;
		int tmp_height = 0;

		if (cxy_fired_sz_.cy == -1) {
			for (int i = 0; i < GetCount(); ++i)
			{
				Control *ctl = GetItemAt(i);
				if (ctl->IsVisible()) {
					tmp_height = ctl->GetFixedHeight() + ctl->GetPadding().top + ctl->GetPadding().bottom;
					height = tmp_height > height ? tmp_height : height;
				}
				
			}

			height += (rc_inset_.bottom + rc_inset_.top);
			cxy_fired_sz_.cy = height;
		}

		if (pm_ != NULL) {
			return pm_->GetDPIObj()->Scale(cxy_fired_sz_.cy);
		}
		return cxy_fired_sz_.cy;
	}

	bool Box::IsAutoDestroy() const
	{
		return auto_destroy_;
	}

	void Box::SetAutoDestroy(bool bAuto)
	{
		auto_destroy_ = bAuto;
	}

	bool Box::IsDelayedDestroy() const
	{
		return delayed_destroy_;
	}

	void Box::SetDelayedDestroy(bool bDelayed)
	{
		delayed_destroy_ = bDelayed;
	}

	RECT Box::GetInset() const
	{
		if(pm_) return pm_->GetDPIObj()->Scale(rc_inset_);
		return rc_inset_;
	}

	void Box::SetInset(RECT rcInset)
	{
		rc_inset_ = rcInset;
		NeedUpdate();
	}

	int Box::GetChildPadding() const
	{
		if (pm_) return pm_->GetDPIObj()->Scale(child_padding_);
		return child_padding_;
	}


	void Box::SetChildPadding(int iPadding)
	{
		child_padding_ = iPadding;
		NeedUpdate();
	}

	UINT Box::GetChildAlign() const
	{
		return child_align_;
	}

	void Box::SetChildAlign(UINT iAlign)
	{
		child_align_ = iAlign;
		NeedUpdate();
	}

	UINT Box::GetChildVAlign() const
	{
		return child_v_align_;
	}

	void Box::SetChildVAlign(UINT iVAlign)
	{
		child_v_align_ = iVAlign;
		NeedUpdate();
	}

	bool Box::IsMouseChildEnabled() const
	{
		return child_mouse_enabled_;
	}

	void Box::SetMouseChildEnabled(bool bEnable)
	{
		child_mouse_enabled_ = bEnable;
	}

	void Box::SetVisible(bool bVisible)
	{
		if( visible_ == bVisible ) return;
		Control::SetVisible(bVisible);
		for( int it = 0; it < items_.GetSize(); it++ ) {
			static_cast<Control*>(items_[it])->SetInternVisible(IsVisible());
		}
	}

	// 逻辑上，对于Container控件不公开此方法
	// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
	void Box::SetInternVisible(bool bVisible)
	{
		Control::SetInternVisible(bVisible);
		if( items_.IsEmpty() ) return;
		for( int it = 0; it < items_.GetSize(); it++ ) {
			// 控制子控件显示状态
			// InternVisible状态应由子控件自己控制
			static_cast<Control*>(items_[it])->SetInternVisible(IsVisible());
		}
	}

	void Box::SetEnabled(bool bEnabled)
	{
		if( enabled_ == bEnabled ) return;

		enabled_ = bEnabled;

		for( int it = 0; it < items_.GetSize(); it++ ) {
			static_cast<Control*>(items_[it])->SetEnabled(enabled_);
		}

		Invalidate();
	}

	void Box::SetMouseEnabled(bool bEnabled)
	{
		if( vertical_scrollbar_ != NULL ) vertical_scrollbar_->SetMouseEnabled(bEnabled);
		if( horizontal_scrollbar_ != NULL ) horizontal_scrollbar_->SetMouseEnabled(bEnabled);
		Control::SetMouseEnabled(bEnabled);
	}

	void Box::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Control::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetFocus ) 
		{
			focused_ = true;
			return;
		}
		if( event.type_ == kEventKillFocus ) 
		{
			focused_ = false;
			return;
		}
		if( vertical_scrollbar_ != NULL && vertical_scrollbar_->IsVisible() && vertical_scrollbar_->IsEnabled() )
		{
			if( event.type_ == kEventKeyDown ) 
			{
				switch( event.key_ ) {
				case VK_DOWN:
					LineDown();
					return;
				case VK_UP:
					LineUp();
					return;
				case VK_NEXT:
					PageDown();
					return;
				case VK_PRIOR:
					PageUp();
					return;
				case VK_HOME:
					HomeUp();
					return;
				case VK_END:
					EndDown();
					return;
				}
			}
			else if( event.type_ == kMouseScrollWheel )
			{
				switch( LOWORD(event.w_param_) ) {
				case SB_LINEUP:
					LineUp();
					return;
				case SB_LINEDOWN:
					LineDown();
					return;
				}
			}
		}
		if( horizontal_scrollbar_ != NULL && horizontal_scrollbar_->IsVisible() && horizontal_scrollbar_->IsEnabled() ) {
			if( event.type_ == kEventKeyDown ) 
			{
				switch( event.key_ ) {
				case VK_DOWN:
					LineRight();
					return;
				case VK_UP:
					LineLeft();
					return;
				case VK_NEXT:
					PageRight();
					return;
				case VK_PRIOR:
					PageLeft();
					return;
				case VK_HOME:
					HomeLeft();
					return;
				case VK_END:
					EndRight();
					return;
				}
			}
			else if( event.type_ == kMouseScrollWheel )
			{
				switch( LOWORD(event.w_param_) ) {
				case SB_LINEUP:
					LineLeft();
					return;
				case SB_LINEDOWN:
					LineRight();
					return;
				}
			}
		}
		Control::DoEvent(event);
	}

	SIZE Box::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) sz.cy = vertical_scrollbar_->GetScrollPos();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) sz.cx = horizontal_scrollbar_->GetScrollPos();
		return sz;
	}

	SIZE Box::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) sz.cy = vertical_scrollbar_->GetScrollRange();
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) sz.cx = horizontal_scrollbar_->GetScrollRange();
		return sz;
	}

	void Box::SetScrollPos(SIZE szPos, bool bMsg)
	{
		int cx = 0;
		int cy = 0;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			int iLastScrollPos = vertical_scrollbar_->GetScrollPos();
			vertical_scrollbar_->SetScrollPos(szPos.cy);
			cy = vertical_scrollbar_->GetScrollPos() - iLastScrollPos;
		}

		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			int iLastScrollPos = horizontal_scrollbar_->GetScrollPos();
			horizontal_scrollbar_->SetScrollPos(szPos.cx);
			cx = horizontal_scrollbar_->GetScrollPos() - iLastScrollPos;
		}

		if( cx == 0 && cy == 0 ) return;

		RECT rcPos;
		for( int it2 = 0; it2 < items_.GetSize(); it2++ ) {
			Control* pControl = static_cast<Control*>(items_[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos);
		}

		Invalidate();

		if(vertical_scrollbar_)
		{
			// 发送滚动消息
			if( pm_ != NULL && bMsg ) {
				int nPage = (vertical_scrollbar_->GetScrollPos() + vertical_scrollbar_->GetLineSize()) / vertical_scrollbar_->GetLineSize();
				//pm_->SendNotify(this, DUI_MSGTYPE_SCROLL, (WPARAM)nPage);
				Notify(kEventScrollChange, (WPARAM)nPage);
			}
		}
	}

	void Box::SetScrollStepSize(int nSize)
	{
		if (nSize >0)
			scroll_steop_size_ = nSize;
	}

	int Box::GetScrollStepSize() const
	{
		if(pm_ )return pm_->GetDPIObj()->Scale(scroll_steop_size_);

		return scroll_steop_size_;
	}

	void Box::LineUp()
	{
		int cyLine = GetScrollStepSize();
		if (cyLine == 0) {
			cyLine = 8;
			if( pm_ ) cyLine = GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 8;
		}

		SIZE sz = GetScrollPos();
		sz.cy -= cyLine;
		SetScrollPos(sz);
	}

	void Box::LineDown()
	{
		int cyLine = GetScrollStepSize();
		if (cyLine == 0) {
			cyLine = 8;
			if( pm_ ) cyLine = GlobalManager::GetDefaultFontInfo(pm_->GetPaintDC())->tm_.tmHeight + 8;
		}

		SIZE sz = GetScrollPos();
		sz.cy += cyLine;
		SetScrollPos(sz);
	}

	void Box::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = rect_.bottom - rect_.top - rc_inset_.top - rc_inset_.bottom;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) iOffset -= horizontal_scrollbar_->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void Box::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = rect_.bottom - rect_.top - rc_inset_.top - rc_inset_.bottom;
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) iOffset -= horizontal_scrollbar_->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void Box::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void Box::EndDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	void Box::LineLeft()
	{
		int nScrollStepSize = GetScrollStepSize();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos();
		sz.cx -= cxLine;
		SetScrollPos(sz);
	}

	void Box::LineRight()
	{
		int nScrollStepSize = GetScrollStepSize();
		int cxLine = nScrollStepSize == 0 ? 8 : nScrollStepSize;

		SIZE sz = GetScrollPos();
		sz.cx += cxLine;
		SetScrollPos(sz);
	}

	void Box::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = rect_.right - rect_.left - rc_inset_.left - rc_inset_.right;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) iOffset -= vertical_scrollbar_->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void Box::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = rect_.right - rect_.left - rc_inset_.left - rc_inset_.right;
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) iOffset -= vertical_scrollbar_->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void Box::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void Box::EndRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void Box::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if( bEnableVertical && !vertical_scrollbar_ ) {
			vertical_scrollbar_ = new ScrollBar;
			vertical_scrollbar_->SetOwner(this);
			vertical_scrollbar_->SetManager(pm_, NULL, false);
			if ( pm_ ) {
				std::wstring pDefaultAttributes = pm_->GetDefaultAttributeList(_T("VScrollBar"));
				if( !pDefaultAttributes.empty() ) {
					vertical_scrollbar_->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableVertical && vertical_scrollbar_ ) {
			delete vertical_scrollbar_;
			vertical_scrollbar_ = NULL;
		}

		if( bEnableHorizontal && !horizontal_scrollbar_ ) {
			horizontal_scrollbar_ = new ScrollBar;
			horizontal_scrollbar_->SetHorizontal(true);
			horizontal_scrollbar_->SetOwner(this);
			horizontal_scrollbar_->SetManager(pm_, NULL, false);

			if ( pm_ ) {
				std::wstring pDefaultAttributes = pm_->GetDefaultAttributeList(_T("HScrollBar"));
				if( !pDefaultAttributes.empty()) {
					horizontal_scrollbar_->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableHorizontal && horizontal_scrollbar_ ) {
			delete horizontal_scrollbar_;
			horizontal_scrollbar_ = NULL;
		}

		NeedUpdate();
	}

	ScrollBar* Box::GetVerticalScrollBar() const
	{
		return vertical_scrollbar_;
	}

	ScrollBar* Box::GetHorizontalScrollBar() const
	{
		return horizontal_scrollbar_;
	}

	int Box::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if( GetCount() == 0 ) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if( bForward ) {
			for( int i = iIndex; i < GetCount(); i++ ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return -1;
		}
		else {
			for( int i = iIndex; i >= 0; --i ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return FindSelectable(0, true);
		}
	}

	RECT Box::GetClientPos() const
	{
		RECT rc = rect_;
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			rc.top -= vertical_scrollbar_->GetScrollPos();
			rc.bottom -= vertical_scrollbar_->GetScrollPos();
			rc.bottom += vertical_scrollbar_->GetScrollRange();
			rc.right -= vertical_scrollbar_->GetFixedWidth();
		}
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			rc.left -= horizontal_scrollbar_->GetScrollPos();
			rc.right -= horizontal_scrollbar_->GetScrollPos();
			rc.right += horizontal_scrollbar_->GetScrollRange();
			rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
		}
		return rc;
	}

	void Box::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		Control::Move(szOffset, bNeedInvalidate);
		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) vertical_scrollbar_->Move(szOffset, false);
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) horizontal_scrollbar_->Move(szOffset, false);
		for( int it = 0; it < items_.GetSize(); it++ ) {
			Control* pControl = static_cast<Control*>(items_[it]);
			if( pControl != NULL && pControl->IsVisible() ) pControl->Move(szOffset, false);
		}
	}

	void Box::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		if( items_.IsEmpty() ) return;

		rc = rect_;
		rc.left += rc_inset_.left;
		rc.top += rc_inset_.top;
		rc.right -= rc_inset_.right;
		rc.bottom -= rc_inset_.bottom;

		if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) {
			rc.top -= vertical_scrollbar_->GetScrollPos();
			rc.bottom -= vertical_scrollbar_->GetScrollPos();
			rc.bottom += vertical_scrollbar_->GetScrollRange();
			rc.right -= vertical_scrollbar_->GetFixedWidth();
		}
		if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) {
			rc.left -= horizontal_scrollbar_->GetScrollPos();
			rc.right -= horizontal_scrollbar_->GetScrollPos();
			rc.right += horizontal_scrollbar_->GetScrollRange();
			rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
		}

		for( int it = 0; it < items_.GetSize(); it++ ) {
			Control* pControl = static_cast<Control*>(items_[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it);
			}
			else { 
				SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

				int width = pControl->GetFixedWidth();
				width = width > sz.cx ? sz.cx : width;
				int height = pControl->GetFixedHeight();
				height = height > sz.cy ? sz.cy : height;

				RECT rcCtrl = { rc.left, rc.top, rc.left + width, rc.top + height };
				pControl->SetPos(rcCtrl, false);
			}
		}
	}

	void Box::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("inset")) == 0 ) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetInset(rcInset);
		}
		else if( _tcsicmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("vscrollbar")) == 0 ) {
			EnableScrollBar(_tcsicmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		}
		else if( _tcsicmp(pstrName, _T("vscrollbarclass")) == 0 ) {
			vscrollbar_class_ = pstrValue;
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetVerticalScrollBar() ) {
				std::wstring cls = GlobalManager::GetClass(vscrollbar_class_);
				if( !cls.empty() ) {
					GetVerticalScrollBar()->ApplyAttributeList(cls);
				}
				else {
					GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("hscrollbar")) == 0 ) {
			EnableScrollBar(GetVerticalScrollBar() != NULL, _tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("hscrollbarclass")) == 0 ) {
			hscorllbar_class_ = pstrValue;
			EnableScrollBar(TRUE, GetHorizontalScrollBar() != NULL);
			if( GetHorizontalScrollBar() ) {
				std::wstring cls = GlobalManager::GetClass(hscorllbar_class_);
				if(!cls.empty()) {
					GetHorizontalScrollBar()->ApplyAttributeList(cls);
				}
				else {
					GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
				}
			}
		}
		else if( _tcsicmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("childalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("left")) == 0 ) child_align_ = DT_LEFT;
			else if( _tcscmp(pstrValue, _T("center")) == 0 ) child_align_ = DT_CENTER;
			else if( _tcscmp(pstrValue, _T("right")) == 0 ) child_align_ = DT_RIGHT;
		}
		else if( _tcscmp(pstrName, _T("childvalign")) == 0 ) {
			if( _tcscmp(pstrValue, _T("top")) == 0 ) child_v_align_ = DT_TOP;
			else if( _tcscmp(pstrValue, _T("vcenter")) == 0 ) child_v_align_ = DT_VCENTER;
			else if( _tcscmp(pstrValue, _T("bottom")) == 0 ) child_v_align_ = DT_BOTTOM;
		}
		else if( _tcsicmp(pstrName, _T("scrollstepsize")) == 0 ) SetScrollStepSize(_ttoi(pstrValue));
		else Control::SetAttribute(pstrName, pstrValue);
	}

	void Box::SetManager(PaintManager* pManager, Control* pParent, bool bInit)
	{
		for( int it = 0; it < items_.GetSize(); it++ ) {
			static_cast<Control*>(items_[it])->SetManager(pManager, this, bInit);
		}

		if( vertical_scrollbar_ != NULL ) vertical_scrollbar_->SetManager(pManager, this, bInit);
		if( horizontal_scrollbar_ != NULL ) horizontal_scrollbar_->SetManager(pManager, this, bInit);
		Control::SetManager(pManager, pParent, bInit);
	}

	Control* Box::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		if( (uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&rect_, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( (uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL ) return NULL;

		Control* pResult = NULL;
		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		if( pResult == NULL && vertical_scrollbar_ != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = vertical_scrollbar_->FindControl(Proc, pData, uFlags);
		}
		if( pResult == NULL && horizontal_scrollbar_ != NULL ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = horizontal_scrollbar_->FindControl(Proc, pData, uFlags);
		}
		if( pResult != NULL ) return pResult;

		if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled() ) {
			RECT rc = rect_;
			rc.left += rc_inset_.left;
			rc.top += rc_inset_.top;
			rc.right -= rc_inset_.right;
			rc.bottom -= rc_inset_.bottom;
			if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
			if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();
			if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
				for( int it = items_.GetSize() - 1; it >= 0; it-- ) {
					pResult = static_cast<Control*>(items_[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					}          
				}
			}
			else {
				for( int it = 0; it < items_.GetSize(); it++ ) {
					pResult = static_cast<Control*>(items_[it])->FindControl(Proc, pData, uFlags);
					if( pResult != NULL ) {
						if( (uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
							continue;
						else 
							return pResult;
					} 
				}
			}
		}

		pResult = NULL;
		if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) {
			if( (uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled() ) pResult = Proc(this, pData);
		}
		return pResult;
	}

	void Box::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &rect_) ) return;

		RenderClip clip;
		RenderClip::GenerateClip(hDC, rcTemp, clip);
		Control::DoPaint(hDC, rcPaint);

		if( items_.GetSize() > 0 ) {
			RECT rcInset = GetInset();
			RECT rc = rect_;
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			if( vertical_scrollbar_ && vertical_scrollbar_->IsVisible() ) rc.right -= vertical_scrollbar_->GetFixedWidth();
			if( horizontal_scrollbar_ && horizontal_scrollbar_->IsVisible() ) rc.bottom -= horizontal_scrollbar_->GetFixedHeight();

			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				for( int it = 0; it < items_.GetSize(); it++ ) {
					Control* pControl = static_cast<Control*>(items_[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &rect_, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
			else {
				RenderClip childClip;
				RenderClip::GenerateClip(hDC, rcTemp, childClip);
				for( int it = 0; it < items_.GetSize(); it++ ) {
					Control* pControl = static_cast<Control*>(items_[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					if( pControl ->IsFloat() ) {
						if( !::IntersectRect(&rcTemp, &rect_, &pControl->GetPos()) ) continue;
						RenderClip::UseOldClipBegin(hDC, childClip);
						pControl->DoPaint(hDC, rcPaint);
						RenderClip::UseOldClipEnd(hDC, childClip);
					}
					else {
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						pControl->DoPaint(hDC, rcPaint);
					}
				}
			}
		}

		if( vertical_scrollbar_ != NULL && vertical_scrollbar_->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &vertical_scrollbar_->GetPos()) ) {
				vertical_scrollbar_->DoPaint(hDC, rcPaint);
			}
		}

		if( horizontal_scrollbar_ != NULL && horizontal_scrollbar_->IsVisible() ) {
			if( ::IntersectRect(&rcTemp, &rcPaint, &horizontal_scrollbar_->GetPos()) ) {
				horizontal_scrollbar_->DoPaint(hDC, rcPaint);
			}
		}
	}

	void Box::SetFloatPos(int iIndex)
	{
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if( iIndex < 0 || iIndex >= items_.GetSize() ) return;

		Control* pControl = static_cast<Control*>(items_[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};

		int nParentWidth = rect_.right - rect_.left;
		int nParentHeight = rect_.bottom - rect_.top;

		UINT uAlign = pControl->GetFloatAlign();
		if(uAlign != 0) {
			RECT rcCtrl = {0, 0, sz.cx, sz.cy};
			if((uAlign & DT_CENTER) != 0) {
				::OffsetRect(&rcCtrl, (nParentWidth - sz.cx) / 2, 0);
			}
			else if((uAlign & DT_RIGHT) != 0) {
				::OffsetRect(&rcCtrl, nParentWidth - sz.cx, 0);
			}
			else {
				::OffsetRect(&rcCtrl, szXY.cx, 0);
			}

			if((uAlign & DT_VCENTER) != 0) {
				::OffsetRect(&rcCtrl, 0, (nParentHeight - sz.cy) / 2);
			}
			else if((uAlign & DT_BOTTOM) != 0) {
				::OffsetRect(&rcCtrl, 0, nParentHeight - sz.cy);
			}
			else {
				::OffsetRect(&rcCtrl, 0, szXY.cy);
			}

			::OffsetRect(&rcCtrl, rect_.left, rect_.top);
			pControl->SetPos(rcCtrl, false);
		}
		else {
			TPercentInfo rcPercent = pControl->GetFloatPercent();
			LONG width = rect_.right - rect_.left;
			LONG height = rect_.bottom - rect_.top;
			RECT rcCtrl = { 0 };
			rcCtrl.left = (LONG)(width*rcPercent.left) + szXY.cx+ rect_.left;
			rcCtrl.top = (LONG)(height*rcPercent.top) + szXY.cy+ rect_.top;
			rcCtrl.right = (LONG)(width*rcPercent.right) + szXY.cx + sz.cx+ rect_.left;
			rcCtrl.bottom = (LONG)(height*rcPercent.bottom) + szXY.cy + sz.cy+ rect_.top;
			pControl->SetPos(rcCtrl, false);
		}
	}

	void Box::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		// by 冰下海 2015/08/16
		while (horizontal_scrollbar_)
		{
			// Scroll needed
			if (cxRequired > rc.right - rc.left && !horizontal_scrollbar_->IsVisible())
			{
				horizontal_scrollbar_->SetVisible(true);
				horizontal_scrollbar_->SetScrollRange(cxRequired - (rc.right - rc.left));
				horizontal_scrollbar_->SetScrollPos(0);
				SetPos(rect_);
				break;
			}

			// No scrollbar required
			if (!horizontal_scrollbar_->IsVisible()) break;

			// Scroll not needed anymore?
			int cxScroll = cxRequired - (rc.right - rc.left);
			if (cxScroll <= 0)
			{
				horizontal_scrollbar_->SetVisible(false);
				horizontal_scrollbar_->SetScrollPos(0);
				horizontal_scrollbar_->SetScrollRange(0);
				SetPos(rect_);
			}
			else
			{
				RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + horizontal_scrollbar_->GetFixedHeight() };
				horizontal_scrollbar_->SetPos(rcScrollBarPos);

				if (horizontal_scrollbar_->GetScrollRange() != cxScroll) 
				{
					int iScrollPos = horizontal_scrollbar_->GetScrollPos();
					horizontal_scrollbar_->SetScrollRange(::abs(cxScroll)); // if scrollpos>range then scrollpos=range
					if(iScrollPos > horizontal_scrollbar_->GetScrollPos()) 
					{
						SetPos(rect_);
					}
				}
			}
			break;
		}

		while (vertical_scrollbar_)
		{
			// Scroll needed
			if (cyRequired > rc.bottom - rc.top && !vertical_scrollbar_->IsVisible()) 
			{
				vertical_scrollbar_->SetVisible(true);
				vertical_scrollbar_->SetScrollRange(cyRequired - (rc.bottom - rc.top));
				vertical_scrollbar_->SetScrollPos(0);
				SetPos(rect_);
				break;
			}

			// No scrollbar required
			if (!vertical_scrollbar_->IsVisible()) break;

			// Scroll not needed anymore?
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if (cyScroll <= 0) 
			{
				vertical_scrollbar_->SetVisible(false);
				vertical_scrollbar_->SetScrollPos(0);
				vertical_scrollbar_->SetScrollRange(0);
				SetPos(rect_);
				break;
			}

			RECT rcScrollBarPos = { rc.right, rc.top, rc.right + vertical_scrollbar_->GetFixedWidth(), rc.bottom };
			vertical_scrollbar_->SetPos(rcScrollBarPos);

			if (vertical_scrollbar_->GetScrollRange() != cyScroll)
			{
				int iScrollPos = vertical_scrollbar_->GetScrollPos();
				vertical_scrollbar_->SetScrollRange(::abs(cyScroll)); // if scrollpos>range then scrollpos=range
				if(iScrollPos > vertical_scrollbar_->GetScrollPos()) 
				{
					SetPos(rect_);
				}
			}
			break;
		}
	}

	bool Box::SetSubControlText( std::wstring pstrSubControlName,std::wstring pstrText )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetText(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool Box::SetSubControlFixedHeight( std::wstring pstrSubControlName,int cy )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedHeight(cy);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool Box::SetSubControlFixedWdith( std::wstring pstrSubControlName,int cx )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedWidth(cx);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool Box::SetSubControlUserData( std::wstring pstrSubControlName,std::wstring pstrText )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetUserData(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	std::wstring Box::GetSubControlText( std::wstring pstrSubControlName )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetText();
	}

	int Box::GetSubControlFixedHeight( std::wstring pstrSubControlName )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedHeight();
	}

	int Box::GetSubControlFixedWdith( std::wstring pstrSubControlName )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedWidth();
	}

	const std::wstring Box::GetSubControlUserData( std::wstring pstrSubControlName )
	{
		Control* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetUserData();
	}

	Control* Box::FindSubControl( std::wstring pstrSubControlName )
	{
		Control* pSubControl=NULL;
		if(pm_ != NULL) pSubControl = static_cast<Control*>(pm_->FindSubControlByName(this,pstrSubControlName));
		return pSubControl;
	}

} // namespace DuiLib
