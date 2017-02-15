#include "StdAfx.h"
#include "ScrollBar.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(ScrollBar)

	ScrollBar::ScrollBar() : m_bHorizontal(false), m_nRange(100), m_nScrollPos(0), m_nLineSize(8), 
		owner_(NULL), m_nLastScrollPos(0), m_nLastScrollOffset(0), m_nScrollRepeatDelay(0), m_uButton1State(0), \
		m_uButton2State(0), m_uThumbState(0), m_bShowButton1(true), m_bShowButton2(true)
	{
		cxy_fired_sz_.cx = DEFAULT_SCROLLBAR_SIZE;
		last_mouse_pt_.x = last_mouse_pt_.y = 0;
		::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
		::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
	}

	std::wstring ScrollBar::GetClass() const
	{
		return DUI_CTR_SCROLLBAR;
	}

	LPVOID ScrollBar::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_SCROLLBAR) == 0 ) return static_cast<ScrollBar*>(this);
		return Control::GetInterface(pstrName);
	}

	void ScrollBar::AttachScrollChange(const EventCallback & cb)
	{
		on_event_[kEventScrollChange] += cb;
	}

	void ScrollBar::DoInit()
	{
		if (IsHorizontal()) {
			std::wstring cls = GlobalManager::GetClass(L"hscrollbar");
			if (!cls.empty()) {
				ApplyAttributeList(cls);
			}
		}
		else {
			std::wstring cls = GlobalManager::GetClass(L"vscrollbar");
			if (!cls.empty()) {
				ApplyAttributeList(cls);
			}
		}
	}

	Box* ScrollBar::GetOwner() const
	{
		return owner_;
	}

	void ScrollBar::SetOwner(Box* pOwner)
	{
		owner_ = pOwner;
	}

	void ScrollBar::SetVisible(bool bVisible)
	{
		if( visible_ == bVisible ) return;

		bool v = IsVisible();
		visible_ = bVisible;
		if( focused_ ) focused_ = false;

	}

	void ScrollBar::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButton1State = 0;
			m_uButton2State = 0;
			m_uThumbState = 0;
		}
	}

	void ScrollBar::SetFocus()
	{
		if( owner_ != NULL ) owner_->SetFocus();
		else Control::SetFocus();
	}

	bool ScrollBar::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void ScrollBar::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		if( m_bHorizontal ) {
			if( cxy_fired_sz_.cy == 0 ) {
				cxy_fired_sz_.cx = 0;
				cxy_fired_sz_.cy = DEFAULT_SCROLLBAR_SIZE;
			}
		}
		else {
			if( cxy_fired_sz_.cx == 0 ) {
				cxy_fired_sz_.cx = DEFAULT_SCROLLBAR_SIZE;
				cxy_fired_sz_.cy = 0;
			}
		}

		if( owner_ != NULL ) owner_->NeedUpdate(); else NeedParentUpdate();
	}

	int ScrollBar::GetScrollRange() const
	{
		return m_nRange;
	}

	void ScrollBar::SetScrollRange(int nRange)
	{
		if( m_nRange == nRange ) return;

		m_nRange = nRange;
		if( m_nRange < 0 ) m_nRange = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(rect_);
	}

	int ScrollBar::GetScrollPos() const
	{
		return m_nScrollPos;
	}

	void ScrollBar::SetScrollPos(int nPos)
	{
		if( m_nScrollPos == nPos ) return;

		m_nScrollPos = nPos;
		if( m_nScrollPos < 0 ) m_nScrollPos = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(rect_);
	}

	int ScrollBar::GetLineSize() const
	{
		return m_nLineSize;
	}

	void ScrollBar::SetLineSize(int nSize)
	{
		m_nLineSize = nSize;
	}

	bool ScrollBar::GetShowButton1()
	{
		return m_bShowButton1;
	}

	void ScrollBar::SetShowButton1(bool bShow)
	{
		m_bShowButton1 = bShow;
		SetPos(rect_);
	}

	std::wstring ScrollBar::GetButton1NormalImage()
	{
		return m_sButton1NormalImage;
	}

	void ScrollBar::SetButton1NormalImage(std::wstring pStrImage)
	{
		m_sButton1NormalImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton1HotImage()
	{
		return m_sButton1HotImage.c_str();
	}

	void ScrollBar::SetButton1HotImage(std::wstring pStrImage)
	{
		m_sButton1HotImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton1PushedImage()
	{
		return m_sButton1PushedImage;
	}

	void ScrollBar::SetButton1PushedImage(std::wstring pStrImage)
	{
		m_sButton1PushedImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton1DisabledImage()
	{
		return m_sButton1DisabledImage;
	}

	void ScrollBar::SetButton1DisabledImage(std::wstring pStrImage)
	{
		m_sButton1DisabledImage = pStrImage;
		Invalidate();
	}

	bool ScrollBar::GetShowButton2()
	{
		return m_bShowButton2;
	}

	void ScrollBar::SetShowButton2(bool bShow)
	{
		m_bShowButton2 = bShow;
		SetPos(rect_);
	}

	std::wstring ScrollBar::GetButton2NormalImage()
	{
		return m_sButton2NormalImage;
	}

	void ScrollBar::SetButton2NormalImage(std::wstring pStrImage)
	{
		m_sButton2NormalImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton2HotImage()
	{
		return m_sButton2HotImage;
	}

	void ScrollBar::SetButton2HotImage(std::wstring pStrImage)
	{
		m_sButton2HotImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton2PushedImage()
	{
		return m_sButton2PushedImage;
	}

	void ScrollBar::SetButton2PushedImage(std::wstring pStrImage)
	{
		m_sButton2PushedImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetButton2DisabledImage()
	{
		return m_sButton2DisabledImage;
	}

	void ScrollBar::SetButton2DisabledImage(std::wstring pStrImage)
	{
		m_sButton2DisabledImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetThumbNormalImage()
	{
		return m_sThumbNormalImage;
	}

	void ScrollBar::SetThumbNormalImage(std::wstring pStrImage)
	{
		m_sThumbNormalImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetThumbHotImage()
	{
		return thumb_hot_image_;
	}

	void ScrollBar::SetThumbHotImage(std::wstring pStrImage)
	{
		thumb_hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetThumbPushedImage()
	{
		return thumb_pushed_image_;
	}

	void ScrollBar::SetThumbPushedImage(std::wstring pStrImage)
	{
		thumb_pushed_image_ = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetThumbDisabledImage()
	{
		return m_sThumbDisabledImage;
	}

	void ScrollBar::SetThumbDisabledImage(std::wstring pStrImage)
	{
		m_sThumbDisabledImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetRailNormalImage()
	{
		return m_sRailNormalImage;
	}

	void ScrollBar::SetRailNormalImage(std::wstring pStrImage)
	{
		m_sRailNormalImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetRailHotImage()
	{
		return m_sRailHotImage;
	}

	void ScrollBar::SetRailHotImage(std::wstring pStrImage)
	{
		m_sRailHotImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetRailPushedImage()
	{
		return m_sRailPushedImage;
	}

	void ScrollBar::SetRailPushedImage(std::wstring pStrImage)
	{
		m_sRailPushedImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetRailDisabledImage()
	{
		return m_sRailDisabledImage;
	}

	void ScrollBar::SetRailDisabledImage(std::wstring pStrImage)
	{
		m_sRailDisabledImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetBkNormalImage()
	{
		return m_sBkNormalImage;
	}

	void ScrollBar::SetBkNormalImage(std::wstring pStrImage)
	{
		m_sBkNormalImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetBkHotImage()
	{
		return m_sBkHotImage;
	}

	void ScrollBar::SetBkHotImage(std::wstring pStrImage)
	{
		m_sBkHotImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetBkPushedImage()
	{
		return m_sBkPushedImage;
	}

	void ScrollBar::SetBkPushedImage(std::wstring pStrImage)
	{
		m_sBkPushedImage = pStrImage;
		Invalidate();
	}

	std::wstring ScrollBar::GetBkDisabledImage()
	{
		return m_sBkDisabledImage;
	}

	void ScrollBar::SetBkDisabledImage(std::wstring pStrImage)
	{
		m_sBkDisabledImage = pStrImage;
		Invalidate();
	}

	void ScrollBar::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		SIZE m_cxyFixed = ScrollBar::cxy_fired_sz_;
		if (pm_ != NULL) {
			GetManager()->GetDPIObj()->Scale(&m_cxyFixed);
		}
		rc = rect_;
		if( m_bHorizontal ) {
			int cx = rc.right - rc.left;
			if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
			if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
			if( cx > m_cxyFixed.cy ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cy;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - m_cxyFixed.cy;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				m_rcThumb.top = rc.top;
				m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
				if( m_nRange > 0 ) {
					int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
					if( cxThumb < m_cxyFixed.cy ) cxThumb = m_cxyFixed.cy;

					m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
					m_rcThumb.right = m_rcThumb.left + cxThumb;
					if( m_rcThumb.right > m_rcButton2.left ) {
						m_rcThumb.left = m_rcButton2.left - cxThumb;
						m_rcThumb.right = m_rcButton2.left;
					}
				}
				else {
					m_rcThumb.left = m_rcButton1.right;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else {
				int cxButton = (rc.right - rc.left) / 2;
				if( cxButton > m_cxyFixed.cy ) cxButton = m_cxyFixed.cy;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + cxButton;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - cxButton;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
		else {
			int cy = rc.bottom - rc.top;
			if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
			if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
			if( cy > m_cxyFixed.cx ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				m_rcThumb.left = rc.left;
				m_rcThumb.right = rc.left + m_cxyFixed.cx;
				if( m_nRange > 0 ) {
					int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
					if( cyThumb < m_cxyFixed.cx ) cyThumb = m_cxyFixed.cx;

					m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcThumb.top + cyThumb;
					if( m_rcThumb.bottom > m_rcButton2.top ) {
						m_rcThumb.top = m_rcButton2.top - cyThumb;
						m_rcThumb.bottom = m_rcButton2.top;
					}
				}
				else {
					m_rcThumb.top = m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else {
				int cyButton = (rc.bottom - rc.top) / 2;
				if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + cyButton;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - cyButton;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
	}

	void ScrollBar::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( owner_ != NULL ) owner_->DoEvent(event);
			else Control::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetFocus ) 
		{
			return;
		}
		if( event.type_ == kEventKillFocus ) 
		{
			return;
		}
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick )
		{
			if( !IsEnabled() ) return;

			m_nLastScrollOffset = 0;
			m_nScrollRepeatDelay = 0;
			pm_->SetTimer(this, DEFAULT_TIMERID, 50U);

			if( ::PtInRect(&m_rcButton1, event.mouse_point_) ) {
				m_uButton1State |= UISTATE_PUSHED;
				if( !m_bHorizontal ) {
					if( owner_ != NULL ) owner_->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( owner_ != NULL ) owner_->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcButton2, event.mouse_point_) ) {
				m_uButton2State |= UISTATE_PUSHED;
				if( !m_bHorizontal ) {
					if( owner_ != NULL ) owner_->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( owner_ != NULL ) owner_->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcThumb, event.mouse_point_) ) {
				m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
				last_mouse_pt_ = event.mouse_point_;
				m_nLastScrollPos = m_nScrollPos;
			}
			else {
				if( !m_bHorizontal ) {
					if( event.mouse_point_.y < m_rcThumb.top ) {
						if( owner_ != NULL ) owner_->PageUp(); 
						else SetScrollPos(m_nScrollPos + rect_.top - rect_.bottom);
					}
					else if ( event.mouse_point_.y > m_rcThumb.bottom ){
						if( owner_ != NULL ) owner_->PageDown(); 
						else SetScrollPos(m_nScrollPos - rect_.top + rect_.bottom);                    
					}
				}
				else {
					if( event.mouse_point_.x < m_rcThumb.left ) {
						if( owner_ != NULL ) owner_->PageLeft(); 
						else SetScrollPos(m_nScrollPos + rect_.left - rect_.right);
					}
					else if ( event.mouse_point_.x > m_rcThumb.right ){
						if( owner_ != NULL ) owner_->PageRight(); 
						else SetScrollPos(m_nScrollPos - rect_.left + rect_.right);                    
					}
				}
			}
			if (pm_ != NULL && owner_ == NULL) {
				Notify(kEventScrollChange);
			} 
			return;
		}
		if( event.type_ == kEventMouseButtonUp )
		{
			m_nScrollRepeatDelay = 0;
			m_nLastScrollOffset = 0;
			pm_->KillTimer(this, DEFAULT_TIMERID);

			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
				m_uButton1State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
				m_uButton2State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseMove )
		{
			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				if( !m_bHorizontal ) {
					int vRange = rect_.bottom - rect_.top - m_rcThumb.bottom + m_rcThumb.top - 2 * cxy_fired_sz_.cx;
					if (vRange != 0) m_nLastScrollOffset = (event.mouse_point_.y - last_mouse_pt_.y) * m_nRange / vRange;
				}
				else {
					int hRange = rect_.right - rect_.left - m_rcThumb.right + m_rcThumb.left - 2 * cxy_fired_sz_.cy;
					if (hRange != 0) m_nLastScrollOffset = (event.mouse_point_.x - last_mouse_pt_.x) * m_nRange / hRange;
				}
			}
			else {
				if( (m_uThumbState & UISTATE_HOT) != 0 ) {
					if( !::PtInRect(&m_rcThumb, event.mouse_point_) ) {
						m_uThumbState &= ~UISTATE_HOT;
						Invalidate();
					}
				}
				else {
					if( !IsEnabled() ) return;
					if( ::PtInRect(&m_rcThumb, event.mouse_point_) ) {
						m_uThumbState |= UISTATE_HOT;
						Invalidate();
					}
				}
			}
			return;
		}
		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		if( event.type_ == kEventTimer && event.w_param_ == DEFAULT_TIMERID )
		{
			++m_nScrollRepeatDelay;
			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				if( !m_bHorizontal ) {
					if( owner_ != NULL ) owner_->SetScrollPos(CDuiSize(owner_->GetScrollPos().cx, \
						m_nLastScrollPos + m_nLastScrollOffset)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				else {
					if( owner_ != NULL ) owner_->SetScrollPos(CDuiSize(m_nLastScrollPos + m_nLastScrollOffset, \
						owner_->GetScrollPos().cy)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( owner_ != NULL ) owner_->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( owner_ != NULL ) owner_->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( owner_ != NULL ) owner_->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( owner_ != NULL ) owner_->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else {
				if( m_nScrollRepeatDelay <= 5 ) return;
				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(pm_->GetPaintWindow(), &pt);
				if( !m_bHorizontal ) {
					if( pt.y < m_rcThumb.top ) {
						if( owner_ != NULL ) owner_->PageUp(); 
						else SetScrollPos(m_nScrollPos + rect_.top - rect_.bottom);
					}
					else if ( pt.y > m_rcThumb.bottom ){
						if( owner_ != NULL ) owner_->PageDown(); 
						else SetScrollPos(m_nScrollPos - rect_.top + rect_.bottom);                    
					}
				}
				else {
					if( pt.x < m_rcThumb.left ) {
						if( owner_ != NULL ) owner_->PageLeft(); 
						else SetScrollPos(m_nScrollPos + rect_.left - rect_.right);
					}
					else if ( pt.x > m_rcThumb.right ){
						if( owner_ != NULL ) owner_->PageRight(); 
						else SetScrollPos(m_nScrollPos - rect_.left + rect_.right);                    
					}
				}
			}
			if (pm_ != NULL && owner_ == NULL) {
				Notify(kEventScrollChange);
			}
			return;
		}
		if( event.type_ == kEventMouseEnter )
		{
			if( IsEnabled() ) {
				m_uButton1State |= UISTATE_HOT;
				m_uButton2State |= UISTATE_HOT;
				if( ::PtInRect(&m_rcThumb, event.mouse_point_) ) m_uThumbState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseLeave )
		{
			if( IsEnabled() ) {
				m_uButton1State &= ~UISTATE_HOT;
				m_uButton2State &= ~UISTATE_HOT;
				m_uThumbState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		if( owner_ != NULL ) owner_->DoEvent(event); else Control::DoEvent(event);
	}

	void ScrollBar::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("button1normalimage")) == 0 ) SetButton1NormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button1hotimage")) == 0 ) SetButton1HotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button1pushedimage")) == 0 ) SetButton1PushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button1disabledimage")) == 0 ) SetButton1DisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button2normalimage")) == 0 ) SetButton2NormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button2hotimage")) == 0 ) SetButton2HotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button2pushedimage")) == 0 ) SetButton2PushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("button2disabledimage")) == 0 ) SetButton2DisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbnormalimage")) == 0 ) SetThumbNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbdisabledimage")) == 0 ) SetThumbDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("railnormalimage")) == 0 ) SetRailNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("railhotimage")) == 0 ) SetRailHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("railpushedimage")) == 0 ) SetRailPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("raildisabledimage")) == 0 ) SetRailDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bknormalimage")) == 0 ) SetBkNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bkhotimage")) == 0 ) SetBkHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bkpushedimage")) == 0 ) SetBkPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("bkdisabledimage")) == 0 ) SetBkDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("linesize")) == 0 ) SetLineSize(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("range")) == 0 ) SetScrollRange(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("value")) == 0 ) SetScrollPos(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("showbutton1")) == 0 ) SetShowButton1(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("showbutton2")) == 0 ) SetShowButton2(_tcsicmp(pstrValue, _T("true")) == 0);
		else Control::SetAttribute(strName, strValue);
	}

	void ScrollBar::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&paint_rect_, &rcPaint, &rect_) ) return;
		PaintBk(hDC);
		PaintButton1(hDC);
		PaintButton2(hDC);
		PaintThumb(hDC);
		PaintRail(hDC);
	}

	void ScrollBar::PaintBk(HDC hDC)
	{
		if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~ UISTATE_DISABLED;

		if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
			if( !m_sBkDisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sBkDisabledImage) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
			if( !m_sBkPushedImage.empty() ) {
				if( !DrawImage(hDC, m_sBkPushedImage) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
			if( !m_sBkHotImage.empty() ) {
				if( !DrawImage(hDC, m_sBkHotImage) ) {}
				else return;
			}
		}

		if( !m_sBkNormalImage.empty() ) {
			if( !DrawImage(hDC, m_sBkNormalImage) ) {}
			else return;
		}
	}

	void ScrollBar::PaintButton1(HDC hDC)
	{
		if( !m_bShowButton1 ) return;

		if( !IsEnabled() ) m_uButton1State |= UISTATE_DISABLED;
		else m_uButton1State &= ~ UISTATE_DISABLED;

		int d1 = MulDiv(m_rcButton1.left - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d2 = MulDiv(m_rcButton1.top - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		int d3 = MulDiv(m_rcButton1.right - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d4 = MulDiv(m_rcButton1.bottom - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		image_modify_.clear();
		image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);

		if( (m_uButton1State & UISTATE_DISABLED) != 0 ) {
			if( !m_sButton1DisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sButton1DisabledImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			if( !m_sButton1PushedImage.empty() ) {
				if( !DrawImage(hDC, m_sButton1PushedImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_HOT) != 0 ) {
			if( !m_sButton1HotImage.empty() ) {
				if( !DrawImage(hDC, m_sButton1HotImage, image_modify_) ) {}
				else return;
			}
		}

		if( !m_sButton1NormalImage.empty() ) {
			if( !DrawImage(hDC, m_sButton1NormalImage, image_modify_) ) {}
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		RenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
	}

	void ScrollBar::PaintButton2(HDC hDC)
	{
		if( !m_bShowButton2 ) return;

		if( !IsEnabled() ) m_uButton2State |= UISTATE_DISABLED;
		else m_uButton2State &= ~ UISTATE_DISABLED;
		int d1 = MulDiv(m_rcButton2.left - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d2 = MulDiv(m_rcButton2.top - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		int d3 = MulDiv(m_rcButton2.right - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d4 = MulDiv(m_rcButton2.bottom - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		image_modify_.clear();
		image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"),d1 ,d2 ,d3 ,d4 );

		if( (m_uButton2State & UISTATE_DISABLED) != 0 ) {
			if( !m_sButton2DisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sButton2DisabledImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			if( !m_sButton2PushedImage.empty() ) {
				if( !DrawImage(hDC, m_sButton2PushedImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_HOT) != 0 ) {
			if( !m_sButton2HotImage.empty() ) {
				if( !DrawImage(hDC, m_sButton2HotImage, image_modify_) ) {}
				else return;
			}
		}

		if( !m_sButton2NormalImage.empty() ) {
			if( !DrawImage(hDC, m_sButton2NormalImage, image_modify_) ) {}
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		RenderEngine::DrawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
	}

	void ScrollBar::PaintThumb(HDC hDC)
	{
		if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
		if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~ UISTATE_DISABLED;
		int d1 = MulDiv(m_rcThumb.left - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d2 = MulDiv(m_rcThumb.top - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		int d3 = MulDiv(m_rcThumb.right - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
		int d4 = MulDiv(m_rcThumb.bottom - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
		image_modify_.clear();
		image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), d1, d2, d3, d4);

		if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
			if( !m_sThumbDisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sThumbDisabledImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
			if( !thumb_pushed_image_.empty() ) {
				if( !DrawImage(hDC, thumb_pushed_image_, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
			if( !thumb_hot_image_.empty() ) {
				if( !DrawImage(hDC, thumb_hot_image_, image_modify_) ) {}
				else return;
			}
		}

		if( !m_sThumbNormalImage.empty() ) {
			if( !DrawImage(hDC, m_sThumbNormalImage, image_modify_) ) {}
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		RenderEngine::DrawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
	}

	void ScrollBar::PaintRail(HDC hDC)
	{
		if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
		if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~ UISTATE_DISABLED;

		image_modify_.clear();
		if( !m_bHorizontal ) {
			int d1 = MulDiv(m_rcThumb.left - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
			int d2 = MulDiv((m_rcThumb.top + m_rcThumb.bottom) / 2 - rect_.top - cxy_fired_sz_.cx / 2, 100, GetManager()->GetDPIObj()->GetScale());
			int d3 = MulDiv(m_rcThumb.right - rect_.left, 100, GetManager()->GetDPIObj()->GetScale());
			int d4 = MulDiv((m_rcThumb.top + m_rcThumb.bottom) / 2 - rect_.top + cxy_fired_sz_.cx - cxy_fired_sz_.cx / 2, 100, GetManager()->GetDPIObj()->GetScale());
			image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), d1, d2, d3,d4);
		}
		else {
			int d1 = MulDiv((m_rcThumb.left + m_rcThumb.right) / 2 - rect_.left - cxy_fired_sz_.cy / 2, 100, GetManager()->GetDPIObj()->GetScale());
			int d2 = MulDiv(m_rcThumb.top - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
			int d3 = MulDiv((m_rcThumb.left + m_rcThumb.right) / 2 - rect_.left + cxy_fired_sz_.cy - cxy_fired_sz_.cy / 2, 100, GetManager()->GetDPIObj()->GetScale());
			int d4 = MulDiv(m_rcThumb.bottom - rect_.top, 100, GetManager()->GetDPIObj()->GetScale());
			image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), d1,d2, d3, d4);
		}

		if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
			if( !m_sRailDisabledImage.empty() ) {
				if( !DrawImage(hDC, m_sRailDisabledImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
			if( !m_sRailPushedImage.empty() ) {
				if( !DrawImage(hDC, m_sRailDisabledImage, image_modify_) ) {}
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
			if( !m_sRailHotImage.empty() ) {
				if( !DrawImage(hDC, m_sRailHotImage, image_modify_) ) {}
				else return;
			}
		}

		if( !m_sRailNormalImage.empty() ) {
			if( !DrawImage(hDC, m_sRailNormalImage, image_modify_) ) {}
			else return;
		}
	}
}
