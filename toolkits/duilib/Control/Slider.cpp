#include "StdAfx.h"
#include "Slider.h"

namespace ui
{
	IMPLEMENT_DUICONTROL(Slider)
		Slider::Slider() : btn_state_(0), step_(1),send_move_(false)
	{
		text_style_ = DT_SINGLELINE | DT_CENTER;
		thumb_sz_.cx = thumb_sz_.cy = 10;
	}

	std::wstring Slider::GetClass() const
	{
		return DUI_CTR_SLIDER;
	}

	UINT Slider::GetControlFlags() const
	{
		if( IsEnabled() ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID Slider::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_SLIDER) == 0 ) return static_cast<Slider*>(this);
		return Progress::GetInterface(pstrName);
	}

	void Slider::AttachValueChanged(const EventCallback & cb)
	{
		on_event_[kEventValueChanged] += cb;
	}

	void Slider::AttachValueChangedMove(const EventCallback & cb)
	{
		on_event_[kEventValueChangedMove] += cb;
	}

	void Slider::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	int Slider::GetChangeStep()
	{
		return step_;
	}

	void Slider::SetChangeStep(int step)
	{
		step_ = step;
	}

	void Slider::SetThumbSize(SIZE szXY)
	{
		thumb_sz_ = szXY;
	}

	RECT Slider::GetThumbRect() const
	{
		RECT rcThumb = {0};
		SIZE m_szThumb = Slider::thumb_sz_;
		if (GetManager() != NULL) {
			GetManager()->GetDPIObj()->Scale(&m_szThumb);
		}
		if( m_bHorizontal ) {
			int left = rect_.left + (rect_.right - rect_.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (rect_.bottom + rect_.top - m_szThumb.cy) / 2;
			rcThumb = CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (rect_.right + rect_.left - m_szThumb.cx) / 2;
			int top = rect_.bottom - m_szThumb.cy - (rect_.bottom - rect_.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			rcThumb = CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		if(pm_ != NULL) {
			//m_pManager->GetDPIObj()->Scale(&rcThumb);
		}
		return rcThumb;
	}

	std::wstring Slider::GetThumbImage() const
	{
		return thumb_image_;
	}

	void Slider::SetThumbImage(std::wstring pStrImage)
	{
		thumb_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Slider::GetThumbHotImage() const
	{
		return thumb_hot_image_;
	}

	void Slider::SetThumbHotImage(std::wstring pStrImage)
	{
		thumb_hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Slider::GetThumbPushedImage() const
	{
		return thumb_pushed_image_;
	}

	void Slider::SetThumbPushedImage(std::wstring pStrImage)
	{
		thumb_pushed_image_ = pStrImage;
		Invalidate();
	}

	void Slider::SetValue(int nValue)
	{
		if( (btn_state_ & UISTATE_CAPTURED) != 0 ) return;
		Progress::SetValue(nValue);
	}

	void Slider::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Progress::DoEvent(event);
			return;
		}

		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick ) {
			if( IsEnabled() ) {
				btn_state_ |= UISTATE_CAPTURED;

				int nValue;
				if( m_bHorizontal ) {
					if( event.mouse_point_.x >= rect_.right - thumb_sz_.cx / 2 ) nValue = m_nMax;
					else if( event.mouse_point_.x <= rect_.left + thumb_sz_.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (m_nMax - m_nMin) * (event.mouse_point_.x - rect_.left - thumb_sz_.cx / 2 ) / (rect_.right - rect_.left - thumb_sz_.cx);
				}
				else {
					if( event.mouse_point_.y >= rect_.bottom - thumb_sz_.cy / 2 ) nValue = m_nMin;
					else if( event.mouse_point_.y <= rect_.top + thumb_sz_.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (m_nMax - m_nMin) * (rect_.bottom - event.mouse_point_.y - thumb_sz_.cy / 2 ) / (rect_.bottom - rect_.top - thumb_sz_.cy);
				}
				if(m_nValue != nValue && nValue >= m_nMin && nValue <= m_nMax) {
					m_nValue = nValue;
					Invalidate();
				}
				UpdateText();
			}
			return;
		}

		if( event.type_ == kEventMouseButtonUp || event.type_ == kEventMouseRightButtonUp) {
			if( IsEnabled() ) {
				int nValue = 0;
				if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
					btn_state_ &= ~UISTATE_CAPTURED;
				}
				if( m_bHorizontal ) {
					if( event.mouse_point_.x >= rect_.right - thumb_sz_.cx / 2 ) nValue = m_nMax;
					else if( event.mouse_point_.x <= rect_.left + thumb_sz_.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (m_nMax - m_nMin) * (event.mouse_point_.x - rect_.left - thumb_sz_.cx / 2 ) / (rect_.right - rect_.left - thumb_sz_.cx);
				}
				else {
					if( event.mouse_point_.y >= rect_.bottom - thumb_sz_.cy / 2 ) nValue = m_nMin;
					else if( event.mouse_point_.y <= rect_.top + thumb_sz_.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (m_nMax - m_nMin) * (rect_.bottom - event.mouse_point_.y - thumb_sz_.cy / 2 ) / (rect_.bottom - rect_.top - thumb_sz_.cy);
				}
				if(nValue >= m_nMin && nValue <= m_nMax) {
					m_nValue =nValue;
					Notify(kEventValueChanged);
					Invalidate();
				}
				UpdateText();
				return;
			}
		}
		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		if( event.type_ == kMouseScrollWheel ) 
		{
			if( IsEnabled() ) {
				switch( LOWORD(event.w_param_) ) {
				case SB_LINEUP:
					SetValue(GetValue() + GetChangeStep());
					Notify(kEventValueChanged);
					return;
				case SB_LINEDOWN:
					SetValue(GetValue() - GetChangeStep());
					Notify(kEventValueChanged);
					return;
				}
			}
		}
		if( event.type_ == kEventMouseMove ) {
			if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
				if( m_bHorizontal ) {
					if( event.mouse_point_.x >= rect_.right - thumb_sz_.cx / 2 ) m_nValue = m_nMax;
					else if( event.mouse_point_.x <= rect_.left + thumb_sz_.cx / 2 ) m_nValue = m_nMin;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.mouse_point_.x - rect_.left - thumb_sz_.cx / 2 ) / (rect_.right - rect_.left - thumb_sz_.cx);
				}
				else {
					if( event.mouse_point_.y >= rect_.bottom - thumb_sz_.cy / 2 ) m_nValue = m_nMin;
					else if( event.mouse_point_.y <= rect_.top + thumb_sz_.cy / 2  ) m_nValue = m_nMax;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (rect_.bottom - event.mouse_point_.y - thumb_sz_.cy / 2 ) / (rect_.bottom - rect_.top - thumb_sz_.cy);
				}
				if (send_move_) {
					UpdateText();
					Notify(kEventValueChangedMove);
				}
				Invalidate();
			}

			POINT pt = event.mouse_point_;
			RECT rcThumb = GetThumbRect();
			if( IsEnabled() && ::PtInRect(&rcThumb, event.mouse_point_) ) {
				btn_state_ |= UISTATE_HOT;
				Invalidate();
			}
			else {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventSetCursor )
		{
			RECT rcThumb = GetThumbRect();
			if( IsEnabled()) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
		if( event.type_ == kEventMouseLeave )
		{
			if( IsEnabled() ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		Control::DoEvent(event);
	}

	void Slider::SetCanSendMove(bool bCanSend)
	{
		send_move_ = bCanSend;
	}
	bool Slider::GetCanSendMove() const
	{
		return send_move_;
	}

	void Slider::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("thumbimage")) == 0 ) SetThumbImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbsize")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if( _tcsicmp(pstrName, _T("step")) == 0 ) {
			SetChangeStep(_ttoi(pstrValue));
		}
		else if( _tcsicmp(pstrName, _T("sendmove")) == 0 ) {
			SetCanSendMove(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else Progress::SetAttribute(strName, strValue);
	}

	void Slider::PaintForeImage(HDC hDC)
	{
		Progress::PaintForeImage(hDC);

		RECT rcThumb = GetThumbRect();
		rcThumb.left -= rect_.left;
		rcThumb.top -= rect_.top;
		rcThumb.right -= rect_.left;
		rcThumb.bottom -= rect_.top;

		GetManager()->GetDPIObj()->ScaleBack(&rcThumb);

		if( (btn_state_ & UISTATE_CAPTURED) != 0 ) {
			if( !thumb_pushed_image_.empty() ) {
				image_modify_.clear();
				image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, thumb_pushed_image_, image_modify_) ) {}
				else return;
			}
		}
		else if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( !thumb_hot_image_.empty() ) {
				image_modify_.clear();
				image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
				if( !DrawImage(hDC, thumb_hot_image_, image_modify_) ) {}
				else return;
			}
		}

		if( !thumb_image_.empty() ) {
			image_modify_.clear();
			image_modify_ = nbase::StringPrintf(_T("dest='%d,%d,%d,%d'"), rcThumb.left, rcThumb.top, rcThumb.right, rcThumb.bottom);
			if( !DrawImage(hDC, thumb_image_, image_modify_) ) {}
			else return;
		}
	}
}
