#include "StdAfx.h"
#include "Edit.h"

namespace ui
{
	class EditWnd : public Window
	{
	public:
		EditWnd();

		void Init(Edit* pOwner);
		RECT CalPos();

		std::wstring GetWindowClassName() const;
		std::wstring GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		Edit* owner_;
		HBRUSH m_hBkBrush;
		bool m_bInit;
		bool m_bDrawCaret;
	};


	EditWnd::EditWnd() : owner_(NULL), m_hBkBrush(NULL), m_bInit(false), m_bDrawCaret(false)
	{
	}

	void EditWnd::Init(Edit* pOwner)
	{
		owner_ = pOwner;
		RECT rcPos = CalPos();
		UINT uStyle = 0;
		if(owner_->GetManager()->IsLayered()) {
			uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE;
			RECT rcWnd={0};
			::GetWindowRect(owner_->GetManager()->GetPaintWindow(), &rcWnd);
			rcPos.left += rcWnd.left;
			rcPos.right += rcWnd.left;
			rcPos.top += rcWnd.top - 1;
			rcPos.bottom += rcWnd.top - 1;
		}
		else {
			uStyle = WS_CHILD | ES_AUTOHSCROLL;
		}
		UINT uTextStyle = owner_->GetTextStyle();
		if(uTextStyle & DT_LEFT) uStyle |= ES_LEFT;
		else if(uTextStyle & DT_CENTER) uStyle |= ES_CENTER;
		else if(uTextStyle & DT_RIGHT) uStyle |= ES_RIGHT;
		if( owner_->IsPasswordMode() ) uStyle |= ES_PASSWORD;
		Create(owner_->GetManager()->GetPaintWindow(), L"", uStyle, 0, rcPos);
		HFONT hFont=NULL;
		int iFontIndex=owner_->GetFont();
		if (iFontIndex!=-1)
			hFont = GlobalManager::GetFont(iFontIndex);
		if (hFont == NULL)
			hFont = GlobalManager::GetDefaultFontInfo(::GetDC(wnd_))->handle_;

		SetWindowFont(wnd_, hFont, TRUE);
		Edit_LimitText(wnd_, owner_->GetMaxChar());
		if( owner_->IsPasswordMode() ) Edit_SetPasswordChar(wnd_, owner_->GetPasswordChar());
		Edit_SetText(wnd_, owner_->GetText().c_str());
		Edit_SetModify(wnd_, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(wnd_, owner_->IsEnabled() == true);
		Edit_SetReadOnly(wnd_, owner_->IsReadOnly() == true);

		//Styls
		LONG styleValue = ::GetWindowLong(wnd_, GWL_STYLE);
		styleValue |= pOwner->GetWindowStyls();
		::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
		::ShowWindow(wnd_, SW_SHOWNOACTIVATE);

		int cchLen = ::GetWindowTextLength(wnd_);
		if(cchLen <= 0) cchLen = 1;
		::SetFocus(wnd_);
		::SendMessage(wnd_, EM_SETSEL, 0, cchLen);
		m_bInit = true;    
	}

	RECT EditWnd::CalPos()
	{
		CDuiRect rcPos = owner_->GetPos();
		RECT rcInset = owner_->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		LONG lEditHeight = GlobalManager::GetFontInfo(owner_->GetFont(), ::GetDC(wnd_) )->tm_.tmHeight;
		if( lEditHeight < rcPos.GetHeight() ) {
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}

		Control* pParent = owner_;
		RECT rcParent;
		while( pParent = pParent->GetParent() ) {
			if( !pParent->IsVisible() ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			if( !::IntersectRect(&rcPos, &rcPos, &rcParent) ) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}

		return rcPos;
	}

	std::wstring EditWnd::GetWindowClassName() const
	{
		return _T("EditWnd");
	}

	std::wstring EditWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void EditWnd::OnFinalMessage(HWND hWnd)
	{
		owner_->Invalidate();
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		if (owner_->GetManager()->IsLayered()) {
			owner_->GetManager()->RemovePaintChildWnd(hWnd);
		}
		owner_->wnd_ = NULL;
		delete this;
	}

	LRESULT EditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_CREATE ) {
			if( owner_->GetManager()->IsLayered() ) {
				owner_->GetManager()->AddPaintChildWnd(wnd_);
				::SetTimer(wnd_, CARET_TIMERID, ::GetCaretBlinkTime(), NULL);
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_KILLFOCUS ) lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
				RECT rcClient;
				::GetClientRect(wnd_, &rcClient);
				::InvalidateRect(wnd_, &rcClient, FALSE);
			}
		}
		else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ){
			owner_->Notify(kEventReturn);
		}
		else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_TAB ){
			if (owner_->GetManager()->IsLayered()) {
				owner_->GetManager()->SetNextTabControl();
			}
		}
		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
			if( owner_->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
			::SetBkMode((HDC)wParam, TRANSPARENT);

			DWORD dwTextColor;
			if (owner_->GetNativeEditTextColor() != 0x000000)
				dwTextColor = owner_->GetNativeEditTextColor();
			else
				dwTextColor = owner_->GetTextColor();

			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
			if( m_hBkBrush == NULL ) {
				DWORD clrColor = owner_->GetNativeEditBkColor();
				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
			}
			return (LRESULT)m_hBkBrush;
		}
		else if( uMsg == WM_PRINT ) {
			if (owner_->GetManager()->IsLayered()) {
				lRes = Window::HandleMessage(uMsg, wParam, lParam);
				if( owner_->IsEnabled() && m_bDrawCaret ) {
					RECT rcClient;
					::GetClientRect(wnd_, &rcClient);
					POINT ptCaret;
					::GetCaretPos(&ptCaret);
					RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x, ptCaret.y+rcClient.bottom-rcClient.top };
					RenderEngine::DrawLine((HDC)wParam, rcCaret, 1, 0xFF000000);
				}
				return lRes;
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_TIMER ) {
			if (wParam == CARET_TIMERID) {
				m_bDrawCaret = !m_bDrawCaret;
				RECT rcClient;
				::GetClientRect(wnd_, &rcClient);
				::InvalidateRect(wnd_, &rcClient, FALSE);
				return 0;
			}
			bHandled = FALSE;
		}
		else bHandled = FALSE;

		if( !bHandled ) return Window::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT EditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(wnd_, uMsg, wParam, lParam);
		PostMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT EditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) return 0;
		if( owner_ == NULL ) return 0;
		// Copy text back
		int cchLen = ::GetWindowTextLength(wnd_) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(wnd_, pstr, cchLen);
		owner_->text_ = pstr;
		owner_->Notify(kEventTextChange);
		if( owner_->GetManager()->IsLayered() ) owner_->Invalidate();
		return 0;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	IMPLEMENT_DUICONTROL(Edit)

	Edit::Edit() : wnd_(NULL), max_char_(255), is_readonly_(false), 
		is_password_mode_(false), password_char_(_T('*')), btn_state_(0), 
		bk_color_(0xFFFFFFFF), text_color_(0x00000000), wnd_style_(0),tip_color_(0xFFBAC0C5)
	{
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
	}

	std::wstring Edit::GetClass() const
	{
		return _T("Edit");
	}

	LPVOID Edit::GetInterface(std::wstring pstrName)
	{
		if( _tcsicmp(pstrName.c_str(), DUI_CTR_EDIT) == 0 ) return static_cast<Edit*>(this);
		return Label::GetInterface(pstrName);
	}

	UINT Edit::GetControlFlags() const
	{
		if( !IsEnabled() ) return Control::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void Edit::AttachReturn(const EventCallback & cb)
	{
		on_event_[kEventReturn] += cb;
	}

	void Edit::AttachTextChange(const EventCallback & cb)
	{
		on_event_[kEventTextChange] += cb;
	}

	int Edit::GetFixedWidth()
	{
		if (cxy_fired_sz_.cx == -1 && !normal_image_.empty()) {
			int width = 0;
			const TImageInfo *image_info = pm_->GetImageEx(normal_image_);
			if (image_info == NULL) {
				TDrawInfo draw_info;
				draw_info.Parse(normal_image_);
				if (draw_info.IsValidDest()) {
					width = draw_info.rcDest.right - draw_info.rcDest.left;
				}
				else {
					image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
					width = image_info->nX;
				}
			}
			else {
				width = image_info->nX;
			}

			cxy_fired_sz_.cx = width;

		}

		return __super::GetFixedWidth();
	}

	int Edit::GetFixedHeight()
	{
		if (cxy_fired_sz_.cy == -1 && !normal_image_.empty()) {
			int height = 0;
			const TImageInfo *image_info = pm_->GetImageEx(normal_image_);
			if (image_info == NULL) {
				TDrawInfo draw_info;
				draw_info.Parse(normal_image_);
				if (draw_info.IsValidDest()) {
					height = draw_info.rcDest.bottom - draw_info.rcDest.top;
				}
				else {
					image_info = pm_->AddImage(draw_info.sImageName, draw_info.sResType, draw_info.dwMask, draw_info.bHSL);
					height = image_info->nX;
				}

			}
			else {
				height = image_info->nY;
			}

			cxy_fired_sz_.cy = height;
		}
		return __super::GetFixedHeight();
	}

	void Edit::DoEvent(EventArgs& event)
	{
		if( !IsMouseEnabled() && event.type_ > kEventMouseBegin && event.type_ < kEventMouseEnd ) {
			if( parent_ != NULL ) parent_->DoEvent(event);
			else Label::DoEvent(event);
			return;
		}

		if( event.type_ == kEventSetCursor && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.type_ == kEventWindowSize ){
			if( wnd_ != NULL ) pm_->SetFocusNeeded(this);
		}
		if( event.type_ == kMouseScrollWheel ){
			if( wnd_ != NULL ) return;
		}
		if( event.type_ == kEventSetFocus && IsEnabled() ) {
			if( wnd_ ) return;
			wnd_ = new EditWnd();
			ASSERT(wnd_);
			wnd_->Init(this);
			Invalidate();
		}
		if( event.type_ == kEventKillFocus && IsEnabled() ) {
			Invalidate();
		}
		if( event.type_ == kEventMouseButtonDown || event.type_ == kEventMouseDoubleClick 
			|| event.type_ == kEventMouseRightButtonDown) {
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && wnd_ == NULL ){
					wnd_ = new EditWnd();
					ASSERT(wnd_);
					wnd_->Init(this);

					if( PtInRect(&rect_, event.mouse_point_) )
					{
						int nSize = GetWindowTextLength(*wnd_);
						if( nSize == 0 ) nSize = 1;
						Edit_SetSel(*wnd_, 0, nSize);
					}
				}
				else if( wnd_ != NULL ){
					int nSize = GetWindowTextLength(*wnd_);
					if( nSize == 0 ) nSize = 1;
					Edit_SetSel(*wnd_, 0, nSize);
				}
			}
			return;
		}
		if( event.type_ == kEventMouseMove ) {
			return;
		}

		if( event.type_ == kEventMouseButtonUp ) {
			return;
		}

		if( event.type_ == kEventMouseMenu )
		{
			return;
		}
		if( event.type_ == kEventMouseEnter )
		{
			if( IsEnabled() ) {
				btn_state_ |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.type_ == kEventMouseLeave )
		{
			if( IsEnabled() ) {
				btn_state_ &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		Label::DoEvent(event);
	}

	void Edit::SetEnabled(bool bEnable)
	{
		Control::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			btn_state_ = 0;
		}
	}

	void Edit::SetText(std::wstring pstrText)
	{
		text_ = pstrText;
		if( wnd_ != NULL ) Edit_SetText(*wnd_, text_.c_str());
		Invalidate();
	}

	void Edit::SetMaxChar(UINT uMax)
	{
		max_char_ = uMax;
		if( wnd_ != NULL ) Edit_LimitText(*wnd_, max_char_);
	}

	UINT Edit::GetMaxChar()
	{
		return max_char_;
	}

	void Edit::SetReadOnly(bool bReadOnly)
	{
		if( is_readonly_ == bReadOnly ) return;

		is_readonly_ = bReadOnly;
		if( wnd_ != NULL ) Edit_SetReadOnly(*wnd_, is_readonly_);
		Invalidate();
	}

	bool Edit::IsReadOnly() const
	{
		return is_readonly_;
	}

	void Edit::SetNumberOnly(bool bNumberOnly)
	{
		if( bNumberOnly )
		{
			wnd_style_ |= ES_NUMBER;
		}
		else
		{
			wnd_style_ &= ~ES_NUMBER;
		}
	}

	bool Edit::IsNumberOnly() const
	{
		return wnd_style_&ES_NUMBER ? true:false;
	}

	int Edit::GetWindowStyls() const 
	{
		return wnd_style_;
	}

	void Edit::SetPasswordMode(bool bPasswordMode)
	{
		if( is_password_mode_ == bPasswordMode ) return;
		is_password_mode_ = bPasswordMode;
		Invalidate();
		if( wnd_ != NULL ) 
		{
			LONG styleValue = ::GetWindowLong(*wnd_, GWL_STYLE);
			bPasswordMode ? styleValue |= ES_PASSWORD : styleValue &= ~ES_PASSWORD;
			::SetWindowLong(*wnd_, GWL_STYLE, styleValue);
		}
	}

	bool Edit::IsPasswordMode() const
	{
		return is_password_mode_;
	}

	void Edit::SetPasswordChar(TCHAR cPasswordChar)
	{
		if( password_char_ == cPasswordChar ) return;
		password_char_ = cPasswordChar;
		if( wnd_ != NULL ) Edit_SetPasswordChar(*wnd_, password_char_);
		Invalidate();
	}

	TCHAR Edit::GetPasswordChar() const
	{
		return password_char_;
	}

	std::wstring Edit::GetNormalImage()
	{
		return normal_image_;
	}

	void Edit::SetNormalImage(std::wstring pStrImage)
	{
		normal_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Edit::GetHotImage()
	{
		return hot_image_;
	}

	void Edit::SetHotImage(std::wstring pStrImage)
	{
		hot_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Edit::GetFocusedImage()
	{
		return focused_image_;
	}

	void Edit::SetFocusedImage(std::wstring pStrImage)
	{
		focused_image_ = pStrImage;
		Invalidate();
	}

	std::wstring Edit::GetDisabledImage()
	{
		return disabled_image_;
	}

	void Edit::SetDisabledImage(std::wstring pStrImage)
	{
		disabled_image_ = pStrImage;
		Invalidate();
	}

	void Edit::SetNativeEditBkColor(DWORD dwBkColor)
	{
		bk_color_ = dwBkColor;
	}

	DWORD Edit::GetNativeEditBkColor() const
	{
		return bk_color_;
	}

	void Edit::SetNativeEditTextColor(std::wstring strColor )
	{
		text_color_ = GlobalManager::GetColor(strColor);
	}

	DWORD Edit::GetNativeEditTextColor() const
	{
		return text_color_;
	}

	void Edit::SetSel(long nStartChar, long nEndChar)
	{
		if( wnd_ != NULL ) Edit_SetSel(*wnd_, nStartChar,nEndChar);
	}

	void Edit::SetSelAll()
	{
		SetSel(0,-1);
	}

	void Edit::SetReplaceSel(std::wstring lpszReplace)
	{
		if( wnd_ != NULL ) Edit_ReplaceSel(*wnd_, lpszReplace.c_str());
	}

	void Edit::SetTipValue(std::wstring pStrTipValue )
	{
		tip_	= pStrTipValue;
	}

	std::wstring Edit::GetTipValue()
	{
		if (!IsResourceText()) return tip_;
		return ResourceManager::GetInstance()->GetText(tip_);
	}

	void Edit::SetTipValueColor(std::wstring strColor )
	{
		tip_color_ = GlobalManager::GetColor(strColor);
	}

	DWORD Edit::GetTipValueColor()
	{
		return tip_color_;
	}
	
	
	void Edit::SetPos(RECT rc, bool bNeedInvalidate)
	{
		Control::SetPos(rc, bNeedInvalidate);
		if( wnd_ != NULL ) {
			RECT rcPos = wnd_->CalPos();
			::SetWindowPos(wnd_->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void Edit::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		Control::Move(szOffset, bNeedInvalidate);
		if( wnd_ != NULL ) {
			RECT rcPos = wnd_->CalPos();
			::SetWindowPos(wnd_->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void Edit::SetVisible(bool bVisible)
	{
		Control::SetVisible(bVisible);
		if( !IsVisible() && wnd_ != NULL ) pm_->SetFocus(NULL);
	}

	void Edit::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && wnd_ != NULL ) pm_->SetFocus(NULL);
	}

	SIZE Edit::EstimateSize(SIZE szAvailable)
	{
		if (cxy_fired_sz_.cy == 0) {
			return CDuiSize(cxy_fired_sz_.cx, 
				GlobalManager::GetFontInfo(GetFont(), pm_->GetPaintDC())->tm_.tmHeight + 6);
		}
		return Control::EstimateSize(szAvailable);
	}

	void Edit::SetAttribute(std::wstring strName, std::wstring strValue)
	{
		auto pstrName = strName.c_str();
		auto pstrValue = strValue.c_str();

		if( _tcsicmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("passwordchar")) == 0 ) SetPasswordChar(*pstrValue);
		else if( _tcsicmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcsicmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
		else if( _tcsicmp(pstrName, _T("nativetextcolor")) == 0 ) SetNativeEditTextColor(pstrValue);
		else if( _tcsicmp(pstrName, _T("nativebkcolor")) == 0 ) {
			SetNativeEditBkColor(GlobalManager::GetColor(strValue));
		}
		else Label::SetAttribute(pstrName, pstrValue);
	}

	void Edit::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) btn_state_ |= UISTATE_FOCUSED;
		else btn_state_ &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) btn_state_ |= UISTATE_DISABLED;
		else btn_state_ &= ~ UISTATE_DISABLED;

		if( (btn_state_ & UISTATE_DISABLED) != 0 ) {
			if( !disabled_image_.empty() ) {
				if( !DrawImage(hDC, disabled_image_) ) {}
				else return;
			}
		}
		else if( (btn_state_ & UISTATE_FOCUSED) != 0 ) {
			if( !focused_image_.empty() ) {
				if( !DrawImage(hDC, focused_image_) ) {}
				else return;
			}
		}
		else if( (btn_state_ & UISTATE_HOT) != 0 ) {
			if( !hot_image_.empty() ) {
				if( !DrawImage(hDC, hot_image_) ) {}
				else return;
			}
		}

		if( !normal_image_.empty() ) {
			if( !DrawImage(hDC, normal_image_) ) {}
			else return;
		}
	}

	void Edit::PaintText(HDC hDC)
	{
		DWORD mCurTextColor = text_color_;

		if( text_color_ == 0 ) mCurTextColor = text_color_ = GlobalManager::GetDefaultFontColor();
		if( disabled_text_color_ == 0 ) disabled_text_color_ = GlobalManager::GetDefaultDisabledColor();

		std::wstring sDrawText = GetText();
		std::wstring sTipValue = GetTipValue();
		if(sDrawText == sTipValue || sDrawText == _T("")) {
			mCurTextColor = tip_color_;
			sDrawText = sTipValue;
		}
		else {
			std::wstring sTemp = sDrawText;
			if( is_password_mode_ ) {
				sDrawText.clear();
				auto pStr = sTemp.c_str();
				while( *pStr != _T('\0') ) {
					sDrawText += password_char_;
					pStr = ::CharNext(pStr);
				}
			}
		}

		RECT rc = rect_;
		rc.left += text_padding_rect_.left;
		rc.right -= text_padding_rect_.right;
		rc.top += text_padding_rect_.top;
		rc.bottom -= text_padding_rect_.bottom;
		if( IsEnabled() ) {
			RenderEngine::DrawText(hDC, pm_, rc, sDrawText, mCurTextColor, \
				font_, DT_SINGLELINE | text_style_);
		}
		else {
			RenderEngine::DrawText(hDC, pm_, rc, sDrawText, disabled_text_color_, \
				font_, DT_SINGLELINE | text_style_);
		}
	}
}
